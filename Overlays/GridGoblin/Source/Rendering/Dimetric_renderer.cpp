// Copyright 2024 Jovan Batnozic. Released under MS-PL licence in Serbia.
// See https://github.com/jbatnozic/Hobgoblin?tab=readme-ov-file#licence

#include <GridGoblin/Rendering/Dimetric_renderer.hpp>
#include <GridGoblin/Rendering/Drawing_order.hpp>
#include <GridGoblin/Spatial/Position_conversions.hpp>

#include "../Detail_access.hpp"

#include <Hobgoblin/HGExcept.hpp>
#include <Hobgoblin/Math/Core.hpp>

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

    _viewData.pointOfView = aPointOfView;

    _objectsToRender.clear();
    _cellAdapters.clear();

    _prepareCells(true, true, &aVisCals);

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

    const auto screenPov = dimetric::ToPositionInView(_viewData.pointOfView);
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
                                     const VisibilityCalculator* aVisCalc) {
    const auto cr = _world.getCellResolution();

    _diagonalTraverse(
        _world,
        _viewData,
        [this, cr, aVisibility, &aVisCalc](const CellInfo& aCellInfo, PositionInView aPosInView) {
            if (aCellInfo.cell == nullptr) {
                return;
            }

            std::uint16_t mask;
            mask = _updateFlagsOfCellRendererMask(*aCellInfo.cell);

            const auto flags = aCellInfo.cell->getFlags();

            if ((flags & CellModel::WALL_INITIALIZED) == 0) {
                _reduceCellsBelowIfCellIsVisible({aCellInfo.gridX, aCellInfo.gridY},
                                                 aPosInView,
                                                 *aVisCalc); // TODO
            }

            const auto drawingData = GetExtensionData(*aCellInfo.cell)
                                         .getDrawingData(cr,
                                                         {aCellInfo.gridX * cr, aCellInfo.gridY * cr},
                                                         _viewData.pointOfView);

            mask = _updateFadeValueOfCellRendererMask(*aCellInfo.cell, drawingData);

            if (drawingData.state == detail::DrawingData::NONE) {
                return;
            }

            if (flags & CellModel::WALL_INITIALIZED) {
                _cellAdapters.emplace_back(
                    *this,
                    *aCellInfo.cell,
                    SpatialInfo::fromTopLeftAndSize({aCellInfo.gridX * cr, aCellInfo.gridY * cr},
                                                    {cr, cr},
                                                    Layer::WALL),
                    mask);
            } else if (flags & CellModel::FLOOR_INITIALIZED) {
                _cellAdapters.emplace_back(
                    *this,
                    *aCellInfo.cell,
                    SpatialInfo::fromTopLeftAndSize({aCellInfo.gridX * cr, aCellInfo.gridY * cr},
                                                    {cr, cr},
                                                    Layer::FLOOR),
                    mask);
            }
        });

    for (const auto& adapter : _cellAdapters) {
        _objectsToRender.push_back(&adapter);
    }
}

std::uint16_t DimetricRenderer::_updateFlagsOfCellRendererMask(const CellModel& aCell) {
    auto& ext  = GetMutableExtensionData(aCell);
    auto  mask = ext.getRendererMask();

    if ((_renderCycleCounter & 0x01) == 0) {
        const auto expected           = RM_RENDER_CYCLE_FLAG | RM_CELL_TOUCHED;
        const bool cellAlreadyTouched = ((mask & expected) == expected);
        if (!cellAlreadyTouched) {
            mask &= ~RM_SHOULD_REDUCE;
            mask |= expected;
        }
    } else {
        const auto expected           = RM_RENDER_CYCLE_FLAG | RM_CELL_TOUCHED;
        const bool cellAlreadyTouched = ((mask & expected) == 0);
        if (!cellAlreadyTouched) {
            mask &= ~RM_SHOULD_REDUCE;
            mask &= ~expected;
        }
    }

    ext.setRendererMask(mask);
    return mask;
}

