#pragma once

#include <Hobgoblin/Common.hpp>
#include <Hobgoblin/Graphics/Sprite_loader.hpp>
#include <Hobgoblin/Graphics/Render_texture.hpp>
#include <Hobgoblin/Math.hpp>
#include <Hobgoblin/Utility/Grids.hpp>

#include <GridWorld/Model/Cell.hpp>
#include <GridWorld/Model/Lights.hpp>
#include <GridWorld/Model/Sprites.hpp>

#include <optional>
#include <unordered_map>

namespace gridworld {

namespace hg = jbatnozic::hobgoblin;

class IsometricRenderer;

// TODO
//    Idea: turn off tile refreshing (refresh all when turning on)
class World {
public:
    World(hg::PZInteger aWidth, hg::PZInteger aHeight, float aCellResolution)
        : _grid{aWidth, aHeight}
        , _cellResolution{aCellResolution}
    {
    }

    // Cell height (z)

    float getCellResolution() const;

    hg::PZInteger getCellCountX() const;

    hg::PZInteger getCellCountY() const;

    ///////////////////////////////////////////////////////////////////////////
    // CELL GETTERS                                                          //
    ///////////////////////////////////////////////////////////////////////////

//#define TEMPORARY
#ifdef TEMPORARY
    model::Cell& getCellAt(hg::PZInteger aX, hg::PZInteger aY);

    model::Cell& getCellAt(hg::math::Vector2pz aPos) ;

    model::Cell& getCellAtUnchecked(hg::PZInteger aX, hg::PZInteger aY);

    model::Cell& getCellAtUnchecked(hg::math::Vector2pz aPos);
#endif

    const model::Cell& getCellAt(hg::PZInteger aX, hg::PZInteger aY) const;

    const model::Cell& getCellAt(hg::math::Vector2pz aPos) const;

    const model::Cell& getCellAtUnchecked(hg::PZInteger aX, hg::PZInteger aY) const;

    const model::Cell& getCellAtUnchecked(hg::math::Vector2pz aPos) const;

    ///////////////////////////////////////////////////////////////////////////
    // CELL UPDATERS                                                         //
    ///////////////////////////////////////////////////////////////////////////

    // Floor

    void updateCellAt(hg::PZInteger aX,
                      hg::PZInteger aY,
                      const std::optional<model::Cell::Floor>& aFloorOpt);

    void updateCellAt(hg::math::Vector2pz aPos,
                      const std::optional<model::Cell::Floor>& aFloorOpt);

    void updateCellAtUnchecked(hg::PZInteger aX,
                               hg::PZInteger aY,
                               const std::optional<model::Cell::Floor>& aFloorOpt);

    void updateCellAtUnchecked(hg::math::Vector2pz aPos,
                               const std::optional<model::Cell::Floor>& aFloorOpt);

    // Wall

    void updateCellAt(hg::PZInteger aX,
                      hg::PZInteger aY,
                      const std::optional<model::Cell::Wall>& aWallOpt);

    void updateCellAt(hg::math::Vector2pz aPos,
                      const std::optional<model::Cell::Wall>& aWallOpt);

    void updateCellAtUnchecked(hg::PZInteger aX,
                               hg::PZInteger aY,
                               const std::optional<model::Cell::Wall>& aWallOpt);

    void updateCellAtUnchecked(hg::math::Vector2pz aPos,
                               const std::optional<model::Cell::Wall>& aWallOpt);

    ///////////////////////////////////////////////////////////////////////////
    // LIGHTS                                                                //
    ///////////////////////////////////////////////////////////////////////////

    model::LightId createLight(model::SpriteId aSpriteId, hg::math::Vector2pz aSize);

    void updateLight(model::LightId aLightHandle, hg::math::Vector2f aPosition, hg::math::AngleF aAngle);

    void destroyLight(model::LightId aLightHandle);

    // TODO(createRAIILight())

    model::LightDataMapConstIterator lightDataBegin() const {
        return _lights.begin();
    }

    model::LightDataMapConstIterator lightDataEnd() const {
        return _lights.end();
    }

private:
    // ===== Cells =====

    hg::util::RowMajorGrid<model::Cell> _grid;
    float _cellResolution;

    // ===== Lights =====

    model::LightMap _lights;
    model::LightId _lightIdCounter = 0;

    void _renderLight(model::LightData& aLightData);

    void _refreshCellAtUnchecked(hg::PZInteger aX, hg::PZInteger aY);

//public: // TODO(temp)
//    hg::gr::RenderTexture* _renderLight(int aLightHandle);
};

} // namespace gridw
