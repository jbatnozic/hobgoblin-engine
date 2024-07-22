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

    //! 
    //! \throws hg::PreconditionNotMetError if the spooler is not paused when this method is called.
    virtual void setChunksToLoad(std::vector<LoadRequest> aLoadRequests) = 0;

    //! Loads a chunk immediately.
    //! 
    //! The spooler must be paused when this is called.
    //! 
    //! \throws hg::PreconditionNotMetError if the spooler is not paused when this method is called.
    virtual std::optional<Chunk> loadImmediately(ChunkId aChunkId) = 0;

    //! \throws hg::PreconditionNotMetError if the spooler is not paused when this method is called.
    virtual hg::PZInteger unloadChunk(ChunkId aChunkId, Chunk&& aChunk) = 0;

    //! \throws hg::PreconditionNotMetError if the spooler is not paused when this method is called.
    virtual void unloadRuntimeCache() = 0;

    struct LoadedChunk {
        std::optional<Chunk> chunk;
        ChunkId              id;

        LoadedChunk(std::optional<Chunk> aChunk, ChunkId aId)
            : chunk{std::move(aChunk)}
            , id{aId} {}
    };

    //! Returns all chunks that have been loaded so far (since the last call to this method).
    virtual std::vector<LoadedChunk> getLoaded() = 0;
};

} // namespace detail

} // namespace gridworld
