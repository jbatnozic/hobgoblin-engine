#pragma once

#include "Chunk_spooler_default.hpp"

#include <Hobgoblin/HGExcept.hpp>

#include <algorithm>

namespace gridworld {
namespace detail {

DefaultChunkSpooler::DefaultChunkSpooler() {
    _worker = std::thread{&DefaultChunkSpooler::_workerBody, this};
}

DefaultChunkSpooler::~DefaultChunkSpooler() {

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
    _cv.notify_all();
}

bool DefaultChunkSpooler::isPaused() const {
    std::unique_lock<Mutex> lock{_mutex};
    const auto result = _paused;
    lock.unlock();
    return result;
}

void DefaultChunkSpooler::setChunksToLoad(std::vector<LoadRequest> aLoadRequests) {
    std::sort(aLoadRequests.begin(), aLoadRequests.end(), [](const LoadRequest& aLhs, const LoadRequest& aRhs) {
        // We want to sort it so that the highest priorities (lowest numbers)
        // end up at the end of the vector -- sorting in descending order.
        // note: Returning true means aLhs comes before aRhs
        return (aLhs.priority > aRhs.priority);
    });

    std::unique_lock<Mutex> lock{_mutex};
    _loadRequests = std::move(aLoadRequests);
}

std::optional<Chunk> DefaultChunkSpooler::loadImmediately(ChunkId aChunkId) {
    std::unique_lock<Mutex> lock{_mutex};

    HG_VALIDATE_PRECONDITION(_paused && "Spooler must be paused when this method is called");

    // TODO...
}

void DefaultChunkSpooler::unloadChunk(ChunkId aChunkId, Chunk aChunk) {

}

void DefaultChunkSpooler::unloadRuntimeCache() {

}

std::vector<Chunk> DefaultChunkSpooler::getLoaded() {

}

void DefaultChunkSpooler::_workerBody() {
    while (true) {
        LoadRequest request;

        /* SYNCHRONIZATION */
        {
            std::unique_lock<Mutex> lock{_mutex};
            _workerStatus = WorkerStatus::PREP_OR_IDLE;
            while (true) {
                if (_stopped) {
                    return; // Spooler is being destroyed
                }
                if (!_paused && _countAvailableRequests(lock) > 0) {
                    break; // Ready to work
                }
                _cv.wait(lock);
            }
            _workerStatus = WorkerStatus::WORKING;

            request = _takeRequestWithHighestPriority(lock);
            lock.unlock();
        }

        /* WORK */
    }
}

hg::PZInteger DefaultChunkSpooler::_countAvailableRequests(const std::unique_lock<Mutex>&) const {
    return hg::stopz(_loadRequests.size());
}

DefaultChunkSpooler::LoadRequest DefaultChunkSpooler::_takeRequestWithHighestPriority(const std::unique_lock<Mutex>&) {
    HG_ASSERT(!_loadRequests.empty());
    const auto result = _loadRequests.back();
    _loadRequests.pop_back();
    return result;
}

} // namespace detail
} // namespace gridworld
