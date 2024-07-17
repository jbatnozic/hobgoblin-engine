#pragma once

#include <GridWorld/Model/Chunk.hpp>
#include <GridWorld/Model/Chunk_id.hpp>

#include <optional>
#include <vector>

namespace gridworld {

namespace detail {

class ChunkSpoolerInterface { // ChunkLoader?
public:
    virtual ~ChunkSpoolerInterface() = default;

    virtual void pause() = 0;

    virtual void unpause() = 0;

    virtual bool isPaused() const = 0;

    struct LoadRequest {
        ChunkId       chunkId;
        hg::PZInteger priority; //!< Lower number = higher priority
    };

    virtual void setChunksToLoad(std::vector<LoadRequest> aLoadRequests) = 0;

    // loads a chunk immediately
    // only works if spooler is paused
    virtual std::optional<Chunk> loadImmediately(ChunkId aChunkId) = 0;

    // only works if spooler is paused
    virtual hg::PZInteger unloadChunk(ChunkId aChunkId, Chunk&& aChunk) = 0;

    virtual void unloadRuntimeCache() = 0;

    struct LoadedChunk {
        std::optional<Chunk> chunk;
        ChunkId              id;

        LoadedChunk(std::optional<Chunk> aChunk, ChunkId aId)
            : chunk{std::move(aChunk)}
            , id{aId} {}
    };

    // returns all chunks that have been loaded so far
    virtual std::vector<LoadedChunk> getLoaded() = 0;
};

} // namespace detail

} // namespace gridworld
