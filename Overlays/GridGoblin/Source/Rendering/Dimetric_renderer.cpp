// Copyright 2024 Jovan Batnozic. Released under MS-PL licence in Serbia.
// See https://github.com/jbatnozic/Hobgoblin?tab=readme-ov-file#licence

#include <GridGoblin/Rendering/Dimetric_renderer.hpp>
#include <GridGoblin/Rendering/Drawing_order.hpp>
#include <GridGoblin/Spatial/Position_conversions.hpp>

#include "../Detail_access.hpp"

#include <Hobgoblin/HGExcept.hpp>

#include <algorithm>

namespace jbatnozic {
namespace gridgoblin {

// MARK: Cell renderer mask bits

#define RM_FADE_MASK         0x03FF
#define RM_RENDER_CYCLE_FLAG 0x0400
#define RM_CELL_TOUCHED      0x0800
#define RM_SHOULD_REDUCE     0x1000

// MARK: Templates

template <class taCallable>
void DimetricRenderer::_diagonalTraverse(const World&                      aWorld,
                                         const DimetricRenderer::ViewData& aViewData,
                                         taCallable&&                      aFunc) {
    const float cellRes = aWorld.getCellResolution();

    const int cellsPerRow =
        (aViewData.size.x + aViewData.overdraw.left + aViewData.overdraw.right) / (cellRes * 2.f) + 1;
    const int cellsPerColumn =
        (aViewData.size.y + aViewData.overdraw.top + aViewData.overdraw.bottom) * 2.f / cellRes;

    int startX = static_cast<int>(trunc(aViewData.topLeft->x / cellRes));
    int startY = static_cast<int>(trunc(aViewData.topLeft->y / cellRes));

    for (int row = 0; row < cellsPerColumn; row += 1) {
        for (int col = 0; col < cellsPerRow; col += 1) {
            const int x = startX + col;
            const int y = startY + col;

            if (x >= 0 && x < aWorld.getCellCountX() && y >= 0 && y < aWorld.getCellCountY()) {
                const auto* cell = aWorld.getCellAtUnchecked(x, y);

                const auto posInWorld = PositionInWorld{(x + 0.5f) * cellRes, (y + 0.5f) * cellRes};
                const auto posInView  = dimetric::ToPositionInView(posInWorld);

                aFunc(CellInfo{cell, x, y}, posInView);
            }
        }

        if (row % 2 == 0) {
            startX -= 1;
        } else {
            startY += 1;
        }
    }
}

// MARK: Public

DimetricRenderer::DimetricRenderer(const World& aWorld, const hg::gr::SpriteLoader& aSpriteLoader)
    : _world{aWorld}
    , _spriteLoader{aSpriteLoader} {}

void DimetricRenderer::prepareToRenderStart(const hg::gr::View&         aView,
                                            PositionInWorld             aPointOfView,
                                            const OverdrawAmounts&      aOverdrawAmounts,
                                            const VisibilityCalculator& aVisCals) {
    _viewData.center   = PositionInView{aView.getCenter()};
    _viewData.size     = aView.getSize();
    _viewData.overdraw = aOverdrawAmounts;

    _viewData.topLeft = dimetric::ToPositionInWorld(
        PositionInView{_viewData.center->x - (_viewData.size.x / 2.f) - _viewData.overdraw.left,
                       _viewData.center->y - (_viewData.size.y / 2.f) - _viewData.overdraw.top});

    _objectsToRender.clear();
    _cellAdapters.clear();

    _prepareCells(true, true, aPointOfView, &aVisCals);

    _renderCycleCounter += 1;
}

void DimetricRenderer::prepareToRenderEnd() {
    std::sort(_objectsToRender.begin(),
              _objectsToRender.end(),
              [](const RenderedObject* aLhs, const RenderedObject* aRhs) -> bool {
                  // Implements: Does `aLhs` come before `aRhs`?

                  HG_ASSERT(aLhs != nullptr && aRhs != nullptr);

                  const auto order =
                      dimetric::CheckDrawingOrder(aLhs->getSpatialInfo(), aRhs->getSpatialInfo());

                  switch (order) {
                  case dimetric::DRAW_LHS_FIRST:
                      return true;

                  case dimetric::DOES_NOT_MATTER:
                      return (aLhs < aRhs);

                  case dimetric::DRAW_RHS_FIRST:
                      return false;

                  default:
                      HG_UNREACHABLE("Invalid value for drawing order ({}).", (int)order);
                      return {};
                  }
              });
}

void DimetricRenderer::render(hg::gr::Canvas& aCanvas) {
    for (const auto& object : _objectsToRender) {
        const auto& spatialInfo = object->getSpatialInfo();
        object->render(aCanvas, dimetric::ToPositionInView(spatialInfo.getCenter()));
    }
}

// MARK: Private

hg::gr::Sprite& DimetricRenderer::_getSprite(SpriteId aSpriteId) const {
    const auto iter = _spriteCache.find(aSpriteId);
    if (iter != _spriteCache.end()) {
        return iter->second;
    }

    const auto blueprint = _spriteLoader.getBlueprint(aSpriteId);
    const auto newIter   = _spriteCache.emplace(std::make_pair(aSpriteId, blueprint.spr()));
    return newIter.first->second;
}

void DimetricRenderer::_reduceCellsBelowIfCellIsVisible(hg::math::Vector2pz         aCell,
                                                        PositionInView              aCellPosInView,
                                                        PositionInWorld             aPointOfView,
                                                        const VisibilityCalculator& aVisCalc) {
    const auto cr = _world.getCellResolution();

    static constexpr float   PADDING      = 1.f;
    const hg::math::Vector2f positions[4] = {
        {(aCell.x + 0) * cr + PADDING, (aCell.y + 0) * cr + PADDING},
        {(aCell.x + 1) * cr - PADDING, (aCell.y + 0) * cr + PADDING},
        {(aCell.x + 1) * cr - PADDING, (aCell.y + 1) * cr - PADDING},
        {(aCell.x + 0) * cr + PADDING, (aCell.y + 1) * cr - PADDING}
    };

    const bool cellIsVisible =
        std::any_of(std::begin(positions), std::end(positions), [&aVisCalc](const auto& aPos) {
            return aVisCalc.testVisibilityAt(PositionInWorld{aPos}).value_or(false);
        });

    const auto markCell = [this](hg::math::Vector2pz aCell) {
        auto* cell = _world.getCellAt(aCell);
        if (cell) {
            auto& ext  = GetMutableExtensionData(*cell);
            auto  mask = ext.getRendererMask();
            if ((_renderCycleCounter & 0x01) == 0) {
                mask |= (RM_RENDER_CYCLE_FLAG | RM_CELL_TOUCHED | RM_SHOULD_REDUCE);
            } else {
                mask &= ~(RM_RENDER_CYCLE_FLAG | RM_CELL_TOUCHED);
                mask |= RM_SHOULD_REDUCE;
            }
            ext.setRendererMask(mask);
        }
    };

    if (!cellIsVisible) {
        return;
    }

    const auto screenPov = dimetric::ToPositionInView(aPointOfView);
    const int  limit     = 7; // TODO: limit needs to be based on height
    for (int i = 0; i < limit; i += 1) {
        if (screenPov->x > aCellPosInView->x) {
            const int x = aCell.x - 1 - i;
            const int y = aCell.y + 0 + i;

            if (x >= 0 && x < _world.getCellCountX() && y >= 0 && y < _world.getCellCountY()) {
                markCell({x, y});
            }
        }
        {
            const int x = aCell.x - 1 - i;
            const int y = aCell.y + 1 + i;

            if (x >= 0 && x < _world.getCellCountX() && y >= 0 && y < _world.getCellCountY()) {
                markCell({x, y});
            }
        }
        if (screenPov->x < aCellPosInView->x) {
            const int x = aCell.x - 0 - i;
            const int y = aCell.y + 1 + i;

            if (x >= 0 && x < _world.getCellCountX() && y >= 0 && y < _world.getCellCountY()) {
                markCell({x, y});
            }
        }
    } // end_for
}

void DimetricRenderer::_prepareCells(bool                        aPredicate,
                                     bool                        aVisibility,
                                     PositionInWorld             aPointOfView,
                                     const VisibilityCalculator* aVisCalc) {
    const auto cr = _world.getCellResolution();

    _diagonalTraverse(
        _world,
        _viewData,
        [this, cr, aPointOfView, aVisibility, &aVisCalc](const CellInfo& aCellInfo,
                                                         PositionInView  aPosInView) {
            if (aCellInfo.cell == nullptr) {
                return;
            }

            // Adjust mask
            std::uint16_t maskk;
            {
                auto& ext = GetMutableExtensionData(*aCellInfo.cell);
                auto mask = ext.getRendererMask();

                if ((_renderCycleCounter & 0x01) == 0) {
                    const auto expected = RM_RENDER_CYCLE_FLAG | RM_CELL_TOUCHED;
                    if ((mask & expected) != expected) {
                        // cell has not been touched in this cycle
                        mask &= ~RM_SHOULD_REDUCE;
                        mask |= expected;
                    }
                } else {
                    const auto expected = RM_RENDER_CYCLE_FLAG | RM_CELL_TOUCHED;
                    if ((mask & expected) != 0) {
                        // cell has not been touched in this cycle
                        mask &= ~RM_SHOULD_REDUCE;
                        mask &= ~expected;
                    }
                }

                // TODO: inc or dec fade counter

                ext.setRendererMask(mask);

                maskk = mask;
            }

            const auto flags = aCellInfo.cell->getFlags();

            if (!(flags & CellModel::WALL_INITIALIZED)) {
                _reduceCellsBelowIfCellIsVisible({aCellInfo.gridX, aCellInfo.gridY},
                                                 aPosInView,
                                                 aPointOfView,
                                                 *aVisCalc); // TODO
            }

            auto drawingData =
                GetExtensionData(*aCellInfo.cell)
                    .getDrawingData(cr, {aCellInfo.gridX * cr, aCellInfo.gridY * cr}, aPointOfView);
            if (drawingData.state == detail::DrawingData::NONE) {
                return;
            }

            // switch (drawingData.state) {
            // case detail::DrawingData::NONE:
            //     return;

            // case detail::DrawingData::FULL:
            //     if ((maskk & RM_SHOULD_REDUCE) != 0) {
            //         drawingData.state = detail::DrawingData::REDUCED;
            //     }
            //     break;

            // default:
            //     break;
            // }
            if ((maskk & RM_SHOULD_REDUCE) == 0) {
                drawingData.state = detail::DrawingData::FULL;
            } else {
                drawingData.state = detail::DrawingData::REDUCED;
            }

            if (flags & CellModel::WALL_INITIALIZED) {
                _cellAdapters.emplace_back(
                    *this,
                    *aCellInfo.cell,
                    SpatialInfo::fromTopLeftAndSize({aCellInfo.gridX * cr, aCellInfo.gridY * cr},
                                                    {cr, cr},
                                                    Layer::WALL),
                    drawingData.state);
            } else if (flags & CellModel::FLOOR_INITIALIZED) {
                _cellAdapters.emplace_back(
                    *this,
                    *aCellInfo.cell,
                    SpatialInfo::fromTopLeftAndSize({aCellInfo.gridX * cr, aCellInfo.gridY * cr},
                                                    {cr, cr},
                                                    Layer::FLOOR),
                    drawingData.state);
            }
        });

    for (const auto& adapter : _cellAdapters) {
        _objectsToRender.push_back(&adapter);
    }
}

// MARK: Cell adapter impl

DimetricRenderer::CellToRenderedObjectAdapter::CellToRenderedObjectAdapter(
    DimetricRenderer&  aRenderer,
    const CellModel&   aCell,
    const SpatialInfo& aSpatialInfo,
    int                aState)
    : RenderedObject{aSpatialInfo}
    , _renderer{aRenderer}
    , _cell{aCell}
    , _state{aState} {}

void DimetricRenderer::CellToRenderedObjectAdapter::render(hg::gr::Canvas& aCanvas,
                                                           PositionInView  aScreenPosition) const {
    auto* sprite = [this]() -> hg::gr::Sprite* {
        switch (_spatialInfo.getLayer()) {
        case Layer::FLOOR:
            return &_renderer._getSprite(_cell.getFloor().spriteId);
        case Layer::WALL:
            if (_state == detail::DrawingData::FULL) {
                return &_renderer._getSprite(_cell.getWall().spriteId);
            } else {
                return &_renderer._getSprite(_cell.getWall().spriteId_reduced);
            }
        default:
            return nullptr;
        }
    }();

    if (sprite) {
        sprite->setPosition(*aScreenPosition);
        aCanvas.draw(*sprite);
    }
}

} // namespace gridgoblin
} // namespace jbatnozic
