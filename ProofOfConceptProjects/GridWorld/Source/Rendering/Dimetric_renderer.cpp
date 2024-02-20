
#include <GridWorld/Coord_conversion.hpp>
#include <GridWorld/Rendering/Dimetric_renderer.hpp>
#include <GridWorld/Dimetric_transform.hpp>

#include <Hobgoblin/GSL.hpp>
#include <Hobgoblin/HGExcept.hpp>

#include <cmath>

#include "../Detail_access.hpp"

namespace gridworld {

namespace {
struct CellInfo {
    hg::NotNull<const model::Cell*> cell;
    hg::PZInteger gridX;
    hg::PZInteger gridY;
};
} // namespace

template <class taCallable>
static void DimetricRenderer::_diagonalTraverse(const World& aWorld,
                                                const DimetricRenderer::ViewData& aViewData,
                                                taCallable&& aFunc) {
    const float cellRes = aWorld.getCellResolution();
 
    const int cellsPerRow    = (aViewData.size.x / (cellRes * 2.f)) + 1;
    const int cellsPerColumn = (aViewData.size.y * 2.f) / cellRes;

    int startX = static_cast<int>(trunc(aViewData.isometricTopLeft.x / cellRes));
    int startY = static_cast<int>(trunc(aViewData.isometricTopLeft.y / cellRes));

    for (int row = 0; row < cellsPerColumn; row += 1) {
        for (int col = 0; col < cellsPerRow; col += 1) {
            const int x = startX + col;
            const int y = startY + col;

            if (x >= 0 && x < aWorld.getCellCountX() &&
                y >= 0 && y < aWorld.getCellCountY()) {
                const auto& cell = aWorld.getCellAtUnchecked(x, y);
                const auto pos = IsometricCoordinatesToScreen({(x + 0.5f) * cellRes, (y + 0.5f) * cellRes});
                aFunc(CellInfo{&cell, x, y}, pos);
            }
        }

        if (row % 2 == 0) {
            startX -= 1;
        } else {
            startY += 1;
        }
    }
}

DimetricRenderer::DimetricRenderer(const World& aWorld,
                                   const hg::gr::SpriteLoader& aSpriteLoader,
                                   LightingRenderer2D& aLightingRenderer,
                                   LineOfSightRenderer2D& aLineOfSightRenderer)
    : _world{aWorld}
    , _spriteLoader{aSpriteLoader}
    , _lightingRenderer{aLightingRenderer}
    , _losRenderer{aLineOfSightRenderer}
{
}

bool DimetricRenderer::RenderedObjectPtrLess::operator()(const RenderedObject* aLhs,
                                                         const RenderedObject* aRhs) const {
    const auto order = aLhs->spatialInfo.checkIsometricDrawingOrder(aRhs->spatialInfo);
    switch (order) {
    case SpatialInfo::DRAW_OTHER_FIRST:
        return false;

    case SpatialInfo::DOES_NOT_MATTER:
        return (aLhs < aRhs);

    case SpatialInfo::DRAW_THIS_FIRST:
        return true;

    default:
        HG_UNREACHABLE("Invalid value for drawing order ({}).", (int)order);
        return {};
    }
}

void DimetricRenderer::start(const hg::gr::View& aView, hg::math::Vector2f aPointOfView) {
    // clear all data
    _viewData.center = aView.getCenter();
    _viewData.size   = aView.getSize();

    _viewData.isometricTopLeft     = ScreenCoordinatesToIsometric({_viewData.center.x - _viewData.size.x / 2.f,
                                                                   _viewData.center.y - _viewData.size.y / 2.f});
    _viewData.isometricBottomRight = ScreenCoordinatesToIsometric({_viewData.center.x + _viewData.size.x / 2.f,
                                                                   _viewData.center.y + _viewData.size.y / 2.f});

    _viewData.pointOfView = aPointOfView;

    _lightingRenderer.start(ScreenCoordinatesToIsometric(aView.getCenter()), 
                            aView.getSize(),
                            0.f); // TODO(use padding parameter)
    _losRenderer.start(ScreenCoordinatesToIsometric(aView.getCenter()),
                       aView.getSize(),
                       aPointOfView,
                       0.f); // TODO(use padding parameter)
}

void DimetricRenderer::render(hg::gr::Canvas& aCanvas) {
    _renderFloor(aCanvas);
    _renderLighting(aCanvas);
    _renderWalls(aCanvas);

    // render floor lighting
    // render walls & objects
    // ? render ceiling ?
    // render LoS
}

hg::gr::Sprite& DimetricRenderer::_getSprite(model::SpriteId aSpriteId) const {
    const auto iter = _spriteCache.find(aSpriteId);
    if (iter != _spriteCache.end()) {
        return iter->second;
    }

    const auto blueprint = _spriteLoader.getBlueprint(aSpriteId);
    const auto newIter = _spriteCache.emplace(std::make_pair(aSpriteId, blueprint.spr()));
    return newIter.first->second;
}

void DimetricRenderer::_renderFloor(hg::gr::Canvas& aCanvas) const {
    _diagonalTraverse(_world, _viewData, [this, &aCanvas](const CellInfo& aCellInfo, hg::math::Vector2f aPos) {
        if (aCellInfo.cell->wall.has_value() || !aCellInfo.cell->floor.has_value()) {
            return;
        }
        auto& sprite = _getSprite(aCellInfo.cell->floor->spriteId);
        sprite.setPosition(aPos);
        aCanvas.draw(sprite);
    });
}

void DimetricRenderer::_renderLighting(hg::gr::Canvas& aCanvas) const {
    _lightingRenderer.render();
    
    hg::math::Vector2f scale;
    const auto& tex = _lightingRenderer.getTexture(&scale);
    hg::gr::Sprite spr{&tex};
    spr.setOrigin({tex.getSize().x * 0.5f, tex.getSize().y * 0.5f});
    spr.setScale(scale);
    spr.setPosition(ScreenCoordinatesToIsometric(_viewData.center));
    spr.setColor({255, 255, 255, 155});

    //! Multiplicative blending with a dimetric transform.
    //! Note: This cannot be a freestanding global variable, because it depends on
    //!       DIMETRIC_TRANSFORM and BLEND_MULTIPLY, and if those aren't initialized first,
    //!       bad stuff happens...
    static const auto GLOBAL_LIGHTING_RENDER_STATES = hg::gr::RenderStates{
        nullptr,
        nullptr,
        DIMETRIC_TRANSFORM,
        hg::gr::BLEND_MULTIPLY
    };

    aCanvas.draw(spr, GLOBAL_LIGHTING_RENDER_STATES);
}

void DimetricRenderer::_renderWalls(hg::gr::Canvas& aCanvas) const {
    _losRenderer.render();

    _diagonalTraverse(_world, _viewData, [](const CellInfo& aCellInfo, auto) {
        GetMutableExtensionData(*aCellInfo.cell).setLowered(false);
        GetMutableExtensionData(*aCellInfo.cell).setVisible(false);
    });

    _diagonalTraverse(_world, _viewData, [this](const CellInfo& aCellInfo, auto) {
        const float cellres = _world.getCellResolution();
        const float offset = 1.f;
        const hg::math::Vector2f positions[4] = { // TODO: do top row then bottom row (cache efficiency)
            {(aCellInfo.gridX + 0) * cellres + offset, (aCellInfo.gridY + 0) * cellres + offset},
            {(aCellInfo.gridX + 1) * cellres - offset, (aCellInfo.gridY + 0) * cellres + offset},
            {(aCellInfo.gridX + 1) * cellres - offset, (aCellInfo.gridY + 1) * cellres - offset},
            {(aCellInfo.gridX + 0) * cellres + offset, (aCellInfo.gridY + 1) * cellres - offset}
        };
        bool cellIsVisible = false;
        for (const auto pos : positions) {
            const auto visibility = _losRenderer.testVisibilityAt(pos); // TODO: if there is a wall, we know there is no visibility!
            if (visibility.value_or(false)) {
                cellIsVisible = true;
                break;
            }
        }
        GetMutableExtensionData(*aCellInfo.cell).setVisible(cellIsVisible);

        if (cellIsVisible) {
            for (int i = 0; i < 5; i += 1) { // TODO: limit needs to be based on height
                if (i > 0) {
                    const int x = aCellInfo.gridX - i;
                    const int y = aCellInfo.gridY + i - 1;

                    if (x >= 0 && x < _world.getCellCountX() &&
                        y >= 0 && y < _world.getCellCountY()) {
                        GetMutableExtensionData(_world.getCellAtUnchecked(x, y)).setLowered(true);
                    }
                }
                {
                    const int x = aCellInfo.gridX - i;
                    const int y = aCellInfo.gridY + i;

                    if (x >= 0 && x < _world.getCellCountX() &&
                        y >= 0 && y < _world.getCellCountY()) {
                        GetMutableExtensionData(_world.getCellAtUnchecked(x, y)).setLowered(true);
                    }
                }
                if (i > 0) {
                    const int x = aCellInfo.gridX - i + 1;
                    const int y = aCellInfo.gridY + i;

                    if (x >= 0 && x < _world.getCellCountX() &&
                        y >= 0 && y < _world.getCellCountY()) {
                        GetMutableExtensionData(_world.getCellAtUnchecked(x, y)).setLowered(true);
                    }
                }
            }
        }
    });

    _diagonalTraverse(_world, _viewData, [this, &aCanvas](const CellInfo& aCellInfo,
                                                          hg::math::Vector2f aPos) {
        if (!aCellInfo.cell->wall.has_value()) {
            return;
        }

        //const auto drawMode = aCellInfo.cell->determineDrawMode(
        //    _world.getCellResolution(),
        //    {aCellInfo.gridX * _world.getCellResolution(), aCellInfo.gridY * _world.getCellResolution()},
        //    _viewData.pointOfView
        //);

        const auto drawMode = GetExtensionData(*aCellInfo.cell).isLowered() ? detail::DrawMode::LOWERED
                                                                            : detail::DrawMode::FULL;

        const auto& wall = *(aCellInfo.cell->wall);

#if 0
        const auto colopt = _lightingRenderer.getColorAt(
            {aCellInfo.gridX * _world.getCellResolution() - 2.f,
             aCellInfo.gridY * _world.getCellResolution() + 2.f}
        );
        const auto col = colopt.value_or(hg::gr::COLOR_BLACK); // TODO(temporary)
#else
        const auto col = hg::gr::COLOR_WHITE;
#endif

        switch (drawMode) {
        case detail::DrawMode::NONE:
            break;

        case detail::DrawMode::LOWERED:
            {
                auto& sprite = _getSprite(wall.spriteId_lowered);
                sprite.setPosition(aPos);
                sprite.setColor(col);
                aCanvas.draw(sprite);
            }
            break;

        case detail::DrawMode::FULL:
            {
                auto& sprite = _getSprite(wall.spriteId);
                sprite.setPosition(aPos);
                sprite.setColor(col);
                aCanvas.draw(sprite);
            }
            break;

        default:
            HG_UNREACHABLE("Invalid value for DrawMode ({}).", (int)drawMode);
        }
    });
}

} // namespace gridw
