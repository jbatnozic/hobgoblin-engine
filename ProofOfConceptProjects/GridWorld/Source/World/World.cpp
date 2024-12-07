
#include <GridWorld/World/World.hpp>

#include <Hobgoblin/HGExcept.hpp>

#include <cmath>

#include "../Detail_access.hpp"
#include "../Model/Chunk_disk_io_handler_interface.hpp"
#include "../Model/Chunk_spooler_default.hpp"

namespace gridworld {

// World::World(const WorldConfig& aConfig) {}

World::World(const WorldConfig&                                  aConfig,
             hg::NeverNull<detail::ChunkDiskIoHandlerInterface*> aChunkDiskIoHandler)
    : _config{aConfig}
    , _chunkDiskIoHandler{nullptr}
    , _chunkSpooler{std::make_unique<detail::DefaultChunkSpooler>(*aChunkDiskIoHandler)}
    , _chunkStorage{*_chunkSpooler, aConfig} {}

#ifdef FUTURE
World::World(const WorldConfig& aConfig, hg::NeverNull<detail::ChunkSpoolerInterface*> aChunkSpooler)
    : _config{aConfig}
    , _chunkDiskIoHandler{nullptr}
    , _chunkSpooler{nullptr}
    , _chunkStorage{*aChunkSpooler, aConfig} {}
#endif

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
// CONVERSIONS                                                           //
///////////////////////////////////////////////////////////////////////////

hg::math::Vector2pz World::posToCell(float aX, float aY) const {
    HG_VALIDATE_ARGUMENT(aX >= 0.f && aX < _config.cellCountX * _config.cellResolution);
    HG_VALIDATE_ARGUMENT(aY >= 0.f && aY < _config.cellCountY * _config.cellResolution);
    return posToCellUnchecked(aX, aY);
}

hg::math::Vector2pz World::posToCell(hg::math::Vector2f aPos) const {
    HG_VALIDATE_ARGUMENT(aPos.x >= 0.f && aPos.x < _config.cellCountX * _config.cellResolution);
    HG_VALIDATE_ARGUMENT(aPos.y >= 0.f && aPos.y < _config.cellCountY * _config.cellResolution);
    return posToCellUnchecked(aPos);
}

hg::math::Vector2pz World::posToCellUnchecked(float aX, float aY) const {
    return {static_cast<hg::PZInteger>(aX / _config.cellResolution),
            static_cast<hg::PZInteger>(aY / _config.cellResolution)};
}

hg::math::Vector2pz World::posToCellUnchecked(hg::math::Vector2f aPos) const {
    return {static_cast<hg::PZInteger>(aPos.x / _config.cellResolution),
            static_cast<hg::PZInteger>(aPos.y / _config.cellResolution)};
}

ChunkId World::cellToChunkId(hg::PZInteger aX, hg::PZInteger aY) const {
    HG_VALIDATE_ARGUMENT(aX < _config.cellCountX);
    HG_VALIDATE_ARGUMENT(aY < _config.cellCountY);
    return cellToChunkIdUnchecked(aX, aY);
}

ChunkId World::cellToChunkId(hg::math::Vector2pz aCell) const {
    HG_VALIDATE_ARGUMENT(aCell.x < _config.cellCountX);
    HG_VALIDATE_ARGUMENT(aCell.y < _config.cellCountY);
    return cellToChunkIdUnchecked(aCell);
}

ChunkId World::cellToChunkIdUnchecked(hg::PZInteger aX, hg::PZInteger aY) const {
    return {aX / _config.cellsPerChunkX, aY / _config.cellsPerChunkY};
}

ChunkId World::cellToChunkIdUnchecked(hg::math::Vector2pz aCell) const {
    return {aCell.x / _config.cellsPerChunkX, aCell.y / _config.cellsPerChunkY};
}

///////////////////////////////////////////////////////////////////////////
// LOCKING                                                               //
///////////////////////////////////////////////////////////////////////////

World::EditPermission::~EditPermission() {
    // signal world
}

std::unique_ptr<World::EditPermission> World::getPermissionToEdit() {
    return std::unique_ptr<EditPermission>{new EditPermission{}}; // TODO: temporary
}

///////////////////////////////////////////////////////////////////////////
// CELL GETTERS                                                          //
///////////////////////////////////////////////////////////////////////////

float World::getCellResolution() const {
    return _config.cellResolution;
}

hg::PZInteger World::getCellCountX() const {
    return _config.cellCountX;
}

hg::PZInteger World::getCellCountY() const {
    return _config.cellCountY;
}

const CellModel* World::getCellAt(hg::PZInteger aX, hg::PZInteger aY) const {
    HG_VALIDATE_ARGUMENT(aX < getCellCountX());
    HG_VALIDATE_ARGUMENT(aY < getCellCountY());
    return _chunkStorage.getCellAtUnchecked(aX, aY);
}

const CellModel* World::getCellAt(hg::math::Vector2pz aCell) const {
    HG_VALIDATE_ARGUMENT(aCell.x < getCellCountX());
    HG_VALIDATE_ARGUMENT(aCell.y < getCellCountY());
    return _chunkStorage.getCellAtUnchecked(aCell.x, aCell.y);
}

const CellModel* World::getCellAtUnchecked(hg::PZInteger aX, hg::PZInteger aY) const {
    return _chunkStorage.getCellAtUnchecked(aX, aY);
}

const CellModel* World::getCellAtUnchecked(hg::math::Vector2pz aCell) const {
    return _chunkStorage.getCellAtUnchecked(aCell.x, aCell.y);
}

const CellModel& World::getCellAt(const EditPermission& /*aPerm*/,
                                  hg::PZInteger aX,
                                  hg::PZInteger aY) const {
    HG_VALIDATE_ARGUMENT(aX < getCellCountX());
    HG_VALIDATE_ARGUMENT(aY < getCellCountY());
    return _chunkStorage.getCellAtUnchecked(aX, aY, detail::LOAD_IF_MISSING);
}

const CellModel& World::getCellAt(const EditPermission& /*aPerm*/, hg::math::Vector2pz aCell) const {
    HG_VALIDATE_ARGUMENT(aCell.x < getCellCountX());
    HG_VALIDATE_ARGUMENT(aCell.y < getCellCountY());
    return _chunkStorage.getCellAtUnchecked(aCell.x, aCell.y, detail::LOAD_IF_MISSING);
}

const CellModel& World::getCellAtUnchecked(const EditPermission& /*aPerm*/,
                                           hg::PZInteger aX,
                                           hg::PZInteger aY) const {
    return _chunkStorage.getCellAtUnchecked(aX, aY, detail::LOAD_IF_MISSING);
}

const CellModel& World::getCellAtUnchecked(const EditPermission& /*aPerm*/,
                                           hg::math::Vector2pz aCell) const {
    return _chunkStorage.getCellAtUnchecked(aCell.x, aCell.y, detail::LOAD_IF_MISSING);
}

///////////////////////////////////////////////////////////////////////////
// CHUNKS                                                                //
///////////////////////////////////////////////////////////////////////////

hg::PZInteger World::getChunkCountX() const {
    return _chunkStorage.getChunkCountX();
}

hg::PZInteger World::getChunkCountY() const {
    return _chunkStorage.getChunkCountY();
}

const Chunk* World::getChunkAt(hg::PZInteger aX, hg::PZInteger aY) const {
    HG_VALIDATE_ARGUMENT(aX < getCellCountX());
    HG_VALIDATE_ARGUMENT(aY < getCellCountY());
    return getChunkAtUnchecked(aX, aY);
}

const Chunk* World::getChunkAtUnchecked(hg::PZInteger aX, hg::PZInteger aY) const {
    const auto id = ChunkId{static_cast<std::uint16_t>(aX / _config.cellsPerChunkX),
                            static_cast<std::uint16_t>(aY / _config.cellsPerChunkY)};
    return getChunkAtIdUnchecked(id);
}

const Chunk* World::getChunkAt(hg::math::Vector2pz aCell) const {
    return getChunkAt(aCell.x, aCell.y);
}

const Chunk* World::getChunkAtUnchecked(hg::math::Vector2pz aCell) const {
    return getChunkAtUnchecked(aCell.x, aCell.y);
}

const Chunk* World::getChunkAtId(ChunkId aChunkId) const {
    HG_VALIDATE_ARGUMENT(static_cast<hg::PZInteger>(aChunkId.x) < getChunkCountX());
    HG_VALIDATE_ARGUMENT(static_cast<hg::PZInteger>(aChunkId.y) < getChunkCountY());
    return getChunkAtIdUnchecked(aChunkId);
}

const Chunk* World::getChunkAtIdUnchecked(ChunkId aChunkId) const {
    return _chunkStorage.getChunkAtIdUnchecked(aChunkId);
}

const Chunk& World::getChunkAt(const EditPermission& aPerm, hg::PZInteger aX, hg::PZInteger aY) const {
    HG_VALIDATE_ARGUMENT(aX < getCellCountX());
    HG_VALIDATE_ARGUMENT(aY < getCellCountY());
    return getChunkAtUnchecked(aPerm, aX, aY);
}

const Chunk& World::getChunkAtUnchecked(const EditPermission& aPerm,
                                        hg::PZInteger         aX,
                                        hg::PZInteger         aY) const {
    const auto id = ChunkId{static_cast<std::uint16_t>(aX / _config.cellsPerChunkX),
                            static_cast<std::uint16_t>(aY / _config.cellsPerChunkY)};
    return getChunkAtIdUnchecked(aPerm, id);
}

const Chunk& World::getChunkAt(const EditPermission& aPerm, hg::math::Vector2pz aCell) const {
    return getChunkAt(aPerm, aCell.x, aCell.y);
}

const Chunk& World::getChunkAtUnchecked(const EditPermission& aPerm, hg::math::Vector2pz aCell) const {
    return getChunkAtUnchecked(aPerm, aCell.x, aCell.y);
}

const Chunk& World::getChunkAtId(const EditPermission& aPerm, ChunkId aChunkId) const {
    HG_VALIDATE_ARGUMENT(static_cast<hg::PZInteger>(aChunkId.x) < getChunkCountX());
    HG_VALIDATE_ARGUMENT(static_cast<hg::PZInteger>(aChunkId.y) < getChunkCountY());
    return getChunkAtIdUnchecked(aPerm, aChunkId);
}

const Chunk& World::getChunkAtIdUnchecked(const EditPermission& /*aPerm*/, ChunkId aChunkId) const {
    return _chunkStorage.getChunkAtIdUnchecked(aChunkId, detail::LOAD_IF_MISSING);
}

///////////////////////////////////////////////////////////////////////////
// CHUNK EXTENSIONS                                                      //
///////////////////////////////////////////////////////////////////////////

// TODO

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

void World::onChunkLoaded(ChunkId aChunkId, ChunkExtensionInterface*) {
    // TODO: update openness and stuff of surrounding chunks
    // TODO: inform user listeners
}

void World::onChunkUnloaded(ChunkId aChunkId, ChunkExtensionInterface*) {
    // TODO: inform user listeners
}

// ===== Updating cells =====

void World::_refreshCellAtUnchecked(hg::PZInteger aX, hg::PZInteger aY) {
    // auto& cell = _grid[aY][aX];

    // GetMutableExtensionData(cell).refresh(
    //     (aY <= 0) ? nullptr : std::addressof(_grid[aY - 1][aX]),
    //     (aX <= 0) ? nullptr : std::addressof(_grid[aY][aX - 1]),
    //     (aX >= getCellCountX() - 1) ? nullptr : std::addressof(_grid[aY][aX + 1]),
    //     (aY >= getCellCountY() - 1) ? nullptr : std::addressof(_grid[aY + 1][aX]));
}

void World::_updateFloorAt(hg::PZInteger                          aX,
                           hg::PZInteger                          aY,
                           const std::optional<CellModel::Floor>& aFloorOpt) {
    HG_VALIDATE_ARGUMENT(aX < getCellCountX());
    HG_VALIDATE_ARGUMENT(aY < getCellCountY());

    _updateFloorAtUnchecked(aX, aY, aFloorOpt);
}

void World::_updateFloorAt(hg::math::Vector2pz aCell, const std::optional<CellModel::Floor>& aFloorOpt) {
    _updateFloorAt(aCell.x, aCell.y, aFloorOpt);
}

void World::_updateFloorAtUnchecked(hg::PZInteger                          aX,
                                    hg::PZInteger                          aY,
                                    const std::optional<CellModel::Floor>& aFloorOpt) {
    // TODO: ACTUAL IMPL
}

void World::_updateFloorAtUnchecked(hg::math::Vector2pz                    aCell,
                                    const std::optional<CellModel::Floor>& aFloorOpt) {
    _updateFloorAtUnchecked(aCell.x, aCell.y, aFloorOpt);
}

void World::_updateWallAt(hg::PZInteger                         aX,
                          hg::PZInteger                         aY,
                          const std::optional<CellModel::Wall>& aWallOpt) {
    HG_VALIDATE_ARGUMENT(aX < getCellCountX());
    HG_VALIDATE_ARGUMENT(aY < getCellCountY());

    _updateWallAtUnchecked(aX, aY, aWallOpt);
}

void World::_updateWallAt(hg::math::Vector2pz aCell, const std::optional<CellModel::Wall>& aWallOpt) {
    _updateWallAt(aCell.x, aCell.y, aWallOpt);
}

void World::_updateWallAtUnchecked(hg::PZInteger                         aX,
                                   hg::PZInteger                         aY,
                                   const std::optional<CellModel::Wall>& aWallOpt) {
    // TODO: ACTUAL IMPL
    // TODO: update min/max x/y fields
    // TODO: load chunk if not already loaded (+ prune if needed)
    // TODO: store new wall
}

void World::_updateWallAtUnchecked(hg::math::Vector2pz                   aCell,
                                   const std::optional<CellModel::Wall>& aWallOpt) {
    _updateWallAtUnchecked(aCell.x, aCell.y, aWallOpt);
}

} // namespace gridworld
