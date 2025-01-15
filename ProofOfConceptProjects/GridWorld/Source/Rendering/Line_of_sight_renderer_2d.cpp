
#include <GridWorld/Rendering/Line_of_sight_renderer_2d.hpp>

#include <Hobgoblin/Format.hpp>
#include <Hobgoblin/Graphics.hpp>
#include <Hobgoblin/HGExcept.hpp>
#include <Hobgoblin/Logging.hpp>
#include <Hobgoblin/Math.hpp>

#include <GL/glew.h>

#include <algorithm>
#include <cmath>

#include "OpenGL_helpers.hpp"

namespace gridworld {

namespace {
constexpr auto LOG_ID = "gridworld";

float MultiplierForPurpose(LineOfSightRenderer2D::Purpose aPurpose) {
    switch (aPurpose) {
    case LineOfSightRenderer2D::FOR_TOPDOWN:
        return 1.f;

    case LineOfSightRenderer2D::FOR_DIMETRIC:
        {
            // (17/16) * sqrt(2)
            static constexpr float MAGIC_DIMETRIC_MULTIPLIER = 1.41421356237f * 17.f / 16.f;
            return MAGIC_DIMETRIC_MULTIPLIER;
        }

    default:
        HG_UNREACHABLE("Invalid value for LineOfSightRenderer2D::Purpose ({}).", (int)aPurpose);
    }
}
} // namespace

LineOfSightRenderer2D::LineOfSightRenderer2D(const World&  aWorld,
                                             hg::PZInteger aTextureSize,
                                             Purpose       aPurpose)
    : _world{aWorld}
    , _sizeMultiplier{MultiplierForPurpose(aPurpose)}
    , _textureSize{aTextureSize} {
    _renderTexture.create({aTextureSize, aTextureSize});

    // Allocate Texture RAM Buffer:
    const auto size = hg::pztos(_textureSize) * hg::pztos(_textureSize) * 4u;
    _textureRamBuffer.resize(size);
    std::memset(_textureRamBuffer.data(), 0x00, _textureRamBuffer.size());

    DualPBO_Init(_pboNames, _textureRamBuffer.size());
}

LineOfSightRenderer2D::~LineOfSightRenderer2D() {
    DualPBO_Destroy(_pboNames);
}

void LineOfSightRenderer2D::start(WorldPosition      aViewPosition,
                                  hg::math::Vector2f aViewSize,
                                  WorldPosition      aLineOfSightOrigin,
                                  float              aPadding) {
    const float width           = aViewSize.x + aPadding;
    const float height          = aViewSize.y + aPadding;
    const float largerDimension = std::max(width, height);

    _recommendedScale = largerDimension * _sizeMultiplier / _renderTexture.getSize().x;
    _losOrigin        = aLineOfSightOrigin;
    _viewCenterOffset = *aViewPosition - _renderTexture.getView().getCenter();

    const float virtualSquareEdge = ceil(largerDimension * _sizeMultiplier);

    hg::gr::View view;
    view.setSize({virtualSquareEdge, virtualSquareEdge});
    view.setCenter(*aViewPosition);
    view.setViewport({0.f, 0.f, 1.f, 1.f});
    _renderTexture.setView(view);

    _stepCounter += 1;
}

void LineOfSightRenderer2D::render() {
    _renderTexture.clear(hg::gr::COLOR_WHITE);

    _renderOcclusion();

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

    _renderTexture.display();

    {
        const unsigned targetPbo = (_stepCounter % 2);
        DualPBO_StartTransfer(_pboNames, targetPbo, _renderTexture.getTexture().getNativeHandle());
    }

    {
        const unsigned targetPbo = ((_stepCounter + 1) % 2);
        DualPBO_LoadIntoRam(_pboNames, targetPbo, _textureRamBuffer.data(), _textureRamBuffer.size());
    }
}

std::optional<bool> LineOfSightRenderer2D::testVisibilityAt(WorldPosition aPos) const {
    const auto pixelPos = _renderTexture.mapCoordsToPixel(*aPos + _viewCenterOffset, 0);

    if (pixelPos.x < 0 || pixelPos.x >= _textureSize || pixelPos.y < 0 || pixelPos.y >= _textureSize) {
        return {};
    }

    // Note: The calculation is as such because the pixel data we get from OpenGL is flipped vertically.
    const auto* p =
        _textureRamBuffer.data() + ((_textureSize - 1 - pixelPos.y) * _textureSize + pixelPos.x) * 4;

    return (hg::gr::Color{p[0], p[1], p[2], 255} != hg::gr::COLOR_BLACK);
}

const hg::gr::Texture& LineOfSightRenderer2D::__gwimpl_getTexture(
    hg::math::Vector2f* aRecommendedScale) const {
    if (aRecommendedScale != nullptr) {
        aRecommendedScale->x = _recommendedScale;
        aRecommendedScale->y = _recommendedScale;
    }
    return _renderTexture.getTexture();
}

void LineOfSightRenderer2D::_renderOcclusion() {
    const auto cellResolution = _world.getCellResolution();
    const auto position       = _renderTexture.getView().getCenter();
    const auto size           = _renderTexture.getView().getSize();

    hg::gr::Vertex vertices[10];
    for (auto& vertex : vertices) {
        vertex.color = hg::gr::COLOR_BLACK;
    }

    const auto startGridX =
        hg::math::Clamp(static_cast<int>(trunc((position.x - size.x / 2.f) / cellResolution)),
                        0,
                        _world.getCellCountX() - 1);
    const auto startGridY =
        hg::math::Clamp(static_cast<int>(trunc((position.y - size.y / 2.f) / cellResolution)),
                        0,
                        _world.getCellCountY() - 1);
    const auto endGridX =
        hg::math::Clamp(static_cast<int>(trunc((position.x + size.x / 2.f) / cellResolution)),
                        0,
                        _world.getCellCountX() - 1);
    const auto endGridY =
        hg::math::Clamp(static_cast<int>(trunc((position.y + size.y / 2.f) / cellResolution)),
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
                    hg::math::Vector2f diff = {vertices[i - 1].position.x - _losOrigin->x,
                                               vertices[i - 1].position.y - _losOrigin->y};
                    diff.x *= 2000.f; // TODO: magic number
                    diff.y *= 2000.f; // TODO: magic number

                    vertices[i].position = {vertices[i - 1].position.x + diff.x,
                                            vertices[i - 1].position.y + diff.y};
                }

                _renderTexture.draw(vertices, 10, hg::gr::PrimitiveType::TRIANGLE_STRIP);
            }
            // ****************************************************
        }
    }
}

} // namespace gridworld
