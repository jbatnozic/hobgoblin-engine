
#include <GridWorld/World/World.hpp>

#include <Hobgoblin/HGExcept.hpp>

#include <algorithm>
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

///////////////////////////////////////////////////////////////////////////
// TEMPLATES                                                             //
///////////////////////////////////////////////////////////////////////////

template <bool taAllowedToLoadAdjacent>
World::RingAssessment World::_assessRing(hg::PZInteger aX, hg::PZInteger aY, hg::PZInteger aRing) {
    RingAssessment result;

    const auto isCellSolid = [this](hg::PZInteger aX, hg::PZInteger aY) -> bool {
        if (aX < 0 || aX >= _config.cellCountX || aY < 0 || aY >= _config.cellCountY) {
            return true;
        }

        if constexpr (taAllowedToLoadAdjacent) {
            auto& cell = _chunkStorage.getCellAtUnchecked(aX, aY, detail::LOAD_IF_MISSING);
            return cell.isWallInitialized();
        } else {
            auto* cell = _chunkStorage.getCellAtUnchecked(aX, aY);
            return !cell || cell->isWallInitialized();
        }
    };

    if (aRing == 0) {
        const bool solid = isCellSolid(aX, aY);
        if (solid) {
            result.occupiedCellCount = 1;
        }
        return result;
    }

    // Check top row
    {
        const int y = aY - aRing;

        // Check top-left corner
        {
            const auto x = aX - aRing;
            const bool solid = isCellSolid(x, y);
            if (solid) {
                result.occupiedCellCount += 1;
                result.e.bottom = result.e.right = false;
            }
        }
        // Check top-right corner
        {
            const auto x = aX + aRing;
            const bool solid = isCellSolid(x, y);
            if (solid) {
                result.occupiedCellCount += 1;
                result.e.bottom = result.e.left = false;
            }
        }
        // Check betwixt
        {
            for (int x = aX - aRing + 1; x <= aX + aRing - 1; x += 1) {
                const bool solid = isCellSolid(x, y);
                if (solid) {
                    result.occupiedCellCount += 1;
                    result.e.left = result.e.right = result.e.bottom = false;
                }
            }
        }
    }
    // Check bottom row
    {
        const int y = aY + aRing;

        // Check bottom-left corner
        {
            const auto x = aX - aRing;
            const bool solid = isCellSolid(x, y);
            if (solid) {
                result.occupiedCellCount += 1;
                result.e.top = result.e.right = false;
            }
        }
        // Check bottom-right corner
        {
            const auto x = aX + aRing;
            const bool solid = isCellSolid(x, y);
            if (solid) {
                result.occupiedCellCount += 1;
                result.e.top = result.e.left = false;
            }
        }
        // Check betwixt
        {
            for (int x = aX - aRing + 1; x <= aX + aRing - 1; x += 1) {
                const bool solid = isCellSolid(x, y);
                if (solid) {
                    result.occupiedCellCount += 1;
                    result.e.left = result.e.right = result.e.top = false;
                }
            }
        }
    }
    // Check middle rows (only extremes)
    {
        for (int y = aY - aRing + 1; y <= aY + aRing - 1; y += 1) {
            {
                const auto x = aX - aRing;
                const bool solid = isCellSolid(x, y);
                if (solid) {
                    result.occupiedCellCount += 1;
                    result.e.top = result.e.bottom = result.e.right = false;
                }
            }
            {
                const auto x = aX + aRing;
                const bool solid = isCellSolid(x, y);
                if (solid) {
                    result.occupiedCellCount += 1;
                    result.e.top = result.e.bottom = result.e.left = false;
                }
            }
        }
    }

    return result;
}

template <bool taAllowedToLoadAdjacent>
hg::PZInteger World::_calcOpennessAt(hg::PZInteger aX, hg::PZInteger aY) {
    // Ring 0 check if openness can be 1, ring 1 check if openness can be 2, etc.
    for (hg::PZInteger ring = 0; ring < _config.maxCellOpenness; ring += 1) {
        const auto ras = _assessRing<taAllowedToLoadAdjacent>(aX, aY, ring);

        if (ras.occupiedCellCount == 0) {
            continue;
        }

        if (ring == 0) {
            return 0; // Ring 0 - special case
        }

        if (ras.occupiedCellCount <= (ring * 2 + 1)) {
            if (ras.e.top || ras.e.left || ras.e.right || ras.e.bottom) {
                return ring * 2;
            }
        }

        return ring * 2 - 1;
    }

    return _config.maxCellOpenness * 2 - 1;
}

///////////////////////////////////////////////////////////////////////////
// asdasd                                                                //
///////////////////////////////////////////////////////////////////////////

void World::update() {
    _chunkStorage.update();
}

void World::prune() {
    _chunkStorage.prune();
}

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
// ACTIVE AREAS                                                          //
///////////////////////////////////////////////////////////////////////////

