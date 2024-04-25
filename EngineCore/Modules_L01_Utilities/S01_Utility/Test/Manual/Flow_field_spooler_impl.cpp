// Copyright 2024 Jovan Batnozic. Released under MS-PL licence in Serbia.
// See https://github.com/jbatnozic/Hobgoblin?tab=readme-ov-file#licence

#include "Flow_field_spooler_impl.hpp"

#include <Hobgoblin/Common.hpp>
#include <Hobgoblin/HGExcept.hpp>
#include <Hobgoblin/Logging.hpp>
#include <Hobgoblin/Math/Vector.hpp>
#include <Hobgoblin/Utility/Flow_field_calculator.hpp>
#include <Hobgoblin/Utility/Monitor.hpp>
#include <Hobgoblin/Utility/Semaphore.hpp> // TODO Temp.

#include <atomic>
#include <condition_variable>
#include <cstdint>
#include <limits>
#include <list>
#include <memory>
#include <mutex>
#include <optional>
#include <unordered_map>
#include <thread>
#include <vector>

#include <Hobgoblin/Private/Pmacro_define.hpp>

HOBGOBLIN_NAMESPACE_BEGIN
namespace util {
namespace detail {

namespace {

constexpr auto LOG_ID = "Hobgoblin.Utility";

class OffsetCostProvider {
public:
    OffsetCostProvider(WorldCostFunction aWorldCostFunc, void* aWcfData)
        : _worldCostFunc{aWorldCostFunc}
        , _wcfData{aWcfData}
    {
        HG_VALIDATE_ARGUMENT(aWorldCostFunc != nullptr);
        HG_VALIDATE_ARGUMENT(aWcfData != nullptr);
    }

    void setOffset(math::Vector2pz aOffset) {
        _offset = aOffset;
    }

    std::uint8_t getCostAt(math::Vector2pz aPosition) const {
        return _worldCostFunc(aPosition + _offset, _wcfData);
    }

private:
    WorldCostFunction _worldCostFunc;
    void* _wcfData;
    math::Vector2pz _offset = {0, 0};
};

struct Station {
    Station(WorldCostFunction aWorldCostFunc, void* aWcfData)
        : costProvider{aWorldCostFunc, aWcfData}
    {
    }

    // Prevent copying and moving because `Job` has to be
    // able to hold a pointer to an instance of `Station.
    Station(const Station&) = delete;
    Station& operator=(const Station&) = delete;
    Station(Station&&) = delete;
    Station& operator=(Station&&) = delete;

    ///////////////////////////////////////

    OffsetCostProvider costProvider;
    FlowFieldCalculator<OffsetCostProvider> flowFieldCalculator;
    std::atomic_int remainingJobCount{0};
    bool isOccupied = false;
};

struct Request {
    Request(std::uint64_t aRequestId)
        : id{aRequestId} {}

    ~Request() {
        HG_LOG_DEBUG(LOG_ID, "Flow Field Request (id: {}) destroyed.", id);
    }

    // Prevent copying and moving because `Job` has to be
    // able to hold a pointer to an instance of `Request.
    Request(const Request&) = delete;
    Request& operator=(const Request&) = delete;
    Request(Request&&) = delete;
    Request& operator=(Request&&) = delete;

    ///////////////////////////////////////

    std::uint64_t id;

    math::Vector2pz fieldTopLeft{};
    PZInteger remainingIterations{0};

    std::optional<FlowField> result{};

    Semaphore latch{0}; // TODO

    std::atomic_bool cancelled{false};
};

struct Job {
    Job() {}

    enum Kind {
        CALCULATE_INTEGRATION_FIELD,
        CALCULATE_FLOW_FIELD_PART
    };

    Kind kind;

    struct CalculateIntegrationFieldData {
        math::Vector2pz fieldTopLeft;
        math::Vector2pz fieldDimensions;
        math::Vector2pz target;
    };

    struct CalculateFlowFieldPartData {
        Station*  station;
        PZInteger preferredWorkerId;
        PZInteger startingRow;
        PZInteger rowCount;
    };

    union {
        //! Used only when kind==CALCULATE_INTEGRATION_FIELD.
        CalculateIntegrationFieldData calcIntegrationFieldData;

        //! Used only when kind==CALCULATE_FLOW_FIELD_PART.
        CalculateFlowFieldPartData calcFlowFieldPartData;
    };

