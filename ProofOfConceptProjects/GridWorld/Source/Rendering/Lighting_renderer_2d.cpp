
#include <GridWorld/Rendering/Lighting_renderer_2d.hpp>
#include <GridWorld/Coord_conversion.hpp>

#include <Hobgoblin/HGExcept.hpp>
#include <Hobgoblin/Graphics.hpp>
#include <Hobgoblin/Math.hpp>

#include <algorithm>
#include <cmath>

namespace gridworld {

namespace {
const auto ADDITIVE_DRAW = hg::gr::RenderStates{hg::gr::BLEND_ADD};

float MultiplierForPurpose(LightingRenderer2D::Purpose aPurpose) {
    switch (aPurpose) {
        case LightingRenderer2D::FOR_TOPDOWN:
            return 1.f;

        case LightingRenderer2D::FOR_DIMETRIC:
            //return 2.f;
            return 1.41421356237f; // sqrt(2)

        default:
            HG_UNREACHABLE("Invalid value for LightingRenderer2D::Purpose ({}).", (int)aPurpose);
    }
}
} // namespace

LightingRenderer2D::LightingRenderer2D(const World& aWorld,
                                       const hg::gr::SpriteLoader& aSpriteLoader,
                                       hg::PZInteger aTextureSize,
                                       Purpose aPurpose)
    : _world{aWorld}
    , _spriteLoader{aSpriteLoader}
    , _sizeMultiplier{MultiplierForPurpose(aPurpose)}
{
    _renderTexture.create({aTextureSize, aTextureSize});
}

void LightingRenderer2D::start(hg::math::Vector2f aWorldPosition,
                               hg::math::Vector2f aViewSize,
                               float aPadding) {
    _renderTexture.clear();

    const float width  = aViewSize.x + aPadding;
    const float height = aViewSize.y + aPadding;
    const float largerDimension = std::max(width, height);

    _recommendedScale = largerDimension * _sizeMultiplier / _renderTexture.getSize().x;

    const float virtualSquareEdge = ceil(largerDimension * _sizeMultiplier);

    hg::gr::View view;
    view.setSize({virtualSquareEdge, virtualSquareEdge});
    view.setCenter(aWorldPosition);
    view.setViewport({0.f, 0.f, 1.f, 1.f});
    _renderTexture.setView(view);
}

void LightingRenderer2D::render() {
    for (auto iter = _world.lightDataBegin(), end = _world.lightDataEnd(); iter != end; iter = std::next(iter)) {
        auto& light = iter->second; // TODO(check that this does not do a copy)
        _renderLight(light);
        _drawLight(light);
    }

    // TODO (remove when not needed anymore)
#if 0
    {
        hg::gr::CircleShape circle{4.f};
        circle.setOrigin({2.f, 2.f});
        circle.setFillColor(hg::gr::COLOR_RED);
        circle.setPosition(_renderTexture.getView().getCenter());
        _renderTexture.draw(circle);
    }
#endif

    _renderTexture.display();

    _image = _renderTexture.getTexture().copyToImage();
}

std::optional<hg::gr::Color> LightingRenderer2D::getColorAt(hg::math::Vector2f aPos) const {
    auto pixelPos = _renderTexture.mapCoordsToPixel(aPos, 0);

    if (pixelPos.x < 0 || pixelPos.x >= _image.getSize().x ||
        pixelPos.y < 0 || pixelPos.y >= _image.getSize().y)
    {
        return {};
    }

    return _image.getPixel(pixelPos.x, pixelPos.y);
}

const hg::gr::Texture& LightingRenderer2D::getTexture(hg::math::Vector2f* aRecommendedScale) const {
    if (aRecommendedScale != nullptr) {
        aRecommendedScale->x = _recommendedScale;
        aRecommendedScale->y = _recommendedScale;
    }
    return _renderTexture.getTexture();
}

hg::gr::Sprite& LightingRenderer2D::_getSprite(model::SpriteId aSpriteId) const {
    const auto iter = _spriteCache.find(aSpriteId);
    if (iter != _spriteCache.end()) {
        return iter->second;
    }

    const auto blueprint = _spriteLoader.getBlueprint(aSpriteId);
    const auto newIter = _spriteCache.emplace(std::make_pair(aSpriteId, blueprint.spr()));
    return newIter.first->second;
}

void LightingRenderer2D::_renderLight(const model::LightData& aLightData) {
    const auto size = aLightData.texture.getSize();
    const auto cellResolution = _world.getCellResolution();

    auto& view = aLightData.texture.getView();
    view.setSize({(float)size.x, (float)size.y}); // TODO: call only once on creation
    view.setCenter(aLightData.position);
    view.setViewport({0.f, 0.f, 1.f, 1.f});

    aLightData.texture.clear(hg::gr::COLOR_BLACK);

    {
        auto& sprite = _getSprite(aLightData.spriteId);
        sprite.setPosition(aLightData.position);
        aLightData.texture.draw(sprite);
    }

    hg::gr::Vertex vertices[10];
    for (auto& vertex : vertices) {
        vertex.color = hg::gr::COLOR_BLACK;
    }

    const auto startGridX = 
        hg::math::Clamp(static_cast<int>(trunc((aLightData.position.x - size.x / 2.f) / cellResolution)), 
                        0, 
                        _world.getCellCountX() - 1
        );
    const auto startGridY =
        hg::math::Clamp(static_cast<int>(trunc((aLightData.position.y - size.y / 2.f) / cellResolution)),
                        0,
                        _world.getCellCountY() - 1
        );
    const auto endGridX = 
        hg::math::Clamp(static_cast<int>(trunc((aLightData.position.x + size.x / 2.f) / cellResolution)), 
                        0, 
                        _world.getCellCountX() - 1
        );
    const auto endGridY =
        hg::math::Clamp(static_cast<int>(trunc((aLightData.position.y + size.y / 2.f) / cellResolution)),
                        0,
                        _world.getCellCountY() - 1
        );

    for (hg::PZInteger y = startGridY; y <= endGridY; y += 1) {
        for (hg::PZInteger x = startGridX; x <= endGridX; x += 1) {
            // ****************************************************
            if (auto wall = _world.getCellAtUnchecked(x, y).wall) {
                vertices[0].position = { x      * cellResolution,  y      * cellResolution};
                vertices[2].position = { x      * cellResolution, (y + 1) * cellResolution};
                vertices[4].position = {(x + 1) * cellResolution, (y + 1) * cellResolution};
                vertices[6].position = {(x + 1) * cellResolution,  y      * cellResolution};
                vertices[8] = vertices[0];

                for (int i = 1; i < 10; i += 2) {
                    hg::math::Vector2f diff = {vertices[i - 1].position.x - aLightData.position.x,
                        vertices[i - 1].position.y - aLightData.position.y};
                    diff.x *= 1000.f;
                    diff.y *= 1000.f;

                    vertices[i].position = {vertices[i - 1].position.x + diff.x,
                        vertices[i - 1].position.y + diff.y};
                }

                aLightData.texture.draw(vertices, 10, hg::gr::PrimitiveType::TriangleStrip);
            }
            // ****************************************************
        }
    }

    aLightData.texture.display();
}

void LightingRenderer2D::_drawLight(const model::LightData& aLightData) {
    hg::gr::Sprite spr{&(aLightData.texture.getTexture())};
    const auto bounds = spr.getLocalBounds();
    spr.setOrigin({bounds.w / 2.f, bounds.h / 2.f});
    spr.setPosition(aLightData.position);
    spr.setRotation(aLightData.angle);
    _renderTexture.draw(spr, ADDITIVE_DRAW);
}

} // namespace gridworld
