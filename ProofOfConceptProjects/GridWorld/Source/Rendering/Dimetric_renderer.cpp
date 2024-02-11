
#include <GridWorld/Coord_conversion.hpp>
#include <GridWorld/Rendering/Dimetric_renderer.hpp>
#include <GridWorld/Dimetric_transform.hpp>

#include <Hobgoblin/GSL.hpp>
#include <Hobgoblin/HGExcept.hpp>

#include <cmath>

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
                                   LightingRenderer2D& aLightingRenderer)
    : _world{aWorld}
    , _spriteLoader{aSpriteLoader}
    , _lightingRenderer{aLightingRenderer}
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
    
    hg::math::Vector2f tdlScale;
    const auto& tex = _lightingRenderer.getTexture(&tdlScale);
    hg::gr::Sprite spr{&tex};
    spr.setOrigin({tex.getSize().x * 0.5f, tex.getSize().y * 0.5f});
    spr.setScale(tdlScale);
    spr.setPosition(ScreenCoordinatesToIsometric(_viewData.center));
    spr.setColor({255, 255, 255, 155});
    aCanvas.draw(spr, DIMETRIC_TRANSFORM);
}

void DimetricRenderer::_renderWalls(hg::gr::Canvas& aCanvas) const {
    _diagonalTraverse(_world, _viewData, [this, &aCanvas](const CellInfo& aCellInfo,
                                                          hg::math::Vector2f aPos) {
        if (!aCellInfo.cell->wall.has_value()) {
            return;
        }

        const auto drawMode = aCellInfo.cell->determineDrawMode(
            _world.getCellResolution(),
            {aCellInfo.gridX * _world.getCellResolution(), aCellInfo.gridY * _world.getCellResolution()},
            _viewData.pointOfView
        );

        const auto& wall = *(aCellInfo.cell->wall);

#if 1
        const auto colopt = _lightingRenderer.getColorAt(
            {aCellInfo.gridX * _world.getCellResolution() - 1.f,
             aCellInfo.gridY * _world.getCellResolution() + 1.f}
        );
        const auto col = colopt.value_or(hg::gr::COLOR_BLACK); // TODO(temporary)
#else
        const auto col = hg::gr::COLOR_WHITE;
#endif

        switch (drawMode) {
        case model::DrawMode::NONE:
            break;

        case model::DrawMode::LOWERED:
            {
                auto& sprite = _getSprite(wall.spriteId_lowered);
                sprite.setPosition(aPos);
                sprite.setColor(col);
                aCanvas.draw(sprite);
            }
            break;

        case model::DrawMode::FULL:
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
