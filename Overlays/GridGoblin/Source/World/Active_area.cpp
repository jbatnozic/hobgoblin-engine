// Copyright 2024 Jovan Batnozic. Released under MS-PL licence in Serbia.
// See https://github.com/jbatnozic/Hobgoblin?tab=readme-ov-file#licence

#include <GridGoblin/Private/Chunk_storage_handler.hpp>
#include <GridGoblin/World/Active_area.hpp>
#include <GridGoblin/World/World.hpp>

#include <algorithm>
#include <cmath>

namespace jbatnozic {
namespace gridgoblin {

#define PRIORITY_NOT_IMPORTANT 999999

namespace {
std::vector<detail::ChunkUsageChange> SetChunkListToEmpty(std::vector<ChunkId>& aCurrentChunkList) {
    if (aCurrentChunkList.empty()) {
        return {};
    }

    std::vector<detail::ChunkUsageChange> changes;
    changes.reserve(aCurrentChunkList.size());

    for (const auto id : aCurrentChunkList) {
        changes.push_back({id, -1, PRIORITY_NOT_IMPORTANT});
    }

    aCurrentChunkList.clear();

    return changes;
}
} // namespace

namespace detail {
std::vector<ChunkUsageChange> UpdateChunkList(
    std::vector<ChunkId>&                        aCurrentChunkList,
    std::vector<ChunkId>                         aNewChunkList,
    const std::function<hg::PZInteger(ChunkId)>& aGetLoadPriority)
//
{
    if (aNewChunkList.empty()) {
        return SetChunkListToEmpty(aCurrentChunkList);
    }

    std::sort(aCurrentChunkList.begin(), aCurrentChunkList.end());

    std::vector<detail::ChunkUsageChange> changes;
    changes.reserve(aCurrentChunkList.size());
    for (const auto id : aCurrentChunkList) {
        changes.push_back({id, -1, PRIORITY_NOT_IMPORTANT});
    }

    hg::PZInteger loadPriority = 0;
    for (const auto id : aNewChunkList) {
        const auto iter = std::lower_bound(aCurrentChunkList.begin(), aCurrentChunkList.end(), id);
        if (iter == aCurrentChunkList.end() || (*iter) != id) {
            // If the ID wasn't found in aCurrentChunkList, that means it also isn't present in changes
            // Note: if the same ID is found in aNewChunkList multiple times (which shouldn't really
            //       happen but is possible), this will be handled properly by the storage handler
            //       (load requests past the first won't do anything), but we must later decrement
            //       usage an equal amount of times.
            changes.push_back({id, +1, aGetLoadPriority ? aGetLoadPriority(id) : loadPriority});
            loadPriority += 1;
        } else {
            const auto index          = static_cast<std::size_t>(iter - aCurrentChunkList.begin());
            changes[index].usageDelta = 0;
        }
    }

    changes.erase(std::remove_if(changes.begin(),
                                 changes.end(),
                                 [](const auto& aItem) {
                                     return aItem.usageDelta == 0;
                                 }),
                  changes.end());

    aCurrentChunkList = std::move(aNewChunkList);

    return changes;
}
} // namespace detail

ActiveArea::~ActiveArea() {
    setToNone();
}

void ActiveArea::setToNone() {
    auto changes = SetChunkListToEmpty(_chunkList);
    _storageHandler->_updateChunkUsage(changes);
}

void ActiveArea::setToChunkRingSquare(ChunkId                                      aCentralChunk,
                                      hg::PZInteger                                aRingCount,
                                      const std::function<hg::PZInteger(ChunkId)>& aGetLoadPriority) {
    const auto worldChunkCountX = _storageHandler->_chunks.getWidth();
    const auto worldChunkCountY = _storageHandler->_chunks.getHeight();

    const auto squareSize = aRingCount * 2 + 1;
    const auto startX     = static_cast<int>(aCentralChunk.x) - aRingCount;
    const auto startY     = static_cast<int>(aCentralChunk.y) - aRingCount;

    std::vector<ChunkId> newChunkList;
    newChunkList.reserve(static_cast<std::size_t>(squareSize * squareSize));

    for (int y = startY; y < startY + squareSize; y += 1) {
        if (y < 0) {
            continue;
        }
        if (y >= worldChunkCountY) {
            break;
        }
        for (int x = startX; x < startX + squareSize; x += 1) {
            if (x < 0) {
                continue;
            }
            if (x >= worldChunkCountX) {
                break;
            }
            newChunkList.push_back(ChunkId{(std::uint16_t)x, (std::uint16_t)y});
        }
    }

    auto changes = detail::UpdateChunkList(
        _chunkList,
        newChunkList,
        aGetLoadPriority ? aGetLoadPriority : [aCentralChunk](ChunkId aId) -> hg::PZInteger {
            return std::abs((int)aId.x - (int)aCentralChunk.x) +
                   std::abs((int)aId.y - (int)aCentralChunk.y);
        });
    _storageHandler->_updateChunkUsage(changes);
}

void ActiveArea::setToChunkRingDiamond(ChunkId                                      aCentralChunk,
                                       hg::PZInteger                                aRingCount,
                                       const std::function<hg::PZInteger(ChunkId)>& aGetLoadPriority) {
    const auto worldChunkCountX = _storageHandler->_chunks.getWidth();
    const auto worldChunkCountY = _storageHandler->_chunks.getHeight();

    const auto squareSize = aRingCount * 2 + 1;
    const auto startX     = static_cast<int>(aCentralChunk.x) - aRingCount;
    const auto startY     = static_cast<int>(aCentralChunk.y) - aRingCount;

    std::vector<ChunkId> newChunkList;
    newChunkList.reserve(static_cast<std::size_t>(squareSize * squareSize / 2 + 1));

    for (int y = startY; y < startY + squareSize; y += 1) {
        if (y < 0) {
            continue;
        }
        if (y >= worldChunkCountY) {
            break;
        }
        for (int x = startX; x < startX + squareSize; x += 1) {
            if (x < 0) {
                continue;
            }
            if (x >= worldChunkCountX) {
                break;
            }
            if (std::abs(x - (int)aCentralChunk.x) + std::abs(y - (int)aCentralChunk.y) <= aRingCount) {
                newChunkList.push_back(ChunkId{(std::uint16_t)x, (std::uint16_t)y});
            }
        }
    }

    auto changes = detail::UpdateChunkList(
        _chunkList,
        newChunkList,
        aGetLoadPriority ? aGetLoadPriority : [aCentralChunk](ChunkId aId) -> hg::PZInteger {
            return std::abs((int)aId.x - (int)aCentralChunk.x) +
                   std::abs((int)aId.y - (int)aCentralChunk.y);
        });
    _storageHandler->_updateChunkUsage(changes);
}

void ActiveArea::setToChunkList(std::vector<ChunkId>                         aChunkList,
                                const std::function<hg::PZInteger(ChunkId)>& aGetLoadPriority) {
    auto changes = detail::UpdateChunkList(_chunkList, aChunkList, aGetLoadPriority);
    _storageHandler->_updateChunkUsage(changes);
}

const std::vector<ChunkId>& ActiveArea::getChunkList() const {
    return _chunkList;
}

} // namespace gridgoblin
} // namespace jbatnozic
