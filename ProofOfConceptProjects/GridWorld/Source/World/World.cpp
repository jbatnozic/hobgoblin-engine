
#include <GridWorld/World/World.hpp>

#include <Hobgoblin/HGExcept.hpp>

#include <cmath>

#include "../Detail_access.hpp"

namespace gridworld {

float World::getCellResolution() const {
    return _configuration.cellResolution;
}

hg::PZInteger World::getCellCountX() const {
    return _configuration.cellCountX;
}

hg::PZInteger World::getCellCountY() const {
    return _configuration.cellCountY;
}

// void World::toggleGeneratorMode(bool aGeneratorModeActive) {
//     if (_generatorMode == aGeneratorModeActive) {
//         return;
//     }
//
//     if (aGeneratorModeActive) {
//         _generatorMode = true;
//         return;
//     }
//
//     _generatorMode = false;
//
//     for (hg::PZInteger y = 0; y < getCellCountY(); y += 1) {
//         for (hg::PZInteger x = 0; x < getCellCountX(); x += 1) {
//             _refreshCellAtUnchecked(x, y);
//         }
//     }
// }

///////////////////////////////////////////////////////////////////////////
// CELL GETTERS                                                          //
///////////////////////////////////////////////////////////////////////////

const CellModel* World::getCellAt(hg::PZInteger aX, hg::PZInteger aY) const {
    HG_VALIDATE_ARGUMENT(aX < getCellCountX());
    HG_VALIDATE_ARGUMENT(aY < getCellCountY());
    return _chunkStorage.getCellAtUnchecked(aX, aY);
}

const CellModel* World::getCellAt(hg::math::Vector2pz aPos) const {
    HG_VALIDATE_ARGUMENT(aPos.x < getCellCountX());
    HG_VALIDATE_ARGUMENT(aPos.y < getCellCountY());
    return _chunkStorage.getCellAtUnchecked(aPos.x, aPos.y);
}

const CellModel* World::getCellAtUnchecked(hg::PZInteger aX, hg::PZInteger aY) const {
    return _chunkStorage.getCellAtUnchecked(aX, aY);
}

const CellModel* World::getCellAtUnchecked(hg::math::Vector2pz aPos) const {
    return _chunkStorage.getCellAtUnchecked(aPos.x, aPos.y);
}

const CellModel& World::getCellAt(const EditPermission& /*aPerm*/,
                                  hg::PZInteger aX,
                                  hg::PZInteger aY) const {
    HG_VALIDATE_ARGUMENT(aX < getCellCountX());
    HG_VALIDATE_ARGUMENT(aY < getCellCountY());
    return _chunkStorage.getCellAtUnchecked(aX, aY, detail::LOAD_IF_MISSING);
}

const CellModel& World::getCellAt(const EditPermission& /*aPerm*/, hg::math::Vector2pz aPos) const {
    HG_VALIDATE_ARGUMENT(aPos.x < getCellCountX());
    HG_VALIDATE_ARGUMENT(aPos.y < getCellCountY());
    return _chunkStorage.getCellAtUnchecked(aPos.x, aPos.y, detail::LOAD_IF_MISSING);
}

const CellModel& World::getCellAtUnchecked(const EditPermission& /*aPerm*/,
                                           hg::PZInteger aX,
                                           hg::PZInteger aY) const {
    return _chunkStorage.getCellAtUnchecked(aX, aY, detail::LOAD_IF_MISSING);
}

const CellModel& World::getCellAtUnchecked(const EditPermission& /*aPerm*/,
                                           hg::math::Vector2pz aPos) const {
    return _chunkStorage.getCellAtUnchecked(aPos.x, aPos.y, detail::LOAD_IF_MISSING);
}

///////////////////////////////////////////////////////////////////////////
// LIGHTS                                                                //
///////////////////////////////////////////////////////////////////////////

int World::createLight(SpriteId aSpriteId, hg::math::Vector2pz aSize) {
    const int id = (_lightIdCounter + 1);
    _lightIdCounter += 1;

    const auto pair =
        _lights.emplace(std::piecewise_construct, std::forward_as_tuple(id), std::forward_as_tuple());
    auto& light    = pair.first->second;
    light.spriteId = aSpriteId;
    GetMutableExtensionData(light).texture.create(aSize);

    return id;
}

void World::updateLight(int aLightHandle, hg::math::Vector2f aPosition, hg::math::AngleF aAngle) {
    const auto iter = _lights.find(aLightHandle);
    HG_HARD_ASSERT(iter != _lights.end());

    auto& light    = iter->second;
    light.angle    = aAngle;
    light.position = aPosition;
}

void World::destroyLight(int aLightHandle) {
    const auto iter = _lights.find(aLightHandle);
    HG_HARD_ASSERT(iter != _lights.end());

    _lights.erase(iter);
}

#if 0
inline
void World::Editor::updateWallAtUnchecked(hg::PZInteger                         aX,
                                          hg::PZInteger                         aY,
                                          const std::optional<CellModel::Wall>& aWallOpt) {
    _world._updateWallAtUnchecked(aX, aY, aWallOpt);
    //    _grid[aY][aX].wall = aWallOpt;
    //
    //    if (_generatorMode) {
    //        return; // skip refreshing
    //    }
    //
    //    for (int yOffset = -1; yOffset <= 1; yOffset += 1) {
    //        if (aY + yOffset < 0 || aY + yOffset >= getCellCountY()) {
    //            continue; // out of grid
    //        }
    //        for (int xOffset = -1; xOffset <= 1; xOffset += 1) {
    //            if (aX + xOffset < 0 || aX + xOffset >= getCellCountX()) {
    //                continue; // out of grid
    //            }
    // #if 0 // TODO(temporary)
    //            if (yOffset == 0 && xOffset == 0) {
    //                continue; // this cell
    //            }
    // #endif
    //            _refreshCellAtUnchecked(aX + xOffset, aY + yOffset);
    //        }
    //    }
}
#endif

///////////////////////////////////////////////////////////////////////////
// PRIVATE METHODS                                                       //
///////////////////////////////////////////////////////////////////////////

void World::onChunkLoaded(detail::ChunkId aChunkId, ChunkExtensionInterface*) {
    // TODO: update openness and stuff of surrounding chunks
    // TODO: inform user listeners
}

void World::onChunkUnloaded(detail::ChunkId aChunkId, ChunkExtensionInterface*) {
    // TODO: inform user listeners
}

// ===== Updating cells =====

void World::_refreshCellAtUnchecked(hg::PZInteger aX, hg::PZInteger aY) {
    auto& cell = _grid[aY][aX];

    GetMutableExtensionData(cell).refresh(
        (aY <= 0) ? nullptr : std::addressof(_grid[aY - 1][aX]),
        (aX <= 0) ? nullptr : std::addressof(_grid[aY][aX - 1]),
        (aX >= getCellCountX() - 1) ? nullptr : std::addressof(_grid[aY][aX + 1]),
        (aY >= getCellCountY() - 1) ? nullptr : std::addressof(_grid[aY + 1][aX]));
}

void World::_updateFloorAt(hg::PZInteger                          aX,
                           hg::PZInteger                          aY,
                           const std::optional<CellModel::Floor>& aFloorOpt) {
    HG_VALIDATE_ARGUMENT(aX < getCellCountX());
    HG_VALIDATE_ARGUMENT(aY < getCellCountY());

    _updateFloorAtUnchecked(aX, aY, aFloorOpt);
}

void World::_updateFloorAt(hg::math::Vector2pz aPos, const std::optional<CellModel::Floor>& aFloorOpt) {
    _updateFloorAt(aPos.x, aPos.y, aFloorOpt);
}

void World::_updateFloorAtUnchecked(hg::PZInteger                          aX,
                                    hg::PZInteger                          aY,
                                    const std::optional<CellModel::Floor>& aFloorOpt) {
    // TODO: ACTUAL IMPL
}

void World::_updateFloorAtUnchecked(hg::math::Vector2pz                    aPos,
                                    const std::optional<CellModel::Floor>& aFloorOpt) {
    _updateFloorAtUnchecked(aPos.x, aPos.y, aFloorOpt);
}

void World::_updateWallAt(hg::PZInteger                         aX,
                          hg::PZInteger                         aY,
                          const std::optional<CellModel::Wall>& aWallOpt) {
    HG_VALIDATE_ARGUMENT(aX < getCellCountX());
    HG_VALIDATE_ARGUMENT(aY < getCellCountY());

    _updateWallAtUnchecked(aX, aY, aWallOpt);
}

void World::_updateWallAt(hg::math::Vector2pz aPos, const std::optional<CellModel::Wall>& aWallOpt) {
    _updateWallAt(aPos.x, aPos.y, aWallOpt);
}

void World::_updateWallAtUnchecked(hg::PZInteger                         aX,
                                   hg::PZInteger                         aY,
                                   const std::optional<CellModel::Wall>& aWallOpt) {
    // TODO: ACTUAL IMPL
    // TODO: update min/max x/y fields
    // TODO: load chunk if not already loaded (+ prune if needed)
    // TODO: store new wall
}

void World::_updateWallAtUnchecked(hg::math::Vector2pz                   aPos,
                                   const std::optional<CellModel::Wall>& aWallOpt) {
    _updateWallAtUnchecked(aPos.x, aPos.y, aWallOpt);
}

} // namespace gridworld