    //! Original request that this job relates to.
    std::shared_ptr<Request> request = nullptr;
};

class FlowFieldSpoolerImpl : public FlowFieldSpoolerImplInterface {
public:
    FlowFieldSpoolerImpl(PZInteger aConcurrencyLimit,
                         WorldCostFunction aWorldCostFunc,
                         void* aWcfData)
        : _concurrencyLimit{aConcurrencyLimit}
        , _worldCostFunc{aWorldCostFunc}
        , _wcfData{aWcfData}
    {
        HG_LOG_INFO(LOG_ID, "Creating Spooler with aConcurrencyLimit={}...", aConcurrencyLimit);

        // Create stations
        _stations.Do([=](StationList& aIt) {
            for (PZInteger i = 0; i < aConcurrencyLimit; i += 1) {
                aIt.emplace_back(aWorldCostFunc, aWcfData);
            }
        });

        // Create & start worker threads
        _workers.reserve(pztos(aConcurrencyLimit));
        _workerStatuses.reserve(_workers.size());
        for (PZInteger i = 0; i < aConcurrencyLimit; i += 1) {
            _workers.emplace_back(&FlowFieldSpoolerImpl::_workerBody, this, i);
            _workerStatuses.push_back(WorkerStatus::PREP_OR_IDLE);
        }
    }

    ~FlowFieldSpoolerImpl() override {
        HG_LOG_INFO(LOG_ID, "Stopping Spooler...");

        {
            std::unique_lock<Mutex> lock{_mutex};
            _stopped = true;
        }
        _cv.notify_all();

        for (auto& worker : _workers) {
            worker.join();
        }
    }

    void tick() override;

    void pause() override;

    void unpause() override;

    std::uint64_t addRequest(math::Vector2pz aFieldTopLeft,
                             math::Vector2pz aFieldDimensions,
                             math::Vector2pz aTarget,
                             PZInteger aMaxIterations) override;

    void cancelRequest(std::uint64_t aRequestId) override;

    std::optional<OffsetFlowField> collectResult(std::uint64_t aRequestId) override;

private:
    PZInteger _concurrencyLimit;
    const WorldCostFunction _worldCostFunc;
    void* const _wcfData;

    ///////////////////////////////////////  

    using StationList = std::list<Station>;
    using RequestMap  = std::unordered_map<std::uint64_t, std::shared_ptr<Request>>;

    Monitor<StationList> _stations;
    Monitor<RequestMap>  _requests;
    
    std::atomic_uint64_t _requestIdCounter{0};

    ///////////////////////////////////////   

    using Mutex = std::mutex;
    Mutex _mutex; // Protects: _jobs, _paused, _stopped, _workerStatuses
    std::condition_variable _cv;

    std::vector<Job> _jobs;

    bool _paused{false};
    bool _stopped{false};

    enum class WorkerStatus : std::int8_t {
        PREP_OR_IDLE, //!< Preparing to work or idling
        WORKING       //!< Working
    };

    std::vector<std::thread>  _workers;
    std::vector<WorkerStatus> _workerStatuses;
    std::condition_variable   _cv_workerStatuses;

    /////////////////////////////////////// 

    void _setWorkerStatus(PZInteger aWorkerId, WorkerStatus aStatus, const std::unique_lock<Mutex>&) {
        _workerStatuses[pztos(aWorkerId)] = aStatus;
        _cv_workerStatuses.notify_all();
    }

    void _addJob(const Job& aNewJob, const std::unique_lock<Mutex>&) {
        _jobs.emplace_back(aNewJob);
        _cv.notify_all(); // If a worker was blocked waiting for a job, unblock it
    }

    void _addJobs(const std::vector<Job>& aNewJobs, const std::unique_lock<Mutex>&) {
        for (const auto& job : aNewJobs) {
            _jobs.emplace_back(job);
        }
        _cv.notify_all(); // If a worker(s) was blocked waiting for a job, unblock it(them)
    }

    PZInteger _countAvailableJobs(const std::unique_lock<Mutex>&) const {
        return stopz(_jobs.size());
    }

