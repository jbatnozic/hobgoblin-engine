// Copyright 2024 Jovan Batnozic. Released under MS-PL licence in Serbia.
// See https://github.com/jbatnozic/Hobgoblin?tab=readme-ov-file#licence

#include <GridGoblin/Rendering/Dimetric_renderer.hpp>
#include <GridGoblin/Rendering/Drawing_order.hpp>
#include <GridGoblin/Spatial/Position_conversions.hpp>

#include <Hobgoblin/HGExcept.hpp>

#include <algorithm>

namespace jbatnozic {
namespace gridgoblin {

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

void DimetricRenderer::prepareToRenderStart(const hg::gr::View&    aView,
                                            const OverdrawAmounts& aOverdrawAmounts) {
    _viewData.center   = PositionInView{aView.getCenter()};
    _viewData.size     = aView.getSize();
    _viewData.overdraw = aOverdrawAmounts;

    _viewData.topLeft = dimetric::ToPositionInWorld(
        PositionInView{_viewData.center->x - (_viewData.size.x / 2.f) - _viewData.overdraw.left,
                       _viewData.center->y - (_viewData.size.y / 2.f) - _viewData.overdraw.top});

    _objectsToRender.clear();
    _cellAdapters.clear();

    const auto cellResolution = _world.getCellResolution();

    // Add adapters for floors and walls
    _diagonalTraverse(_world,
                      _viewData,
                      [this, cellResolution](const CellInfo& aCellInfo, PositionInView aPosInView) {
                          if (aCellInfo.cell == nullptr) {
                              return;
                          }
                          const auto flags = aCellInfo.cell->getFlags();
                          if (flags & CellModel::WALL_INITIALIZED) {
                              _cellAdapters.emplace_back(
                                  *this,
                                  *aCellInfo.cell,
                                  SpatialInfo::fromTopLeftAndSize({aCellInfo.gridX * cellResolution,
                                                                   aCellInfo.gridY * cellResolution},
                                                                  {cellResolution, cellResolution},
                                                                  Layer::WALL));
                          } else if (flags & CellModel::FLOOR_INITIALIZED) {
                              _cellAdapters.emplace_back(
                                  *this,
                                  *aCellInfo.cell,
                                  SpatialInfo::fromTopLeftAndSize({aCellInfo.gridX * cellResolution,
                                                                   aCellInfo.gridY * cellResolution},
                                                                  {cellResolution, cellResolution},
                                                                  Layer::FLOOR));
                          }
                      });

    for (const auto& adapter : _cellAdapters) {
        _objectsToRender.push_back(&adapter);
    }
}

void DimetricRenderer::prepareToRenderEnd() {
    std::sort(_objectsToRender.begin(),
              _objectsToRender.end(),
              [](const RenderedObject* aLhs, const RenderedObject* aRhs) -> bool {
                  // Does `aLhs` come before `aRhs`?

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

DimetricRenderer::CellToRenderedObjectAdapter::CellToRenderedObjectAdapter(
    DimetricRenderer&  aRenderer,
    const CellModel&   aCell,
    const SpatialInfo& aSpatialInfo)
    : RenderedObject{aSpatialInfo}
    , _renderer{aRenderer}
    , _cell{aCell} {}

void DimetricRenderer::CellToRenderedObjectAdapter::render(hg::gr::Canvas& aCanvas,
                                                           PositionInView  aScreenPosition) const {
    auto* sprite = [this]() -> hg::gr::Sprite* {
        switch (_spatialInfo.getLayer()) {
        case Layer::FLOOR:
            return &_renderer._getSprite(_cell.getFloor().spriteId);
        case Layer::WALL:
            return &_renderer._getSprite(_cell.getWall().spriteId);
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
