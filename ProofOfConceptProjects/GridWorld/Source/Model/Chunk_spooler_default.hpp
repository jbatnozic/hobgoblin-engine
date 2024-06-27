#pragma once

#include "Chunk_disk_io_handler_interface.hpp"

#include <GridWorld/Model/Chunk_spooler_interface.hpp>

#include <Hobgoblin/Common.hpp>

#include <cstdint>
#include <condition_variable>
#include <mutex>
#include <thread>

namespace gridworld {
namespace detail {

namespace hg = ::jbatnozic::hobgoblin;

class DefaultChunkSpooler : public ChunkSpoolerInterface {
public:
    DefaultChunkSpooler();

    ~DefaultChunkSpooler() override;

    void pause() override;

    void unpause() override;

    bool isPaused() const override;

    void setChunksToLoad(std::vector<LoadRequest> aLoadRequests) override;

    // loads a chunk immediately
    // only works if spooler is paused
    std::optional<Chunk> loadImmediately(ChunkId aChunkId) override;

    // only works if spooler is paused
    void unloadChunk(ChunkId aChunkId, Chunk aChunk) override;

    void unloadRuntimeCache() override;

    // returns all chunks that have been loaded so far
    std::vector<Chunk> getLoaded() override;

private:
    using Mutex = std::mutex;
    mutable Mutex           _mutex;
    std::condition_variable _cv;

    bool _paused  = false;
    bool _stopped = false;

    enum class WorkerStatus : std::int8_t {
        PREP_OR_IDLE,
        WORKING
    };

    WorkerStatus            _workerStatus;
    std::condition_variable _cv_workerStatus;

    std::vector<LoadRequest> _loadRequests;

    std::thread _worker;

    void _workerBody();

    hg::PZInteger _countAvailableRequests(const std::unique_lock<Mutex>&) const;
    LoadRequest _takeRequestWithHighestPriority(const std::unique_lock<Mutex>&);
};

} // namespace detail
} // namespace gridworld
