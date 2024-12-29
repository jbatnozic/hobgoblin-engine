
#include <GridWorld/Coord_conversion.hpp>
#include <GridWorld/Dimetric_transform.hpp>
#include <GridWorld/Rendering/Dimetric_renderer.hpp>

#include <Hobgoblin/Common.hpp>
#include <Hobgoblin/HGExcept.hpp>

#include <cmath>

#include "../Detail_access.hpp"

// TODO(move to some header)
#define WITH(...) [__VA_ARGS__]() -> auto
#define EXEC      ()

namespace gridworld {

void DimetricRenderer::CellToRenderedObjectAdapter::draw(hg::gr::Canvas&    aRenderTarget,
                                                         hg::math::Vector2f aPosition) const {
    GetMutableExtensionData(*_cell);
}

template <class taCallable>
void DimetricRenderer::_diagonalTraverse(const World&                      aWorld,
                                         const DimetricRenderer::ViewData& aViewData,
                                         taCallable&&                      aFunc) {
    const float cellRes = aWorld.getCellResolution();

    const int cellsPerRow    = (aViewData.size.x / (cellRes * 2.f)) + 1;
    const int cellsPerColumn = (aViewData.size.y * 2.f) / cellRes;

    int startX = static_cast<int>(trunc(aViewData.topLeft->x / cellRes));
    int startY = static_cast<int>(trunc(aViewData.topLeft->y / cellRes));

    for (int row = 0; row < cellsPerColumn; row += 1) {
        for (int col = 0; col < cellsPerRow; col += 1) {
            const int x = startX + col;
            const int y = startY + col;

            if (x >= 0 && x < aWorld.getCellCountX() && y >= 0 && y < aWorld.getCellCountY()) {
                const auto* cell = aWorld.getCellAtUnchecked(x, y);
                // TODO: what if cell is null?
                const auto pos =
                    IsometricCoordinatesToScreen({(x + 0.5f) * cellRes, (y + 0.5f) * cellRes});
                aFunc(CellInfo{cell, x, y}, pos);
            }
        }

        if (row % 2 == 0) {
            startX -= 1;
        } else {
            startY += 1;
        }
    }
}

DimetricRenderer::CellToRenderedObjectAdapter::CellToRenderedObjectAdapter(
    const CellModel&   aCell,
    const SpatialInfo& aSpatialInfo)
    : RenderedObject{aSpatialInfo}
    , _cell{&aCell} {}

DimetricRenderer::DimetricRenderer(const World&                aWorld,
                                   const hg::gr::SpriteLoader& aSpriteLoader,
                                   LightingRenderer2D&         aLightingRenderer,
                                   LineOfSightRenderer2D&      aLineOfSightRenderer)
    : _world{aWorld}
    , _spriteLoader{aSpriteLoader}
    , _lightingRenderer{aLightingRenderer}
    , _losRenderer{aLineOfSightRenderer} {}

bool DimetricRenderer::RenderedObjectPtrLess::operator()(const RenderedObject* aLhs,
                                                         const RenderedObject* aRhs) const {
    const auto order = aLhs->getSpatialInfo().checkDimetricDrawingOrder(aRhs->getSpatialInfo());
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

void DimetricRenderer::start(const hg::gr::View& aView, WorldPosition aPointOfView) {
    _viewData.center = aView.getCenter();
    _viewData.size   = aView.getSize();

    _viewData.topLeft = ScreenCoordinatesToIsometric(
        {_viewData.center.x - _viewData.size.x / 2.f, _viewData.center.y - _viewData.size.y / 2.f});
    //_viewData.bottomRight = ScreenCoordinatesToIsometric({_viewData.center->x + _viewData.size.x / 2.f,
    //                                                      _viewData.center->y + _viewData.size.y
    //                                                      / 2.f});

    _viewData.pointOfView = aPointOfView;

    _lightingRenderer.start(ScreenCoordinatesToIsometric(aView.getCenter()),
                            aView.getSize(),
                            0.f); // TODO(use padding parameter)
    _losRenderer.start(ScreenCoordinatesToIsometric(aView.getCenter()),
                       aView.getSize(),
                       aPointOfView,
                       0.f); // TODO(use padding parameter)

    _objectsToRender.clear();
    _cellAdapters.clear();
}

void DimetricRenderer::render(hg::gr::Canvas& aCanvas, int aRenderOptions) {
    _renderFloor(aCanvas);
    _renderLighting(aCanvas);

    _renderWalls(aCanvas, aRenderOptions);

    // render floor lighting
    // render walls & objects
    // ? render ceiling ?
    // render LoS
}

hg::gr::Sprite& DimetricRenderer::_getSprite(SpriteId aSpriteId) const {
    const auto iter = _spriteCache.find(aSpriteId);
    if (iter != _spriteCache.end()) {
        return iter->second;
    }

    const auto blueprint = _spriteLoader.getBlueprint(aSpriteId);
    const auto newIter   = _spriteCache.emplace(std::make_pair(aSpriteId, blueprint.spr()));
    return newIter.first->second;
}

void DimetricRenderer::_renderFloor(hg::gr::Canvas& aCanvas) const {
    _diagonalTraverse(_world,
                      _viewData,
                      [this, &aCanvas](const CellInfo& aCellInfo, hg::math::Vector2f aPos) {
                          if (aCellInfo.cell->isWallInitialized() ||
                              !aCellInfo.cell->isFloorInitialized()) {
                              return;
                          }
                          auto& sprite = _getSprite(aCellInfo.cell->getFloor().spriteId);
                          sprite.setPosition(aPos);
                          aCanvas.draw(sprite);
                      });
}

void DimetricRenderer::_renderLighting(hg::gr::Canvas& aCanvas) const {
    _lightingRenderer.render();

    hg::math::Vector2f scale;
    const auto&        tex = _lightingRenderer.getTexture(&scale);
    hg::gr::Sprite     spr{&tex};
    spr.setOrigin({tex.getSize().x * 0.5f, tex.getSize().y * 0.5f});
    spr.setScale(scale);
    spr.setPosition(ScreenCoordinatesToIsometric(_viewData.center));
    spr.setColor({255, 255, 255, 155});

    //! Multiplicative blending with a dimetric transform.
    //! Note: This cannot be a freestanding global variable, because it depends on
    //!       DIMETRIC_TRANSFORM and BLEND_MULTIPLY, and if those aren't initialized first,
    //!       bad stuff happens...
    static const auto GLOBAL_LIGHTING_RENDER_STATES =
        hg::gr::RenderStates{nullptr, nullptr, DIMETRIC_TRANSFORM, hg::gr::BLEND_MULTIPLY};

    aCanvas.draw(spr, GLOBAL_LIGHTING_RENDER_STATES);
}

bool DimetricRenderer::_isCellObstructed(const CellInfo& aCellInfo) const {
    if (aCellInfo.cell->isWallInitialized()) {
        return true;
    }

    const float              cellres      = _world.getCellResolution();
    const float              offset       = 1.f;
    const hg::math::Vector2f positions[4] = {
        {(aCellInfo.gridX + 0) * cellres + offset, (aCellInfo.gridY + 0) * cellres + offset},
        {(aCellInfo.gridX + 1) * cellres - offset, (aCellInfo.gridY + 0) * cellres + offset},
        {(aCellInfo.gridX + 1) * cellres - offset, (aCellInfo.gridY + 1) * cellres - offset},
        {(aCellInfo.gridX + 0) * cellres + offset, (aCellInfo.gridY + 1) * cellres - offset}
    };
    for (const auto pos : positions) {
        const auto visibility = _losRenderer.testVisibilityAt(pos);
        if (visibility.value_or(false)) {
            return false;
        }
    }
    return true;
}

void DimetricRenderer::_prepareWallsForRendering() const {
    int aRenderOptions = 0; // TODO(temp)

    _diagonalTraverse(_world, _viewData, [](const CellInfo& aCellInfo, auto) {
        GetMutableExtensionData(*aCellInfo.cell).setLowered(false);
        GetMutableExtensionData(*aCellInfo.cell).setVisible(false);
    });

    // Determine new visibility state of all cells in view:
    _diagonalTraverse(
        _world,
        _viewData,
        [this, aRenderOptions](const CellInfo& aCellInfo, auto aScreenPosition) {
            const bool cellIsObstructed = _isCellObstructed(aCellInfo);

            GetMutableExtensionData(*aCellInfo.cell).setVisible(!cellIsObstructed);

            if (cellIsObstructed) {
                return;
            }

            // Loop downwards (as seen on the screen) and mark encountered walls as lowered:
            const auto screenPov = IsometricCoordinatesToScreen(*_viewData.pointOfView);

            const int limit =
                (aRenderOptions & RENDOPT_LOWER_MORE) ? 5 : 1; // TODO: limit needs to be based on height
            for (int i = 0; i < limit; i += 1) {

                if (screenPov.x > aScreenPosition.x) {
                    const int x = aCellInfo.gridX - 1 - i;
                    const int y = aCellInfo.gridY + 0 + i;

                    if (x >= 0 && x < _world.getCellCountX() && y >= 0 && y < _world.getCellCountY()) {
                        GetMutableExtensionData(_world.getCellAtUnchecked(x, y)).setLowered(true);
                    }
                }
                {
                    const int x = aCellInfo.gridX - 1 - i;
                    const int y = aCellInfo.gridY + 1 + i;

                    if (x >= 0 && x < _world.getCellCountX() && y >= 0 && y < _world.getCellCountY()) {
                        GetMutableExtensionData(_world.getCellAtUnchecked(x, y)).setLowered(true);
                    }
                }
                if (screenPov.x < aScreenPosition.x) {
                    const int x = aCellInfo.gridX - 0 - i;
                    const int y = aCellInfo.gridY + 1 + i;

                    if (x >= 0 && x < _world.getCellCountX() && y >= 0 && y < _world.getCellCountY()) {
                        GetMutableExtensionData(_world.getCellAtUnchecked(x, y)).setLowered(true);
                    }
                }
            }

            // Add adapter
            // const float cellres = _world.getCellResolution();
            //_cellAdapters.emplace_back(*aCellInfo.cell,
            //                           SpatialInfo::fromTopLeftAndSize({aCellInfo.gridX * cellres,
            //                           aCellInfo.gridY * cellres},
            //                                                           {cellres, cellres}));
        });
}

void DimetricRenderer::_renderWalls(hg::gr::Canvas& aCanvas, int aRenderOptions) const {
    _losRenderer.render();

    _diagonalTraverse(_world, _viewData, [](const CellInfo& aCellInfo, auto) {
        GetMutableExtensionData(*aCellInfo.cell).setLowered(false);
        GetMutableExtensionData(*aCellInfo.cell).setVisible(false);
    });

    _diagonalTraverse(
        _world,
        _viewData,
        [this, aRenderOptions](const CellInfo& aCellInfo, auto aScreenPosition) {
            const float              cellres      = _world.getCellResolution();
            const float              offset       = 1.f;
            const hg::math::Vector2f positions[4] = {
                {(aCellInfo.gridX + 0) * cellres + offset, (aCellInfo.gridY + 0) * cellres + offset},
                {(aCellInfo.gridX + 1) * cellres - offset, (aCellInfo.gridY + 0) * cellres + offset},
                {(aCellInfo.gridX + 1) * cellres - offset, (aCellInfo.gridY + 1) * cellres - offset},
                {(aCellInfo.gridX + 0) * cellres + offset, (aCellInfo.gridY + 1) * cellres - offset}
            };
            bool cellIsVisible = false;
            for (const auto pos : positions) {
                const auto visibility = _losRenderer.testVisibilityAt(
                    pos); // TODO: if there is a wall, we know there is no visibility!
                if (visibility.value_or(false)) {
                    cellIsVisible = true;
                    break;
                }
            }
            GetMutableExtensionData(*aCellInfo.cell).setVisible(cellIsVisible);

            if (cellIsVisible) {
                const auto screenPov = IsometricCoordinatesToScreen(*_viewData.pointOfView);

                const int limit = (aRenderOptions & RENDOPT_LOWER_MORE)
                                      ? 5
                                      : 1; // TODO: limit needs to be based on height
                for (int i = 0; i < limit; i += 1) {

                    if (screenPov.x > aScreenPosition.x) {
                        const int x = aCellInfo.gridX - 1 - i;
                        const int y = aCellInfo.gridY + 0 + i;

                        if (x >= 0 && x < _world.getCellCountX() && y >= 0 &&
                            y < _world.getCellCountY()) {
                            GetMutableExtensionData(_world.getCellAtUnchecked(x, y)).setLowered(true);
                        }
                    }
                    {
                        const int x = aCellInfo.gridX - 1 - i;
                        const int y = aCellInfo.gridY + 1 + i;

                        if (x >= 0 && x < _world.getCellCountX() && y >= 0 &&
                            y < _world.getCellCountY()) {
                            GetMutableExtensionData(_world.getCellAtUnchecked(x, y)).setLowered(true);
                        }
                    }
                    if (screenPov.x < aScreenPosition.x) {
                        const int x = aCellInfo.gridX - 0 - i;
                        const int y = aCellInfo.gridY + 1 + i;

                        if (x >= 0 && x < _world.getCellCountX() && y >= 0 &&
                            y < _world.getCellCountY()) {
                            GetMutableExtensionData(_world.getCellAtUnchecked(x, y)).setLowered(true);
                        }
                    }
                }
            }
        });

    _diagonalTraverse(
        _world,
        _viewData,
        [this, aRenderOptions, &aCanvas](const CellInfo& aCellInfo, hg::math::Vector2f aPos) {
            if (!aCellInfo.cell->wall.has_value()) {
                return;
            }

            const auto& extData  = GetExtensionData(*aCellInfo.cell);
            const auto  drawMode = WITH(this, aRenderOptions, &aCellInfo, &extData) {
                if (aRenderOptions & RENDOPT_LOWER_MORE) {
                    return extData.isLowered() ? detail::DrawMode::LOWERED : detail::DrawMode::FULL;
                } else {
                    return extData.determineDrawMode(_world.getCellResolution(),
                                                      {aCellInfo.gridX * _world.getCellResolution(),
                                                       aCellInfo.gridY * _world.getCellResolution()},
                                                     *_viewData.pointOfView);
                }
            }
            EXEC;

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

void DimetricRenderer::_renderObjects(hg::gr::Canvas& aCanvas) const {
    for (const auto* object : _objectsToRender) {
        object->draw(aCanvas, IsometricCoordinatesToScreen(*object->getSpatialInfo().getCentre()));
    }
}

} // namespace gridworld
