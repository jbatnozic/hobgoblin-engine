#pragma once

#include "Chunk_disk_io_handler_interface.hpp"

#include <GridWorld/Model/Chunk_spooler_interface.hpp>

#include <Hobgoblin/Common.hpp>

#include <condition_variable>
#include <cstdint>
#include <mutex>
#include <thread>
#include <unordered_map>
#include <variant>

namespace gridworld {
namespace detail {

namespace hg = ::jbatnozic::hobgoblin;

class DefaultChunkSpooler : public ChunkSpoolerInterface {
public:
    DefaultChunkSpooler(ChunkDiskIoHandlerInterface& aDiskIoHandler);
    ~DefaultChunkSpooler() override;

    void pause() override;
    void unpause() override;
    bool isPaused() const override;

    void setChunksToLoad(std::vector<LoadRequest> aLoadRequests) override;

    std::optional<Chunk> loadImmediately(ChunkId aChunkId) override;

    hg::PZInteger unloadChunk(ChunkId aChunkId, Chunk aChunk) override;

    void unloadRuntimeCache() override;

    std::vector<LoadedChunk> getLoaded() override;

private:
    hg::NeverNull<ChunkDiskIoHandlerInterface*> _diskIoHandler;

    using Mutex = std::mutex;
    mutable Mutex           _mutex;

    bool _paused  = false;
    bool _stopped = false;

    enum class WorkerStatus : std::int8_t {
        PREP_OR_IDLE,
        WORKING
    };

    WorkerStatus            _workerStatus;
    std::condition_variable _cv_workerStatus;
    std::condition_variable _cv_workerSync;

    std::vector<LoadRequest> _loadRequests;
    std::vector<LoadedChunk> _loadedChunks;

    struct UnloadRequest {
        Chunk   chunk;
        ChunkId id;
    };

    static constexpr int UNLOAD_REQUEST_DEFAULT_PRIORITY = 10;
    static constexpr int UNLOAD_REQUEST_THRESHOLD        = 32;
    static constexpr int LOADS_PER_UNLOAD                = 2;

    using RequestVariant = std::variant<std::monostate, LoadRequest, UnloadRequest>;

    std::unordered_map<ChunkId, Chunk> _unloadRequests;
    int                                _unloadPriority = 0;

    std::thread _worker;

    void _workerBody();
    void _setWorkerStatus(WorkerStatus aWorkerStatus);

    hg::PZInteger  _countAvailableRequests(const std::unique_lock<Mutex>&) const;
    LoadRequest    _takeLoadRequestWithHighestPriority(const std::unique_lock<Mutex>&);
    UnloadRequest  _takeUnloadRequestWithHighestPriority(const std::unique_lock<Mutex>&);
    RequestVariant _takeRequestWithHighestPriority(const std::unique_lock<Mutex>&);

    void _adjustUnloadPriority(const RequestVariant& aRequestVariant, const std::unique_lock<Mutex>&);
};

} // namespace detail
} // namespace gridworld