    static std::int32_t _calcJobScore(const Job& aJob, PZInteger aWorkerId) {
        // PRIORITIES (Highest to lowest)
        // 1. CALCULATE_FLOW_FIELD_PART job preferred by this thread
        // 2. CALCULATE_INTEGRATION_FIELD job (any)
        // 3. CALCULATE_FLOW_FIELD_PART job of another thread - when there is nothing better to do
        // 
        // A sufficiently urgent (3) can have a higher priority than (2)

        if (aJob.kind == Job::CALCULATE_FLOW_FIELD_PART &&
            aJob.calcFlowFieldPartData.preferredWorkerId == aWorkerId) {
            return std::numeric_limits<std::int32_t>::max();
        }

        std::int32_t score = 0;
        if (aJob.kind == Job::CALCULATE_INTEGRATION_FIELD) {
            score += 1999;
        }
        score -= (aJob.request->remainingIterations * 1000);

        return score;
    }

    Job _takeJob(PZInteger aWorkerId, const std::unique_lock<Mutex>&) {
      HG_ASSERT(!_jobs.empty());

      std::size_t  pickedJobIdx = 0;
      std::int32_t pickedJobScore = _calcJobScore(_jobs[0], aWorkerId);
      for (std::size_t i = 1; i < _jobs.size(); i += 1) {
          const auto& current = _jobs[i];
          const auto score = _calcJobScore(current, aWorkerId);
          if (score > pickedJobScore) {
            pickedJobIdx = i;
            pickedJobScore = score;
          }
      }

      const Job result = _jobs[pickedJobIdx];
      std::swap(_jobs[pickedJobIdx], _jobs[_jobs.size() - 1]);
      _jobs.pop_back();
      return result;
    } 

    void _workerBody(PZInteger aWorkerId) {
        while (true) {
            Job job;

            /* SYNCHRONIZATION */
            {
                std::unique_lock<Mutex> lock{_mutex};
                _setWorkerStatus(aWorkerId, WorkerStatus::PREP_OR_IDLE, lock);
                while (true) {
                    if (_stopped) {
                        return; // Spooler is being destroyed
                    }
                    if (!_paused && _countAvailableJobs(lock) > 0) {
                        break; // Ready to work
                    }
                    _cv.wait(lock);
                }
                _setWorkerStatus(aWorkerId, WorkerStatus::WORKING, lock);

                job = _takeJob(aWorkerId, lock);
                lock.unlock();
            }
            
            const auto requestId = job.request->id;

            /* WORK */
            switch (job.kind) {
            case Job::CALCULATE_INTEGRATION_FIELD:
                {
                    HG_LOG_DEBUG(LOG_ID, "Worker {} starting a CALCULATE_INTEGRATION_FIELD job for request {}...", aWorkerId, requestId);
                    Station* station = _workCalculateIntegrationFieldJob(job, aWorkerId);
                    HG_LOG_DEBUG(LOG_ID, "Worker {} finished a CALCULATE_INTEGRATION_FIELD job for request {}.", aWorkerId, requestId); // TODO: print if cancelled
                    /* TODO: IF NOT CANCELLED */_finalizeCalculateIntegrationFieldJob(job, aWorkerId, station);
                }
                break;

            case Job::CALCULATE_FLOW_FIELD_PART:
                {
                    const auto startRow = job.calcFlowFieldPartData.startingRow;
                    const auto endRow = startRow + job.calcFlowFieldPartData.rowCount - 1;
                    HG_LOG_DEBUG(LOG_ID,
                                 "Worker {} starting a CALCULATE_FLOW_FIELD_PART ({}-{}) job for request {}...",
                                 aWorkerId,
                                 startRow,
                                 endRow,
                                 requestId);
                    _workCalculateFlowFieldPartJob(job, aWorkerId);
                    HG_LOG_DEBUG(LOG_ID,
                                 "Worker {} finished a CALCULATE_FLOW_FIELD_PART ({}-{}) job for request {}.",
                                 aWorkerId,
                                 startRow,
                                 endRow,
                                 requestId); // TODO: print if cancelled
                    _finalizeCalculateFlowFieldPartJob(job, aWorkerId);
                }
                break;

            default:
                HG_UNREACHABLE("Invalid Job::Kind value ({}).", (int)job.kind);
            }
        }
    }

