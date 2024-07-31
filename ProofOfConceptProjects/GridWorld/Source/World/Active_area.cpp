#include <GridWorld/World/Active_area.hpp>
#include <GridWorld/World/world.hpp>

namespace gridworld {

ActiveArea::~ActiveArea() {
    this->setToNone();
}

void ActiveArea::setToNone() {
    _world->_decrementUsageOfChunks(_chunkList);
    _chunkList.clear();
}

void ActiveArea::setToChunkRingSquare(ChunkId aCentralChunk, hg::PZInteger aRingCount) {
    _world->_decrementUsageOfChunks(_chunkList);

    _chunkList.clear();
    const auto squareSize = aRingCount * 2 + 1;
    const auto startX = static_cast<int>(aCentralChunk.x) - aRingCount;
    const auto startY = static_cast<int>(aCentralChunk.y) - aRingCount;
    for (int y = 0; y < startY + squareSize; y += 1) {
        if (y < 0) {
            continue;
        }
        if (y >= _world->getCellCountY()) {
            break;
        }
        for (int x = 0; x < startX + squareSize; x += 1) {
            if (x < 0) {
                continue;
            }
            if (x >= _world->getCellCountX()) {
                break;
            }
            _chunkList.push_back(ChunkId{(std::uint16_t)x, (std::uint16_t)y});
        }
    }

    _world->_incrementUsageOfChunks(_chunkList);
}

void ActiveArea::setToChunkList(std::vector<ChunkId> aChunkList) {
    _world->_decrementUsageOfChunks(_chunkList);
    _chunkList = std::move(aChunkList);
    _world->_incrementUsageOfChunks(_chunkList);
}

} // namespace gridworld
