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
        const auto cost = _worldCostFunc(aPosition + _offset, _wcfData);
        // HG_LOG_DEBUG(LOG_ID,
        //              "Cost @ ({},{}) = {}",
        //              (aPosition + _offset).x,
        //              (aPosition + _offset).y,
        //              cost);
        return cost;
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

    OffsetCostProvider costProvider;
    FlowFieldCalculator<OffsetCostProvider> flowFieldCalculator;
    std::atomic_int remainingJobCount{0};
    bool isOccupied = false;
};

struct Request {
    Request() = default;

    // Prevent copying and moving because `Job` has to be
    // able to hold a pointer to an instance of `Request.
    Request(const Request&) = delete;
    Request& operator=(const Request&) = delete;
    Request(Request&&) = delete;
    Request& operator=(Request&&) = delete;

    math::Vector2pz fieldTopLeft;
    PZInteger remainingIterations;

    std::optional<FlowField> result = {};

    Semaphore latch{0}; // TODO
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

    Request* request = nullptr;
};

class FlowFieldSpoolerImpl : public FlowFieldSpoolerImplInterface {
public:
    FlowFieldSpoolerImpl(PZInteger aConcurrencyLimit,
                         WorldCostFunction aWorldCostFunc,
                         void* aWcfData)
        : _worldCostFunc{aWorldCostFunc}
        , _wcfData{aWcfData}
    {
        _stations.Do([=](StationList& aIt) {
            for (PZInteger i = 0; i < aConcurrencyLimit; i += 1) {
                aIt.emplace_back(aWorldCostFunc, aWcfData);
            }
        });

        for (PZInteger i = 0; i < aConcurrencyLimit; i += 1) {
            _workers.emplace_back(&FlowFieldSpoolerImpl::_workerBody, this, i);
        }

        HG_LOG_INFO(LOG_ID, "Spooler created with aConcurrencyLimit={}.", aConcurrencyLimit);
    }

    ~FlowFieldSpoolerImpl() override {
        HG_LOG_INFO(LOG_ID, "Spooler stopping...");

        _stopped.store(true);
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

    std::optional<OffsetFlowField> collectResult(std::uint64_t aRequestId) override;

private:
    WorldCostFunction _worldCostFunc;
    void* _wcfData;

    using StationList = std::list<Station>;
    using RequestMap  = std::unordered_map<std::uint64_t, std::unique_ptr<Request>>;
    using JobVector   = std::vector<Job>;

    Monitor<StationList> _stations;
    Monitor<RequestMap>  _requests;
    Monitor<JobVector>   _jobs; // Could do without the Monitor

    std::atomic_uint64_t _requestIdCounter{0};

    using Mutex = std::mutex;
    Mutex _mutex;
    std::condition_variable _cv;

    std::atomic_bool _paused{false};
    std::atomic_bool _stopped{false};

    std::vector<std::thread> _workers;

    enum class WorkerStatus {
        PREPARING,
        WORKING
    };

    PZInteger _countAvailableJobs() const {
        return _jobs.Do([](const JobVector& aIt) {
            return stopz(aIt.size());
        });
    }

    void _setWorkerStatus(PZInteger aWorkerId, WorkerStatus aStatus) {
        // TODO
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

        return 0;
    }

    static Job _takeJob(JobVector& aJobs, PZInteger aWorkerId) {
      HG_ASSERT(!aJobs.empty());

      std::size_t  pickedJobIdx = 0;
      std::int32_t pickedJobScore = _calcJobScore(aJobs[0], aWorkerId);
      for (std::size_t i = 1; i < aJobs.size(); i += 1) {
          const auto& current = aJobs[i];
          const auto score = _calcJobScore(current, aWorkerId);
          if (score > pickedJobScore) {
            pickedJobIdx = i;
            pickedJobScore = score;
          }
      }

      const Job result = aJobs[pickedJobIdx];
      std::swap(aJobs[pickedJobIdx], aJobs[aJobs.size() - 1]);
      aJobs.pop_back();
      return result;
    } 

    void _workerBody(PZInteger aWorkerId) {
        while (true) {
            Job job;

            /* SYNCHRONIZATION */
            HG_LOG_DEBUG(LOG_ID, "Worker {} entering SYNCHRONIZATION block...", aWorkerId);
            {
                std::unique_lock<Mutex> lock{_mutex};
                _setWorkerStatus(aWorkerId, WorkerStatus::PREPARING);
                while (true) {
                    if (_stopped.load()) {
                      return; // Spooler is being destroyed
                    }
                    if (!_paused.load() && _countAvailableJobs() > 0) {
                      break; // Ready to work
                    }
                    _cv.wait(lock);
                }
                _setWorkerStatus(aWorkerId, WorkerStatus::WORKING);

                _jobs.Do([this, &job, aWorkerId](JobVector& aIt) {
                    job = _takeJob(aIt, aWorkerId);
                });
                lock.unlock();
            }
            
            /* WORK */
            HG_LOG_DEBUG(LOG_ID, "Worker {} entering WORK block...", aWorkerId);

            switch (job.kind) {
            case Job::CALCULATE_INTEGRATION_FIELD:
                HG_LOG_DEBUG(LOG_ID, "Worker {} starting a CALCULATE_INTEGRATION_FIELD job...", aWorkerId);
                _workCalculateIntegrationFieldJob(job, aWorkerId);
                break;

            case Job::CALCULATE_FLOW_FIELD_PART:
                HG_LOG_DEBUG(LOG_ID,
                             "Worker {} starting a CALCULATE_FLOW_FIELD_PART ({}-{}) job...",
                             aWorkerId,
                             job.calcFlowFieldPartData.startingRow,
                             job.calcFlowFieldPartData.startingRow + job.calcFlowFieldPartData.rowCount - 1);
                _workCalculateFlowFieldPartJob(job, aWorkerId);
                break;

            default:
                HG_UNREACHABLE("Invalid Job::Kind value ({}).", (int)job.kind);
            }
        }
    }

    void _workCalculateIntegrationFieldJob(Job& aJob, PZInteger aWorkerId) {
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
                _setWorkerStatus(aWorkerId, WorkerStatus::PREPARING);
                while (true) {
                    if (_stopped.load()) {
                      return; // Spooler is being destroyed
                    }
                    if (!_paused.load()) {
                      break; // Ready to work
                    }
                    _cv.wait(lock);
                }
                _setWorkerStatus(aWorkerId, WorkerStatus::WORKING);
                lock.unlock();
            }

            /* WORK */
            if (station->flowFieldCalculator.calculateIntegrationField(256)) {
                HG_LOG_DEBUG(LOG_ID, "Worker {} finished calculating an integration field.", aWorkerId);
                break;
            }
        }

