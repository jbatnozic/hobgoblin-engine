#pragma once

#include <Hobgoblin/Common.hpp>

#include <GridWorld/Model/Chunk.hpp>
#include <GridWorld/Model/Chunk_id.hpp>

#include <Hobgoblin/Utility/Grids.hpp>

#include <memory>

namespace gridworld {

namespace hg = jbatnozic::hobgoblin;

namespace detail {

class ChunkHolder {
public:

  CellModelExt& getCellAtUnchecked(hg::PZInteger aX, hg::PZInteger aY) {
    const auto chunkX = aX / _chunkWidth;
    const auto chunkY = aY / _chunkHeight;

    if (auto& chunk = _chunks[chunkY][chunkX]; chunk != nullptr) {
      [[likely]];
      return chunk->getCellAtUnchecked(aX % _chunkWidth, aY % _chunkHeight);
    } else {
      [[unlikely]];
      // TODO: load it
      return chunk->getCellAtUnchecked(aX % _chunkWidth, aY % _chunkHeight);
    }
  }

  const CellModelExt& getCellAtUnchecked(hg::PZInteger aX, hg::PZInteger aY) const {
    
  }

private:
  hg::util::RowMajorGrid<std::unique_ptr<Chunk>> _chunks;

  hg::PZInteger _chunkWidth;
  hg::PZInteger _chunkHeight;
};

} // namespace

} // namespace gridworld
