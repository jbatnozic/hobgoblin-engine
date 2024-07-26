#pragma once

#include <GridWorld/Model/Chunk.hpp>
#include <GridWorld/Model/Chunk_id.hpp>

#include <optional>
#include <vector>

namespace gridworld {

namespace detail {

class ChunkSpoolerInterface { // TODO: ChunkLoader?
public:
    //! Virtual desctructor.
    virtual ~ChunkSpoolerInterface() = default;

    virtual void pause() = 0;

    virtual void unpause() = 0;

    //! Checks whether the spooler is currently paused (returns true if yes).
    virtual bool isPaused() const = 0;

    struct LoadRequest {
        ChunkId       chunkId;
        hg::PZInteger priority; //!< Lower number = higher priority
    };

    class RequestHandleInterface {
    public:
        virtual ~RequestHandleInterface() = default;

        virtual void cancel() = 0;

        virtual ChunkId getChunkId() const = 0;

        virtual bool isFinished() const = 0;

        virtual std::optional<Chunk> takeChunk() = 0;
    };

    virtual std::vector<std::shared_ptr<RequestHandleInterface>> loadChunks(
        const std::vector<LoadRequest>& aLoadRequests) = 0;

    //! Loads a chunk immediately.
    //!
    //! The spooler must be paused when this is called.
    //! \throws hg::PreconditionNotMetError if the spooler is not paused when this method is called.
    virtual std::optional<Chunk> loadImmediately(ChunkId aChunkId) = 0;

    //! TODO(description)
    //!
    //! The spooler must be paused when this is called.
    //! \throws hg::PreconditionNotMetError if the spooler is not paused when this method is called.
    virtual hg::PZInteger unloadChunk(ChunkId aChunkId, Chunk&& aChunk) = 0;

    //! TODO(description)
    //!
    //! The spooler must be paused when this is called.
    //! \throws hg::PreconditionNotMetError if the spooler is not paused when this method is called.
    virtual void unloadRuntimeCache() = 0;
};

} // namespace detail

} // namespace gridworld
