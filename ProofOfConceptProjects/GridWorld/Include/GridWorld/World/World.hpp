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

    void toggleGeneratorMode(bool aGeneratorModeActive);

    ///////////////////////////////////////////////////////////////////////////
    // CELL GETTERS                                                          //
    ///////////////////////////////////////////////////////////////////////////

    const CellModel& getCellAt(hg::PZInteger aX, hg::PZInteger aY) const;

    const CellModel& getCellAt(hg::math::Vector2pz aPos) const;

    const CellModel& getCellAtUnchecked(hg::PZInteger aX, hg::PZInteger aY) const;

    const CellModel& getCellAtUnchecked(hg::math::Vector2pz aPos) const;

    ///////////////////////////////////////////////////////////////////////////
    // CELL UPDATERS                                                         //
    ///////////////////////////////////////////////////////////////////////////

    // Floor

    void updateCellAt(hg::PZInteger aX,
                      hg::PZInteger aY,
                      const std::optional<CellModel::Floor>& aFloorOpt);

    void updateCellAt(hg::math::Vector2pz aPos,
                      const std::optional<CellModel::Floor>& aFloorOpt);

    void updateCellAtUnchecked(hg::PZInteger aX,
                               hg::PZInteger aY,
                               const std::optional<CellModel::Floor>& aFloorOpt);

    void updateCellAtUnchecked(hg::math::Vector2pz aPos,
                               const std::optional<CellModel::Floor>& aFloorOpt);

    // Wall

    void updateCellAt(hg::PZInteger aX,
                      hg::PZInteger aY,
                      const std::optional<CellModel::Wall>& aWallOpt);

    void updateCellAt(hg::math::Vector2pz aPos,
                      const std::optional<CellModel::Wall>& aWallOpt);

    void updateCellAtUnchecked(hg::PZInteger aX,
                               hg::PZInteger aY,
                               const std::optional<CellModel::Wall>& aWallOpt);

    void updateCellAtUnchecked(hg::math::Vector2pz aPos,
                               const std::optional<CellModel::Wall>& aWallOpt);

    ///////////////////////////////////////////////////////////////////////////
    // LIGHTS                                                                //
    ///////////////////////////////////////////////////////////////////////////

    LightId createLight(SpriteId aSpriteId, hg::math::Vector2pz aSize);

    void updateLight(LightId aLightHandle, hg::math::Vector2f aPosition, hg::math::AngleF aAngle);

    void destroyLight(LightId aLightHandle);

    // TODO(createRAIILight())

    // TODO(this shouldn't be exposed like this)
    std::unordered_map<LightId, detail::LightModelExt>::const_iterator lightDataBegin() const {
        return _lights.begin();
    }

    // TODO(this shouldn't be exposed like this)
    std::unordered_map<LightId, detail::LightModelExt>::const_iterator lightDataEnd() const {
        return _lights.end();
    }

private:
    // ===== Cells =====

    hg::util::RowMajorGrid<detail::CellModelExt> _grid;
    float _cellResolution;

    // ===== Lights =====

    std::unordered_map<LightId, detail::LightModelExt> _lights;
    LightId _lightIdCounter = 0;

    // ===== Other =====

    bool _generatorMode = false;

    void _refreshCellAtUnchecked(hg::PZInteger aX, hg::PZInteger aY);
};

} // namespace gridw
