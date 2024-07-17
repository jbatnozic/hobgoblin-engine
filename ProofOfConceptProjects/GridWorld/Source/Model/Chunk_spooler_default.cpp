#pragma once

#include "Chunk_spooler_default.hpp"

#include <Hobgoblin/HGExcept.hpp>
#include <Hobgoblin/Logging.hpp>

#include <algorithm>

// TODO: SetThreadName

namespace gridworld {
namespace detail {

namespace {
constexpr auto LOG_ID = "gridworld";

std::optional<Chunk> LoadChunk(ChunkId aChunkId, ChunkDiskIoHandlerInterface& aDiskIoHandler) {
    auto rtChunk = aDiskIoHandler.loadChunkFromRuntimeCache(aChunkId);
    if (rtChunk.has_value()) {
        return rtChunk;
    }
    return aDiskIoHandler.loadChunkFromPersistentCache(aChunkId); // TODO: store in runtime cache - noooooooo
}

void UnloadChunk(const Chunk& aChunk, ChunkId aChunkId, ChunkDiskIoHandlerInterface& aDiskIoHandler) {
    aDiskIoHandler.storeChunkInRuntimeCache(aChunk, aChunkId);
}

} // namespace

DefaultChunkSpooler::DefaultChunkSpooler(ChunkDiskIoHandlerInterface& aDiskIoHandler)
    : _diskIoHandler{&aDiskIoHandler}
    , _worker{&DefaultChunkSpooler::_workerBody, this} {
    // SetThreadName(_worker, "chunkspool");
    HG_LOG_INFO(LOG_ID, "Spooler started.");
}

DefaultChunkSpooler::~DefaultChunkSpooler() {
    HG_LOG_INFO(LOG_ID, "Stopping Spooler...");

    {
        std::unique_lock<Mutex> lock{_mutex};
        _stopped = true;
    }
    _cv_workerSync.notify_one();
    _worker.join();

    HG_LOG_INFO(LOG_ID, "Spooler stopped.");
}

void DefaultChunkSpooler::pause() {
    std::unique_lock<Mutex> lock{_mutex};

    _paused = true;

    _cv_workerStatus.wait(lock, [this]() -> bool {
        // Return false to wait more, return true to continue
        return (_workerStatus == WorkerStatus::PREP_OR_IDLE);
    });
}

void DefaultChunkSpooler::unpause() {
    std::unique_lock<Mutex> lock{_mutex};

    _paused = false;
    lock.unlock();
    _cv_workerSync.notify_all();
}

bool DefaultChunkSpooler::isPaused() const {
    std::unique_lock<Mutex> lock{_mutex};

    const auto result = _paused;
    lock.unlock();
    return result;
}

void DefaultChunkSpooler::setChunksToLoad(std::vector<LoadRequest> aLoadRequests) {
    std::sort(aLoadRequests.begin(),
              aLoadRequests.end(),
              [](const LoadRequest& aLhs, const LoadRequest& aRhs) {
                  // We want to sort it so that the highest priorities (lowest numbers)
                  // end up at the end of the vector -- sorting in descending order.
                  // note: Returning true means aLhs comes before aRhs
                  return (aLhs.priority > aRhs.priority);
              });

    std::unique_lock<Mutex> lock{_mutex};
    _loadRequests = std::move(aLoadRequests);
    lock.unlock();
    _cv_workerSync.notify_one();
}

std::optional<Chunk> DefaultChunkSpooler::loadImmediately(ChunkId aChunkId) {
    std::unique_lock<Mutex> lock{_mutex};

    HG_VALIDATE_PRECONDITION(_paused && "Spooler must be paused when this method is called");

    std::erase_if(_loadRequests, [aChunkId](const LoadRequest& aLoadRequest) {
        return aLoadRequest.chunkId == aChunkId;
    });

    return LoadChunk(aChunkId, *_diskIoHandler);
}

hg::PZInteger DefaultChunkSpooler::unloadChunk(ChunkId aChunkId, Chunk&& aChunk) {
    std::unique_lock<Mutex> lock{_mutex};

    HG_VALIDATE_PRECONDITION(_paused && "Spooler must be paused when this method is called");

    const auto pair = _unloadRequests.emplace(std::make_pair(aChunkId, std::move(aChunk)));
    HG_HARD_ASSERT(pair.second && "Duplicate unload request");

    const auto result = hg::stopz(_unloadRequests.size());

    lock.unlock();
    _cv_workerSync.notify_one();

    return result;
}

void DefaultChunkSpooler::unloadRuntimeCache() {
    HG_NOT_IMPLEMENTED(); // TODO
}

std::vector<DefaultChunkSpooler::LoadedChunk> DefaultChunkSpooler::getLoaded() {
    std::unique_lock<Mutex> lock{_mutex};

    auto result = std::move(_loadedChunks);
    lock.unlock();
    return result;
}

///////////////////////////////////////////////////////////////////////////
// PRIVATE METHODS                                                       //
///////////////////////////////////////////////////////////////////////////

#define HOLDS_LOAD_REQUEST(_variant_) std::holds_alternative<LoadRequest>(_variant_)

void DefaultChunkSpooler::_workerBody() {
    while (true) {
        RequestVariant requestVariant;

        /* SYNCHRONIZATION */
        {
            std::unique_lock<Mutex> lock{_mutex};
            _setWorkerStatus(WorkerStatus::PREP_OR_IDLE);
            while (true) {
                if (_stopped) {
                    return; // Spooler is being destroyed
                }
                if (!_paused && _countAvailableRequests(lock) > 0) {
                    break; // Ready to work
                }
                _cv_workerSync.wait(lock);
            }
            _setWorkerStatus(WorkerStatus::WORKING);

            requestVariant = _takeRequestWithHighestPriority(lock);
            _adjustUnloadPriority(requestVariant, lock);
        }

        /* WORK */
        HG_ASSERT(!std::holds_alternative<std::monostate>(requestVariant));

        if (HOLDS_LOAD_REQUEST(requestVariant)) {
            const auto& loadRequest = std::get<LoadRequest>(requestVariant);
            {
                std::unique_lock<Mutex> lock{_mutex};
                const auto iter = _unloadRequests.find(loadRequest.chunkId);
                if (iter != _unloadRequests.end()) {
                    _loadedChunks.emplace_back(std::move(iter->second), loadRequest.chunkId);
                    _unloadRequests.erase(iter);
                    continue;
                }
            }
            auto chunk = LoadChunk(loadRequest.chunkId, *_diskIoHandler);
            {
                std::unique_lock<Mutex> lock{_mutex};
                _loadedChunks.emplace_back(std::move(chunk), loadRequest.chunkId);
            }
        } else {
            const auto& unloadRequest = std::get<UnloadRequest>(requestVariant);
            UnloadChunk(unloadRequest.chunk, unloadRequest.id, *_diskIoHandler);
        }
    }
}

void DefaultChunkSpooler::_setWorkerStatus(WorkerStatus aWorkerStatus) {
    _workerStatus = aWorkerStatus;
    _cv_workerStatus.notify_all();
}

hg::PZInteger DefaultChunkSpooler::_countAvailableRequests(const std::unique_lock<Mutex>&) const {
    return hg::stopz(_loadRequests.size() + _unloadRequests.size());
}

DefaultChunkSpooler::LoadRequest DefaultChunkSpooler::_takeLoadRequestWithHighestPriority(
    const std::unique_lock<Mutex>&) {
    auto result = _loadRequests.back();
    _loadRequests.pop_back();
    return {result};
}

DefaultChunkSpooler::UnloadRequest DefaultChunkSpooler::_takeUnloadRequestWithHighestPriority(
    const std::unique_lock<Mutex>&) {
    const auto iter   = _unloadRequests.end();
    auto       result = UnloadRequest{std::move(iter->second), iter->first};
    _unloadRequests.erase(iter);
    return {result};
}

DefaultChunkSpooler::RequestVariant DefaultChunkSpooler::_takeRequestWithHighestPriority(
    const std::unique_lock<Mutex>& aLock) {
    HG_ASSERT(!_loadRequests.empty() || !_unloadRequests.empty());

    if (_unloadRequests.empty()) {
        return _takeLoadRequestWithHighestPriority(aLock);
    }

    if (_loadRequests.empty()) {
        return _takeUnloadRequestWithHighestPriority(aLock);
    }

    if (_loadRequests.back().priority <= _unloadPriority) {
        return _takeLoadRequestWithHighestPriority(aLock);
    } else {
        return _takeUnloadRequestWithHighestPriority(aLock);
    }
}

void DefaultChunkSpooler::_adjustUnloadPriority(const RequestVariant& aRequestVariant,
                                                const std::unique_lock<Mutex>&) {
    HG_ASSERT(!std::holds_alternative<std::monostate>(aRequestVariant));

    // Simple case: when there are no unload requests, return the priority to default
    if (_unloadRequests.empty()) {
        _unloadPriority = UNLOAD_REQUEST_DEFAULT_PRIORITY;
        return;
    }

    // There are too many unload requests piles up: assuming there are no negative
    // priorities for load requests (the expected case), this will shed at least
    // [UNLOAD_REQUEST_THRESHOLD / LOADS_PER_UNLOAD] unload requests before
    // continuing with any loads.
    if (_unloadRequests.size() >= UNLOAD_REQUEST_THRESHOLD) {
        _unloadPriority = -UNLOAD_REQUEST_THRESHOLD;
        HG_LOG_WARN(
            LOG_ID,
            "There is an overwhelming amount of unload requests ({}), unload priority set to {}.",
            _unloadRequests.size(),
            _unloadPriority);
        return;
    }

    if (HOLDS_LOAD_REQUEST(aRequestVariant)) {
        _unloadPriority -= 1;
    } else {
        _unloadPriority += LOADS_PER_UNLOAD;
    }
}

} // namespace detail
} // namespace gridworld
