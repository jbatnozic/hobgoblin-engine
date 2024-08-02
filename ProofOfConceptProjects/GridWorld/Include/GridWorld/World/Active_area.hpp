#pragma once

#include <Hobgoblin/Common.hpp>

#include <GridWorld/World/Chunk_id.hpp>

#include <vector>

namespace gridworld {

namespace hg = jbatnozic::hobgoblin;

class World;

//! TODO(add description)
class ActiveArea {
public:
    //! TODO(add description)
    ~ActiveArea();

    //! TODO(add description)
    void setToNone();

    //! TODO(add description)
    void setToChunkRingSquare(ChunkId aCentralChunk, hg::PZInteger aRingCount);

    //! TODO(add description)
    void setToChunkList(std::vector<ChunkId> aChunkList);

private:
    friend class World;
    explicit ActiveArea(hg::NeverNull<World*> aWorld)
        : _world{aWorld} {}

    hg::NeverNull<World*> _world;
    std::vector<ChunkId>  _chunkList;
};

} // namespace gridworld
