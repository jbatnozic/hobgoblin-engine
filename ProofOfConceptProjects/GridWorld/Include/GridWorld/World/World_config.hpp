#pragma once

#include <GridWorld/Model/Chunk_extension.hpp>

#include <Hobgoblin/Common.hpp>

#include <functional>
#include <memory>

namespace gridworld {

namespace hg = ::jbatnozic::hobgoblin;

struct WorldConfig {
    //! Total count of chunks in the horizontal (X) direction.
    //! Must be between 1 and 4096.
    hg::PZInteger chunkCountX;
    //! Total count of chunks in the vertical (Y) direction.
    //! Must be between 1 and 4096.
    hg::PZInteger chunkCountY;

    //! Total count of cells in a single chunk in the horizontal (X) direction.
    //! Must be between 1 and 1024.
    hg::PZInteger cellsPerChunkX;
    //! Total count of cells in a single chunk in the vertical (Y) direction.
    //! Must be between 1 and 1024.
    hg::PZInteger cellsPerChunkY;

    //! The width and height of a single cell, in pixels. Must be positive.
    float cellResolution;

    //! TODO(description)
    //! \warning MUST BE 0 OR ODD NUMBER
    hg::PZInteger maxCellOpenness;

    //! TODO(description)
    hg::PZInteger maxLoadedNonessentialCells;

    using ChunkExtensionFactory = std::function<std::unique_ptr<ChunkExtensionInterface>()>;
    ChunkExtensionFactory chunkExtensionFactory;
};

// TODO: config validation

} // namespace gridworld
