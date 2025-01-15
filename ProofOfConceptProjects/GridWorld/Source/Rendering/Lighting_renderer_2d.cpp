
#include <GridWorld/Rendering/Lighting_renderer_2d.hpp>

#include <Hobgoblin/Format.hpp>
#include <Hobgoblin/Graphics.hpp>
#include <Hobgoblin/HGExcept.hpp>
#include <Hobgoblin/Logging.hpp>
#include <Hobgoblin/Math.hpp>

#include <GL/glew.h>

#include <algorithm>
#include <cmath>

#include "../Detail_access.hpp"
#include "OpenGL_helpers.hpp"

namespace gridworld {

namespace {
constexpr auto LOG_ID = "gridworld";

const auto ADDITIVE_DRAW = hg::gr::RenderStates{hg::gr::BLEND_ADD};

float MultiplierForPurpose(LightingRenderer2D::Purpose aPurpose) {
    switch (aPurpose) {
    case LightingRenderer2D::FOR_TOPDOWN:
        return 1.f;

    case LightingRenderer2D::FOR_DIMETRIC:
        {
            // (17/16) * sqrt(2)
            static constexpr float MAGIC_DIMETRIC_MULTIPLIER = 1.41421356237f * 17.f / 16.f;
            return MAGIC_DIMETRIC_MULTIPLIER;
        }

    default:
        HG_UNREACHABLE("Invalid value for LightingRenderer2D::Purpose ({}).", (int)aPurpose);
    }
}
} // namespace

LightingRenderer2D::LightingRenderer2D(const World&                aWorld,
                                       const hg::gr::SpriteLoader& aSpriteLoader,
                                       hg::PZInteger               aTextureSize,
                                       Purpose                     aPurpose)
    : _world{aWorld}
    , _spriteLoader{aSpriteLoader}
    , _sizeMultiplier{MultiplierForPurpose(aPurpose)}
    , _textureSize{aTextureSize} {
    _renderTexture.create({aTextureSize, aTextureSize});

    // Allocate Texture RAM Buffer:
    const auto size = hg::pztos(_textureSize) * hg::pztos(_textureSize) * 4u;
    _textureRamBuffer.resize(size);
    std::memset(_textureRamBuffer.data(), 0x00, _textureRamBuffer.size());

    DualPBO_Init(_pboNames, _textureRamBuffer.size());
}

LightingRenderer2D::~LightingRenderer2D() {
    DualPBO_Destroy(_pboNames);
}

void LightingRenderer2D::start(WorldPosition      aWiewPosition,
                               hg::math::Vector2f aViewSize,
                               float              aPadding) {
    _renderTexture.clear(hg::gr::Color{100, 100, 150, 255});

    const float width           = aViewSize.x + aPadding;
    const float height          = aViewSize.y + aPadding;
    const float largerDimension = std::max(width, height);

    _recommendedScale = largerDimension * _sizeMultiplier / _renderTexture.getSize().x;

    const float virtualSquareEdge = ceil(largerDimension * _sizeMultiplier);

    hg::gr::View view;
    view.setSize({virtualSquareEdge, virtualSquareEdge});
    view.setCenter(*aWiewPosition);
    view.setViewport({0.f, 0.f, 1.f, 1.f});
    _renderTexture.setView(view);

    _stepCounter += 1;
}

void LightingRenderer2D::render() {
    for (auto iter = _world.lightDataBegin(), end = _world.lightDataEnd(); iter != end;
         iter = std::next(iter)) {
        auto& light = iter->second; // TODO(check that this does not do a copy)
        _renderLight(light);
        _drawLight(light);
    }

    // Draw red dot at centre - TODO (remove when not needed anymore)
#if 0
    {
        hg::gr::CircleShape circle{4.f};
        circle.setOrigin({2.f, 2.f});
        circle.setFillColor(hg::gr::COLOR_RED);
        circle.setPosition(_renderTexture.getView().getCenter());
        _renderTexture.draw(circle);
    }
#endif

    /* DISPLAY & START TRANSFER TO PBO */ {
        _renderTexture.display();

        const unsigned targetPbo = (_stepCounter % 2);

        DualPBO_StartTransfer(_pboNames, targetPbo, _renderTexture.getTexture().getNativeHandle());
    }

    /* LOAD PBO INTO RAM BUFFER */ {
        const unsigned targetPbo = ((_stepCounter + 1) % 2);

        DualPBO_LoadIntoRam(_pboNames, targetPbo, _textureRamBuffer.data(), _textureRamBuffer.size());
    }
}

std::optional<hg::gr::Color> LightingRenderer2D::getColorAt(WorldPosition aPos) const {
    auto pixelPos = _renderTexture.mapCoordsToPixel(*aPos, 0);

    if (pixelPos.x < 0 || pixelPos.x >= _textureSize || pixelPos.y < 0 || pixelPos.y >= _textureSize) {
        return {};
    }

    // Note: The calculation is as such because the pixel data we get from OpenGL is flipped vertically.
    const auto* p =
        _textureRamBuffer.data() + ((_textureSize - 1 - pixelPos.y) * _textureSize + pixelPos.x) * 4;

    return hg::gr::Color{p[0], p[1], p[2], 255};
}

const hg::gr::Texture& LightingRenderer2D::getTexture(hg::math::Vector2f* aRecommendedScale) const {
    if (aRecommendedScale != nullptr) {
        aRecommendedScale->x = _recommendedScale;
        aRecommendedScale->y = _recommendedScale;
    }
    return _renderTexture.getTexture();
}

hg::gr::Sprite& LightingRenderer2D::_getSprite(SpriteId aSpriteId) const {
    const auto iter = _spriteCache.find(aSpriteId);
    if (iter != _spriteCache.end()) {
        return iter->second;
    }

    const auto blueprint = _spriteLoader.getBlueprint(aSpriteId);
    const auto newIter   = _spriteCache.emplace(std::make_pair(aSpriteId, blueprint.spr()));
    return newIter.first->second;
}

void LightingRenderer2D::_renderLight(const LightModel& aLightModel) {
    auto&      texture        = GetMutableExtensionData(aLightModel).texture;
    const auto size           = texture.getSize();
    const auto cellResolution = _world.getCellResolution();

    auto& view = texture.getView();
    view.setSize({(float)size.x, (float)size.y}); // TODO: call only once on creation
    view.setCenter(aLightModel.position);
    view.setViewport({0.f, 0.f, 1.f, 1.f});

    texture.clear(hg::gr::COLOR_BLACK);
    {
        auto& sprite = _getSprite(aLightModel.spriteId);
        sprite.setPosition(aLightModel.position);
        texture.draw(sprite);
    }

    hg::gr::Vertex vertices[10];
    for (auto& vertex : vertices) {
        vertex.color = hg::gr::COLOR_BLACK;
    }

    const auto startGridX = hg::math::Clamp(
        static_cast<int>(trunc((aLightModel.position.x - size.x / 2.f) / cellResolution)),
        0,
        _world.getCellCountX() - 1);
    const auto startGridY = hg::math::Clamp(
        static_cast<int>(trunc((aLightModel.position.y - size.y / 2.f) / cellResolution)),
        0,
        _world.getCellCountY() - 1);
    const auto endGridX = hg::math::Clamp(
        static_cast<int>(trunc((aLightModel.position.x + size.x / 2.f) / cellResolution)),
        0,
        _world.getCellCountX() - 1);
    const auto endGridY = hg::math::Clamp(
        static_cast<int>(trunc((aLightModel.position.y + size.y / 2.f) / cellResolution)),
        0,
        _world.getCellCountY() - 1);

    for (hg::PZInteger y = startGridY; y <= endGridY; y += 1) {
        for (hg::PZInteger x = startGridX; x <= endGridX; x += 1) {
            // ****************************************************
            if (const auto* cell = _world.getCellAtUnchecked(x, y);
                cell != nullptr && cell->isWallInitialized()) {
                // clang-format off
                vertices[0].position = { x      * cellResolution,  y      * cellResolution};
                vertices[2].position = { x      * cellResolution, (y + 1) * cellResolution};
                vertices[4].position = {(x + 1) * cellResolution, (y + 1) * cellResolution};
                vertices[6].position = {(x + 1) * cellResolution,  y      * cellResolution};
                vertices[8] = vertices[0];
                // clang-format on

                for (int i = 1; i < 10; i += 2) {
                    hg::math::Vector2f diff = {vertices[i - 1].position.x - aLightModel.position.x,
                                               vertices[i - 1].position.y - aLightModel.position.y};
                    diff.x *= 1000.f; // TODO: magic number
                    diff.y *= 1000.f; // TODO: magic number

                    vertices[i].position = {vertices[i - 1].position.x + diff.x,
                                            vertices[i - 1].position.y + diff.y};
                }

                texture.draw(vertices, 10, hg::gr::PrimitiveType::TRIANGLE_STRIP);
            }
            // ****************************************************
        }
    }

    texture.display();
}

void LightingRenderer2D::_drawLight(const LightModel& aLightModel) {
    hg::gr::Sprite spr{&(GetExtensionData(aLightModel).texture.getTexture())};
    const auto     bounds = spr.getLocalBounds();
    spr.setOrigin({bounds.w / 2.f, bounds.h / 2.f});
    spr.setPosition(aLightModel.position);
    spr.setRotation(aLightModel.angle);
    _renderTexture.draw(spr, ADDITIVE_DRAW);
}

} // namespace gridworld