ActiveArea World::createActiveArea() {
    return _chunkStorage.createNewActiveArea();
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
void World::Editor::setWallAtUnchecked(hg::PZInteger                         aX,
                                          hg::PZInteger                         aY,
                                          const std::optional<CellModel::Wall>& aWallOpt) {
    _world._setWallAtUnchecked(aX, aY, aWallOpt);
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

// ===== Editing cells =====

void World::_startEdit() {
    _editMinX = -1;
    _editMinY = -1;
    _editMaxX = -1;
    _editMaxY = -1;
}

void World::_endEdit() {
    if (_editMinX == -1 || _editMinY == -1 || _editMaxX == -1 || _editMaxY == -1) {
        return;
    }

    const auto startX = std::max<hg::PZInteger>(0, _editMinX - _config.maxCellOpenness);
    const auto startY = std::max<hg::PZInteger>(0, _editMinY - _config.maxCellOpenness);
    const auto endX =
        std::min<hg::PZInteger>(_config.cellCountX - 1, _editMaxX + _config.maxCellOpenness - 1);
    const auto endY =
        std::min<hg::PZInteger>(_config.cellCountY - 1, _editMaxY + _config.maxCellOpenness - 1);

    for (hg::PZInteger y = startY; y <= endY; y += 1) {
        for (hg::PZInteger x = startX; x <= endX; x += 1) {
            _refreshCellAtUnchecked(x, y);
        }
    }
}

void World::_refreshCellAtUnchecked(hg::PZInteger aX, hg::PZInteger aY) {
    const auto openness = _calcOpennessAt<false>(aX, aY);

    // auto& cell = _grid[aY][aX];

    // GetMutableExtensionData(cell).refresh(
    //     (aY <= 0) ? nullptr : std::addressof(_grid[aY - 1][aX]),
    //     (aX <= 0) ? nullptr : std::addressof(_grid[aY][aX - 1]),
    //     (aX >= getCellCountX() - 1) ? nullptr : std::addressof(_grid[aY][aX + 1]),
    //     (aY >= getCellCountY() - 1) ? nullptr : std::addressof(_grid[aY + 1][aX]));

    auto* cell = _chunkStorage.getCellAtUnchecked(aX, aY);
    if (cell) {
        GetMutableExtensionData(*cell).openness = openness;
    }
}

void World::_setFloorAt(hg::PZInteger                          aX,
                        hg::PZInteger                          aY,
                        const std::optional<CellModel::Floor>& aFloorOpt) {
    HG_VALIDATE_ARGUMENT(aX < getCellCountX());
    HG_VALIDATE_ARGUMENT(aY < getCellCountY());

    _setFloorAtUnchecked(aX, aY, aFloorOpt);
}

void World::_setFloorAt(hg::math::Vector2pz aCell, const std::optional<CellModel::Floor>& aFloorOpt) {
    _setFloorAt(aCell.x, aCell.y, aFloorOpt);
}

void World::_setFloorAtUnchecked(hg::PZInteger                          aX,
                                 hg::PZInteger                          aY,
                                 const std::optional<CellModel::Floor>& aFloorOpt) {
    auto& cell = _chunkStorage.getCellAtUnchecked(aX, aY, detail::LOAD_IF_MISSING);
    if (aFloorOpt) {
        cell.setFloor(*aFloorOpt);
    } else {
        cell.resetFloor();
    }
}

void World::_setFloorAtUnchecked(hg::math::Vector2pz                    aCell,
                                 const std::optional<CellModel::Floor>& aFloorOpt) {
    _setFloorAtUnchecked(aCell.x, aCell.y, aFloorOpt);
}

void World::_setWallAt(hg::PZInteger                         aX,
                       hg::PZInteger                         aY,
                       const std::optional<CellModel::Wall>& aWallOpt) {
    HG_VALIDATE_ARGUMENT(aX < getCellCountX());
    HG_VALIDATE_ARGUMENT(aY < getCellCountY());

    _setWallAtUnchecked(aX, aY, aWallOpt);
}

void World::_setWallAt(hg::math::Vector2pz aCell, const std::optional<CellModel::Wall>& aWallOpt) {
    _setWallAt(aCell.x, aCell.y, aWallOpt);
}

void World::_setWallAtUnchecked(hg::PZInteger                         aX,
                                hg::PZInteger                         aY,
                                const std::optional<CellModel::Wall>& aWallOpt) {
    auto& cell = _chunkStorage.getCellAtUnchecked(aX, aY, detail::LOAD_IF_MISSING);

    if ((cell.isWallInitialized() == aWallOpt.has_value()) &&
        (!cell.isWallInitialized() || (cell.getWall().shape == aWallOpt->shape))) {
        // In this case, there is no refresh needed because the walls are of the same shape,
        // or are both non-existent
        goto SWAP_WALL;
    }

    if (_editMinX == -1 || _editMaxX == -1) {
        _editMinX = _editMaxX = aX;
    } else {
        _editMinX = std::min(_editMinX, aX);
        _editMaxX = std::max(_editMaxX, aX);
    }

    if (_editMinY == -1 || _editMaxY == -1) {
        _editMinY = _editMaxY = aY;
    } else {
        _editMinY = std::min(_editMinY, aY);
        _editMaxY = std::min(_editMaxY, aY);
    }

SWAP_WALL:
    if (aWallOpt) {
        cell.setWall(*aWallOpt);
    } else {
        cell.resetWall();
    }
}

void World::_setWallAtUnchecked(hg::math::Vector2pz                   aCell,
                                const std::optional<CellModel::Wall>& aWallOpt) {
    _setWallAtUnchecked(aCell.x, aCell.y, aWallOpt);
}

} // namespace gridworld
