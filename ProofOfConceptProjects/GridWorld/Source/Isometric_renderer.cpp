
#include <GridWorld/Coord_conversion.hpp>
#include <GridWorld/Isometric_renderer.hpp>

#include <Hobgoblin/GSL.hpp>
#include <Hobgoblin/HGExcept.hpp>

#include <cmath>

namespace gridworld {

namespace {
struct CellInfo {
    hg::NotNull<const Cell*> cell;
    hg::PZInteger gridX;
    hg::PZInteger gridY;
};
} // namespace

template <class taCallable>
static void IsometricRenderer::_diagonalTraverse(const World& aWorld,
                                                 const IsometricRenderer::ViewData& aViewData,
                                                 taCallable&& aFunc) {
    const float cellRes = aWorld.getCellResolution();
 
    const int cellsPerRow    = (aViewData.size.y / (cellRes * 2.f)) + 1;
    const int cellsPerColumn = (aViewData.size.x * 2.f) / cellRes;

    int startX = static_cast<int>(trunc(aViewData.isometricTopLeft.x / cellRes));
    int startY = static_cast<int>(trunc(aViewData.isometricTopLeft.y / cellRes));

    for (int row = 0; row < cellsPerColumn; row += 1) {
        for (int col = 0; col < cellsPerRow; col += 1) {
            const int x = startX + col;
            const int y = startY + col;

            if (x >= 0 && x < aWorld.getCellCountX() &&
                y >= 0 && y < aWorld.getCellCountY()) {
                const auto& cell = aWorld.getCellAtUnchecked(x, y);
                const auto pos = IsometricCoordinatesToScreen({x * cellRes, y * cellRes});
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

bool IsometricRenderer::RenderedObjectPtrLess::operator()(const RenderedObject* aLhs,
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

void IsometricRenderer::start(const hg::gr::View& aView) {
    // clear all data
    _viewData.center = aView.getCenter();
    _viewData.size   = aView.getSize();

    _viewData.isometricTopLeft     = ScreenCoordinatesToIsometric({_viewData.center.x - _viewData.size.x / 2.f,
                                                                   _viewData.center.y - _viewData.size.y / 2.f});
    _viewData.isometricBottomRight = ScreenCoordinatesToIsometric({_viewData.center.x + _viewData.size.x / 2.f,
                                                                   _viewData.center.y + _viewData.size.y / 2.f});
}

void IsometricRenderer::render(hg::gr::Canvas& aCanvas) {
    _renderFloor(aCanvas);

    // render floor lighting
    // render walls & objects
    // ? render ceiling ?
    // render LoS
}

hg::gr::Sprite& IsometricRenderer::_getSprite(SpriteId aSpriteId) const {
    const auto iter = _spriteCache.find(aSpriteId);
    if (iter != _spriteCache.end()) {
        return iter->second;
    }

    const auto blueprint = _spriteLoader.getBlueprint(aSpriteId);
    const auto newIter = _spriteCache.emplace(std::make_pair(aSpriteId, blueprint.spr()));
    auto& sprite = newIter.first->second;
    auto bounds = sprite.getLocalBounds();
    sprite.setOrigin({bounds.w / 2.f - _world.getCellResolution(), bounds.h / 2.f});
    return newIter.first->second;
}

void IsometricRenderer::_renderFloor(hg::gr::Canvas& aCanvas) const {
    _diagonalTraverse(_world, _viewData, [this, &aCanvas](const CellInfo& aCellInfo, hg::math::Vector2f aPos) {
        if (aCellInfo.cell->wall.has_value()) {
            return;
        }
        auto& sprite = _getSprite(aCellInfo.cell->floor.spriteId);
        sprite.setPosition(aPos);
        aCanvas.draw(sprite);
    });
}

} // namespace gridw