        const PZInteger _concurrencyLimit = 8;
        const auto totalRowCount = aJob.calcIntegrationFieldData.fieldDimensions.y;
        const auto rowsPerJob = (totalRowCount + (_concurrencyLimit - 1)) / _concurrencyLimit;

        std::vector<Job> newJobs;
        PZInteger rowsCovered = 0;
        while (rowsCovered < totalRowCount) {
            newJobs.emplace_back();
            auto& job = newJobs.back();

            job.kind = Job::CALCULATE_FLOW_FIELD_PART;
            job.calcFlowFieldPartData.station = station;
            job.calcFlowFieldPartData.startingRow = rowsCovered;
            job.calcFlowFieldPartData.rowCount =
                (rowsCovered + rowsPerJob < totalRowCount) ? rowsPerJob : (totalRowCount - rowsCovered);
            job.calcFlowFieldPartData.preferredWorkerId = aWorkerId;
            job.request = aJob.request;

            rowsCovered += rowsPerJob;
        }

        station->remainingJobCount.store(static_cast<int>(newJobs.size()));

        _jobs.Do([&newJobs](JobVector& aIt) {
            for (const auto& job : newJobs) {
                aIt.emplace_back(job);
            }
        });
        _cv.notify_all(); // TODO temp.
    }

    void _workCalculateFlowFieldPartJob(Job& aJob, PZInteger aWorkerId) {
        while (true) {
            /* SYNCHRONIZATION */
            {
                std::unique_lock<Mutex> lock{_mutex};
                _setWorkerStatus(aWorkerId, WorkerStatus::PREPARING);
                while (true) {
                    if (_stopped.load()) {
                      return; // Spooler is being destroyed
                    }
                    if (!_paused.load()) {
                      break; // Ready to work
                    }
                    _cv.wait(lock);
                }
                _setWorkerStatus(aWorkerId, WorkerStatus::WORKING);
                lock.unlock();
            }

            /* WORK */
            const auto& jobData = aJob.calcFlowFieldPartData;
            // TODO: not great for pausing ability; refactor (maybe 1 row at a time?)
            jobData.station->flowFieldCalculator.calculateFlowField(jobData.startingRow, jobData.rowCount);

            const auto fsRes = jobData.station->remainingJobCount.fetch_sub(1);
            HG_LOG_DEBUG(LOG_ID, "Worker {} fsRes = {}.", aWorkerId, fsRes);
            if (fsRes == 1) {
                // There was exactly one job left - this one - so the calculation is finished
                auto flowField = jobData.station->flowFieldCalculator.takeFlowField();
                aJob.request->result = {std::move(*flowField)};
                HG_LOG_DEBUG(LOG_ID, "Worker {} signalling latch.", aWorkerId);
                aJob.request->latch.signal();
                jobData.station->isOccupied = false;
            }

            break; // TODO Temp.
        }
    }
};

void FlowFieldSpoolerImpl::tick() {
    _requests.Do([](RequestMap& aIt) {
        for (auto& pair : aIt) {
            auto& request = *pair.second;

            if (request.remainingIterations > 1) {
                request.remainingIterations -=1;
            } else if (request.remainingIterations == 1) {
                request.remainingIterations -=1;
                request.latch.wait();
            }
        }
    });
}

void FlowFieldSpoolerImpl::pause() {
    // TODO
}

void FlowFieldSpoolerImpl::unpause() {
    _paused.store(false);
    _cv.notify_all();
}

std::uint64_t FlowFieldSpoolerImpl::addRequest(math::Vector2pz aFieldTopLeft,
                                               math::Vector2pz aFieldDimensions,
                                               math::Vector2pz aTarget,
                                               PZInteger aMaxIterations) {
    // TODO: error checking for parameters
    HG_VALIDATE_ARGUMENT(aMaxIterations > 0, "aMaxIterations must be at least 1.");

    const auto id = _requestIdCounter.fetch_add(1);
    auto request = std::make_unique<Request>();
    request->fieldTopLeft = aFieldTopLeft;
    request->remainingIterations = aMaxIterations;

    Job job;
    job.kind = Job::CALCULATE_INTEGRATION_FIELD;
    job.calcIntegrationFieldData.fieldTopLeft    = aFieldTopLeft;
    job.calcIntegrationFieldData.fieldDimensions = aFieldDimensions;
    job.calcIntegrationFieldData.target          = aTarget;
    job.request = request.get();

    _requests.Do([request_ = std::move(request), id](RequestMap& aIt) mutable {
        aIt[id] = std::move(request_);
    });

    _jobs.Do([&job](JobVector& aIt) {
        aIt.push_back(job);
    });
    _cv.notify_all(); // TODO temp.

    return id;
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