    //! Note: call without holding the mutex.
    //! \returns pointer to station that was assigned to this Job/Request (never NULL).
    Station* _workCalculateIntegrationFieldJob(Job& aJob, PZInteger aWorkerId) {
        Station* const station = _stations.Do([&aJob](StationList& aIt) -> Station* {
            for (auto& station : aIt) {
                if (!station.isOccupied) {
                    const auto& jobData = aJob.calcIntegrationFieldData;
                    station.costProvider.setOffset(jobData.fieldTopLeft);
                    station.flowFieldCalculator.reset(jobData.fieldDimensions,
                                                      jobData.target,
                                                      station.costProvider);
                    station.isOccupied = true;
                    return &station;
                }
            }
            return nullptr;
        });
        HG_HARD_ASSERT(station != nullptr);

        while (true) {
            /* SYNCHRONIZATION */
            {
                std::unique_lock<Mutex> lock{_mutex};
                _setWorkerStatus(aWorkerId, WorkerStatus::PREP_OR_IDLE, lock);
                while (true) {
                    if (_stopped) {
                        return station; // Spooler is being destroyed
                    }
                    if (aJob.request->cancelled.load()) {
                        return station; // Request cancelled
                    }
                    if (!_paused) {
                        break; // Ready to work
                    }
                    _cv.wait(lock);
                }
                _setWorkerStatus(aWorkerId, WorkerStatus::WORKING, lock);
                lock.unlock();
            }

            /* WORK */
            if (station->flowFieldCalculator.calculateIntegrationField(256)) {
                HG_LOG_DEBUG(LOG_ID, "Worker {} finished calculating an integration field.", aWorkerId);
                break;
            }
        }

        return station;
    }

    //! Note: call without holding the mutex.
    void _finalizeCalculateIntegrationFieldJob(Job& aJob, PZInteger aWorkerId, Station* aStation) {
        const auto totalRowCount = aJob.calcIntegrationFieldData.fieldDimensions.y;
        const auto rowsPerJob = (totalRowCount + (_concurrencyLimit - 1)) / _concurrencyLimit;

        std::vector<Job> newJobs;
        PZInteger rowsCovered = 0;
        while (rowsCovered < totalRowCount) {
            newJobs.emplace_back();
            auto& job = newJobs.back();

            job.kind = Job::CALCULATE_FLOW_FIELD_PART;
            job.calcFlowFieldPartData.station = aStation;
            job.calcFlowFieldPartData.startingRow = rowsCovered;
            job.calcFlowFieldPartData.rowCount =
                (rowsCovered + rowsPerJob < totalRowCount) ? rowsPerJob : (totalRowCount - rowsCovered);
            job.calcFlowFieldPartData.preferredWorkerId = aWorkerId;
            job.request = aJob.request;

            rowsCovered += rowsPerJob;
        }

        aStation->remainingJobCount.store(static_cast<int>(newJobs.size()));

        {
            std::unique_lock<Mutex> lock{_mutex};
            _addJobs(newJobs, lock);
        }
    }

    //! Note: call without holding the mutex.
    void _workCalculateFlowFieldPartJob(Job& aJob, PZInteger aWorkerId) {
        while (true) {
            /* SYNCHRONIZATION */
            {
                std::unique_lock<Mutex> lock{_mutex};
                _setWorkerStatus(aWorkerId, WorkerStatus::PREP_OR_IDLE, lock);
                while (true) {
                    if (_stopped) {
                        return; // Spooler is being destroyed
                    }
                    if (aJob.request->cancelled.load()) {
                        return; // Request cancelled
                    }
                    if (!_paused) {
                        break; // Ready to work
                    }
                    _cv.wait(lock);
                }
                _setWorkerStatus(aWorkerId, WorkerStatus::WORKING, lock);
                lock.unlock();
            }

            /* WORK */
            const auto& jobData = aJob.calcFlowFieldPartData;
            // TODO: not great for pausing ability; refactor (maybe 1 row at a time?)
            jobData.station->flowFieldCalculator.calculateFlowField(jobData.startingRow, jobData.rowCount);

            break; // TODO Temp.
        }
    }