std::uint16_t DimetricRenderer::_updateFadeValueOfCellRendererMask(
    const CellModel&           aCell,
    const detail::DrawingData& aDrawingData) //
{
    const bool aReduceBasedOnPredicate  = false; // TODO
    const bool aReduceBasedOnVisibility = true;  // TODO

    auto& ext  = GetMutableExtensionData(aCell);
    auto  mask = ext.getRendererMask();

    if (aReduceBasedOnPredicate && aReduceBasedOnVisibility) {
        switch (aDrawingData.state) {
        case detail::DrawingData::NONE:
            mask &= ~RM_SHOULD_REDUCE;
            break;

        case detail::DrawingData::REDUCED:
            mask |= RM_SHOULD_REDUCE;
            break;

        case detail::DrawingData::FULL:
            // mask &= ~RM_SHOULD_REDUCE; -- SKIPPED INTENTIONALLY
            break;

        default:
            HG_UNREACHABLE("Unexpected value for DrawingData::State ({}).", (int)aDrawingData.state);
            break;
        }
    } else if (aReduceBasedOnPredicate) {
        switch (aDrawingData.state) {
        case detail::DrawingData::NONE:
            mask &= ~RM_SHOULD_REDUCE;
            break;

        case detail::DrawingData::REDUCED:
            mask |= RM_SHOULD_REDUCE;
            break;

        case detail::DrawingData::FULL:
            mask &= ~RM_SHOULD_REDUCE;
            break;

        default:
            HG_UNREACHABLE("Unexpected value for DrawingData::State ({}).", (int)aDrawingData.state);
            break;
        }
    } else if (aReduceBasedOnVisibility) {
        // Do nothing - decision is already in RM_SHOULD_REDUCE
    } else {
        mask &= ~RM_SHOULD_REDUCE;
    }

    auto fadeValue = mask & RM_FADE_MASK;
    if ((mask & RM_SHOULD_REDUCE) == 0) {
        if (fadeValue < _fadeConfig.step) {
            fadeValue = 0;
        } else {
            fadeValue -= _fadeConfig.step;
        }
    } else {
        if (fadeValue + _fadeConfig.step > 1023) { // TODO: magic number
            fadeValue = 1023;
        } else {
            fadeValue += _fadeConfig.step;
        }
    }

    mask = (mask & ~RM_FADE_MASK) | fadeValue;

    ext.setRendererMask(mask);
    return mask;
}

// MARK: Cell adapter impl

DimetricRenderer::CellToRenderedObjectAdapter::CellToRenderedObjectAdapter(
    DimetricRenderer&  aRenderer,
    const CellModel&   aCell,
    const SpatialInfo& aSpatialInfo,
    std::uint16_t      aRendererMask)
    : RenderedObject{aSpatialInfo}
    , _renderer{aRenderer}
    , _cell{aCell}
    , _rendererMask{aRendererMask} {}

void DimetricRenderer::CellToRenderedObjectAdapter::render(hg::gr::Canvas& aCanvas,
                                                           PositionInView  aScreenPosition) const {
    if (_spatialInfo.getLayer() == Layer::FLOOR) {
        auto& sprite = _renderer._getSprite(_cell.getFloor().spriteId);
        sprite.setPosition(*aScreenPosition);
        sprite.setColor(hg::gr::COLOR_WHITE);
        aCanvas.draw(sprite);
        return;
    }

    std::uint8_t opacity;
    const auto fadeValue = _rendererMask & RM_FADE_MASK;
    if (fadeValue <= _renderer._fadeConfig.lowerBound) {
        opacity = 255;
    } else if (fadeValue >= _renderer._fadeConfig.upperBound) {
        opacity = static_cast<std::uint8_t>(255 * _renderer._fadeConfig.minimalAlpha);
    } else {
        HG_HARD_ASSERT(_renderer._fadeConfig.upperBound > _renderer._fadeConfig.lowerBound);
        const auto stepCount = _renderer._fadeConfig.upperBound - _renderer._fadeConfig.lowerBound;
        const auto stepsTaken = fadeValue - _renderer._fadeConfig.lowerBound;
        const auto stepReduction = (1.f - _renderer._fadeConfig.minimalAlpha) / stepCount; // TODO a lot of this can be precalculated
        using hobgoblin::math::Clamp;
        opacity = static_cast<std::uint8_t>(Clamp(255.f * (1.f - stepsTaken * stepReduction), 0.f, 255.f));
    }

    hg::gr::Sprite* sprite        = nullptr;
    hg::gr::Sprite* spriteReduced = nullptr;

    if (opacity > 0) {
        sprite = &_renderer._getSprite(_cell.getWall().spriteId);
    }
    if (opacity < 255) {
        spriteReduced = &_renderer._getSprite(_cell.getWall().spriteId_reduced);
    }

    if (spriteReduced) {
        spriteReduced->setPosition(*aScreenPosition);
        spriteReduced->setColor(hg::gr::COLOR_WHITE);
        aCanvas.draw(*spriteReduced);
    }

    if (sprite) {
        sprite->setPosition(*aScreenPosition);
        sprite->setColor(hg::gr::COLOR_WHITE.withAlpha(opacity));
        aCanvas.draw(*sprite);
    }
}

} // namespace gridgoblin
} // namespace jbatnozic
