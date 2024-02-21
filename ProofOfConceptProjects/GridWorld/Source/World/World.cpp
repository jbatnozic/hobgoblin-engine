
#include <GridWorld/World/World.hpp>

#include <Hobgoblin/HGExcept.hpp>

#include <cmath>

#include "../Detail_access.hpp"

namespace gridworld {

float World::getCellResolution() const {
    return _cellResolution;
}

hg::PZInteger World::getCellCountX() const {
    return _grid.getWidth();
}

hg::PZInteger World::getCellCountY() const {
    return _grid.getHeight();
}

///////////////////////////////////////////////////////////////////////////
// CELL GETTERS                                                          //
///////////////////////////////////////////////////////////////////////////

//#define TEMPORARY
#ifdef TEMPORARY
CellModel& World::getCellAt(hg::PZInteger aX, hg::PZInteger aY) {
    return _grid.at(aY, aX);
}

CellModel& World::getCellAt(hg::math::Vector2pz aPos) {
    return _grid.at(aPos.y, aPos.x);
}

CellModel& World::getCellAtUnchecked(hg::PZInteger aX, hg::PZInteger aY) {
    return _grid[aY][aX];
}

CellModel& World::getCellAtUnchecked(hg::math::Vector2pz aPos) {
    return _grid[aPos.y][aPos.x];
}
#endif

const CellModel& World::getCellAt(hg::PZInteger aX, hg::PZInteger aY) const {
    return _grid.at(aY, aX);
}

const CellModel& World::getCellAt(hg::math::Vector2pz aPos) const {
    return _grid.at(aPos.y, aPos.x);
}

const CellModel& World::getCellAtUnchecked(hg::PZInteger aX, hg::PZInteger aY) const {
    return _grid[aY][aX];
}

const CellModel& World::getCellAtUnchecked(hg::math::Vector2pz aPos) const {
    return _grid[aPos.y][aPos.x];
}

///////////////////////////////////////////////////////////////////////////
// CELL UPDATERS                                                         //
///////////////////////////////////////////////////////////////////////////

// Floor

void World::updateCellAt(hg::PZInteger aX,
                         hg::PZInteger aY,
                         const std::optional<CellModel::Floor>& aFloorOpt) {
    HG_VALIDATE_ARGUMENT(aX < getCellCountX());
    HG_VALIDATE_ARGUMENT(aY < getCellCountY());

    updateCellAtUnchecked(aX, aY, aFloorOpt);
}

void World::updateCellAt(hg::math::Vector2pz aPos,
                         const std::optional<CellModel::Floor>& aFloorOpt) {
    updateCellAt(aPos.x, aPos.y, aFloorOpt);
}

void World::updateCellAtUnchecked(hg::PZInteger aX,
                                  hg::PZInteger aY,
                                  const std::optional<CellModel::Floor>& aFloorOpt) {
    _grid[aY][aX].floor = aFloorOpt;
}

void World::updateCellAtUnchecked(hg::math::Vector2pz aPos,
                                  const std::optional<CellModel::Floor>& aFloorOpt) {
    updateCellAtUnchecked(aPos.x, aPos.y, aFloorOpt);
}

// Wall

void World::updateCellAt(hg::PZInteger aX,
                         hg::PZInteger aY,
                         const std::optional<CellModel::Wall>& aWallOpt) {
    HG_VALIDATE_ARGUMENT(aX < getCellCountX());
    HG_VALIDATE_ARGUMENT(aY < getCellCountY());
    
    updateCellAtUnchecked(aX, aY, aWallOpt);
}

void World::updateCellAt(hg::math::Vector2pz aPos,
                         const std::optional<CellModel::Wall>& aWallOpt) {
    updateCellAt(aPos.x, aPos.y, aWallOpt);
}

void World::updateCellAtUnchecked(hg::PZInteger aX,
                                  hg::PZInteger aY,
                                  const std::optional<CellModel::Wall>& aWallOpt) {
    _grid[aY][aX].wall = aWallOpt;

    for (int yOffset = -1; yOffset <= 1; yOffset += 1) {
        if (aY + yOffset < 0 || aY + yOffset >= getCellCountY()) {
            continue; // out of grid
        }
        for (int xOffset = -1; xOffset <= 1; xOffset += 1) {
            if (aX + xOffset < 0 || aX + xOffset >= getCellCountX()) {
                continue; // out of grid
            }
#if 0 // TODO(temporary)
            if (yOffset == 0 && xOffset == 0) {
                continue; // this cell
            }
#endif
            _refreshCellAtUnchecked(aX + xOffset, aY + yOffset);
        }
    }
}

void World::updateCellAtUnchecked(hg::math::Vector2pz aPos,
                                  const std::optional<CellModel::Wall>& aWallOpt) {
    updateCellAtUnchecked(aPos.x, aPos.y, aWallOpt);
}

///////////////////////////////////////////////////////////////////////////
// LIGHTS                                                                //
///////////////////////////////////////////////////////////////////////////

int World::createLight(SpriteId aSpriteId, hg::math::Vector2pz aSize) {
    const int id = (_lightIdCounter + 1);
    _lightIdCounter += 1;

    const auto pair = _lights.emplace(std::piecewise_construct,
                                      std::forward_as_tuple(id),
                                      std::forward_as_tuple());
    auto& light = pair.first->second;
    light.spriteId = aSpriteId;
    GetMutableExtensionData(light).texture.create(aSize);

    return id;
}

void World::updateLight(int aLightHandle, hg::math::Vector2f aPosition, hg::math::AngleF aAngle) {
    const auto iter = _lights.find(aLightHandle);
    HG_HARD_ASSERT(iter != _lights.end());

    auto& light = iter->second;
    light.angle = aAngle;
    light.position = aPosition;
}

void World::destroyLight(int aLightHandle) {
    const auto iter = _lights.find(aLightHandle);
    HG_HARD_ASSERT(iter != _lights.end());

    _lights.erase(iter);
}

void World::_refreshCellAtUnchecked(hg::PZInteger aX, hg::PZInteger aY) {
    auto& cell = _grid[aY][aX];

    GetMutableExtensionData(cell).refresh(
        (aY <= 0) ? nullptr : std::addressof(_grid[aY - 1][aX]),
        (aX <= 0) ? nullptr : std::addressof(_grid[aY][aX - 1]),
        (aX >= getCellCountX() - 1) ? nullptr : std::addressof(_grid[aY][aX + 1]),
        (aY >= getCellCountY() - 1) ? nullptr : std::addressof(_grid[aY + 1][aX])
    );
}

} // namespace gridworld