    //! Note: call without holding the mutex.
    void _finalizeCalculateFlowFieldPartJob(Job& aJob, PZInteger aWorkerId) {
        HG_ASSERT(aJob.kind == Job::CALCULATE_FLOW_FIELD_PART);

        const auto& jobData = aJob.calcFlowFieldPartData;
        const auto fsRes = jobData.station->remainingJobCount.fetch_sub(1);

        HG_LOG_DEBUG(LOG_ID, "Worker {} fsRes = {}.", aWorkerId, fsRes);
        if (fsRes == 1) {
            // There was exactly one job left - this one - so the calculation is finished
            auto flowField = jobData.station->flowFieldCalculator.takeFlowField();
            aJob.request->result = {std::move(flowField)};
            HG_LOG_DEBUG(LOG_ID, "Worker {} signalling latch.", aWorkerId);
            aJob.request->latch.signal();
            jobData.station->isOccupied = false;
        }
    }
};

void FlowFieldSpoolerImpl::tick() {
    // TODO: throw exception if _paused==true (otherwise there could be a deadlock)
    _requests.Do([](RequestMap& aIt) {
        for (auto& pair : aIt) {
            auto& request = *pair.second;

            if (request.remainingIterations > 1) {
                request.remainingIterations -=1;
            } else if (request.remainingIterations == 1) {
                request.remainingIterations -=1;
                request.latch.wait(); // Warning: will block all access to _requests until resolved!
            }
        }
    });
}

void FlowFieldSpoolerImpl::pause() {
    std::unique_lock<Mutex> lock{_mutex};

    _paused = true;
    
    _cv_workerStatuses.wait(lock, [this]() -> bool {
        for (const auto status : _workerStatuses) {
            if (status == WorkerStatus::WORKING) {
                return false;
            }
        }
        return true;
    });
}

void FlowFieldSpoolerImpl::unpause() {
    std::unique_lock<Mutex> lock{_mutex};
    _paused = false;
    _cv.notify_all();
}

std::uint64_t FlowFieldSpoolerImpl::addRequest(math::Vector2pz aFieldTopLeft,
                                               math::Vector2pz aFieldDimensions,
                                               math::Vector2pz aTarget,
                                               PZInteger aMaxIterations) {
    // TODO: error checking for parameters
    HG_VALIDATE_ARGUMENT(aMaxIterations > 0, "aMaxIterations must be at least 1.");

    const auto id = _requestIdCounter.fetch_add(1);
    auto request = std::make_shared<Request>(id);
    request->fieldTopLeft = aFieldTopLeft;
    request->remainingIterations = aMaxIterations;

    HG_LOG_DEBUG(
        LOG_ID,
        "Flow Field Request (id: {}; topLeft: ({},{}); size: ({}x{}); target: ({},{}), maxIter: {}) created.",
        id,
        aFieldTopLeft.x,
        aFieldTopLeft.y,
        aFieldDimensions.x,
        aFieldDimensions.y,
        aTarget.x,
        aTarget.y,
        aMaxIterations);

    Job job;
    job.kind = Job::CALCULATE_INTEGRATION_FIELD;
    job.calcIntegrationFieldData.fieldTopLeft    = aFieldTopLeft;
    job.calcIntegrationFieldData.fieldDimensions = aFieldDimensions;
    job.calcIntegrationFieldData.target          = aTarget;
    job.request = std::shared_ptr<Request>{request}; // Share ownership

    _requests.Do([request_ = std::move(request), id](RequestMap& aIt) mutable {
        aIt[id] = std::move(request_);
    });

    {
        std::unique_lock<Mutex> lock{_mutex};
        _addJob(job, lock);
    }

    return id;
}

void FlowFieldSpoolerImpl::cancelRequest(std::uint64_t aRequestId) {
    std::shared_ptr<Request> request;
    _requests.Do([aRequestId, &request](RequestMap& aIt) {
        auto iter = aIt.find(aRequestId);
        if (iter != aIt.end()) {
            std::swap(request, iter->second);
            aIt.erase(iter);
        }
    });

    if (request != nullptr) {
        request->cancelled.store(true);
        HG_LOG_INFO(LOG_ID, "Request {} cancelled.", aRequestId);
    }    
}

std::optional<OffsetFlowField> FlowFieldSpoolerImpl::collectResult(std::uint64_t aRequestId) {
    return _requests.Do([aRequestId](RequestMap& aIt) -> std::optional<OffsetFlowField> {
        const auto iter = aIt.find(aRequestId);
        HG_HARD_ASSERT(iter != aIt.end()); // TODO: different exception

        auto& request = iter->second;

        if (!request->result.has_value()) {
            return {};
        }

        OffsetFlowField result;
        result.flowField = std::move(*(request->result));
        result.offset = request->fieldTopLeft;

        aIt.erase(iter);

        return {result};
    });
}

} // namespace

std::unique_ptr<FlowFieldSpoolerImplInterface> CreateDefaultFlowFieldSpoolerImpl(
    PZInteger aConcurrencyLimit,
    WorldCostFunction aWorldCostFunc,
    void* aWcfData) {
    return std::make_unique<FlowFieldSpoolerImpl>(aConcurrencyLimit, aWorldCostFunc, aWcfData);
}

} // namespace detail
} // namespace util
HOBGOBLIN_NAMESPACE_END
