
#include "Hobgoblin/Logging/User_macros.hpp"
#include "Hobgoblin/Utility/Time_utils.hpp"
#include <SPeMPE/GameContext/Game_context.hpp>

#include <Hobgoblin/HGExcept.hpp>
#include <Hobgoblin/Logging.hpp>

#include <algorithm>
#include <chrono>
#include <iostream>

namespace jbatnozic {
namespace spempe {

namespace {
constexpr const char* LOG_ID = "SPeMPE";
} // namespace

GameContext::GameContext(const RuntimeConfig& aRuntimeConfig, hg::PZInteger aComponentTableSize)
    : _runtimeConfig{aRuntimeConfig}
    , _qaoRuntime{this}
    , _components{aComponentTableSize}
{ 
    if ((_runtimeConfig.deltaTime <= decltype(_runtimeConfig.deltaTime){0.0}) ||
        (_runtimeConfig.maxFramesBetweenDisplays <= 0)) {
        HG_THROW_TRACED(hg::InvalidArgumentError, 0,
                        "Invalid RuntimeConfig provided.");
    }
}

GameContext::~GameContext() {
    _qaoRuntime.destroyAllOwnedObjects();
    //_postStepActions.clear(); TODO

    while (!_ownedComponents.empty()) {
        ContextComponent& component = *_ownedComponents.back();
        detachComponent(component);
    }
}

///////////////////////////////////////////////////////////////////////////
// CONFIGURATION                                                         //
///////////////////////////////////////////////////////////////////////////

void GameContext::setToMode(Mode aMode) {
    // TODO
    _mode = aMode;
}

bool GameContext::isPrivileged() const {
    return ((static_cast<int>(_mode) & detail::GCMF_PRIV) != 0);
}

bool GameContext::isHeadless() const {
    return ((static_cast<int>(_mode) & detail::GCMF_HDLS) != 0);
}

bool GameContext::hasNetworking() const {
    return ((static_cast<int>(_mode) & detail::GCMF_NETW) != 0);
}

const GameContext::RuntimeConfig& GameContext::getRuntimeConfig() const {
    return _runtimeConfig;
}

///////////////////////////////////////////////////////////////////////////
// GAME STATE MANAGEMENT                                                 //
///////////////////////////////////////////////////////////////////////////

GameContext::GameState& GameContext::getGameState() {
    return _state;
}

const GameContext::GameState& GameContext::getGameState() const {
    return _state;
}

///////////////////////////////////////////////////////////////////////////
// GAME OBJECT MANAGEMENT                                                //
///////////////////////////////////////////////////////////////////////////

hg::QAO_Runtime& GameContext::getQAORuntime() {
    return _qaoRuntime;
}

///////////////////////////////////////////////////////////////////////////
// COMPONENT MANAGEMENT                                                  //
///////////////////////////////////////////////////////////////////////////

void GameContext::detachComponent(ContextComponent& aComponent) {
    _components.detachComponent(aComponent);

    _ownedComponents.erase(
        std::remove_if(_ownedComponents.begin(), _ownedComponents.end(),
                       [&aComponent](std::unique_ptr<ContextComponent>& aCurr) {
                           return aCurr.get() == &aComponent;
                       }), _ownedComponents.end());
}

std::string GameContext::getComponentTableString(char aSeparator) const {
    return _components.toString(aSeparator);
}

///////////////////////////////////////////////////////////////////////////
// EXECUTION                                                             //
///////////////////////////////////////////////////////////////////////////

int GameContext::runFor(int aSteps) {
    int rv;
    _quit.store(false);
    _runImpl(this, aSteps, &rv);
    return rv;
}

void GameContext::stop() {
    if (hasChildContext() && isChildContextJoinable()) {
        const auto childRv = stopAndJoinChildContext();
        HG_LOG_INFO(LOG_ID, "Child context stopped with exit code {}.", childRv);
    }

    _quit.store(true);
}

const GameContext::PerformanceInfo& GameContext::getPerformanceInfo() const {
    return _performanceInfo;
}

hg::PZInteger GameContext::getCurrentStepOrdinal() const {
    return _stepOrdinal;
}

///////////////////////////////////////////////////////////////////////////
// CHILD CONTEXT SUPPORT                                                 //
///////////////////////////////////////////////////////////////////////////

void GameContext::attachChildContext(std::unique_ptr<GameContext> aChildContext) {
    if (hasChildContext()) {
        HG_THROW_TRACED(hg::TracedLogicError, 0,
                        "A child context is already attached.");
    }
    _childContext = std::move(aChildContext);
    _childContext->_parentContext = this;
}

std::unique_ptr<GameContext> GameContext::detachChildContext() {
    if (hasChildContext() && isChildContextJoinable()) {
        HG_THROW_TRACED(hg::TracedLogicError, 0,
                        "Cannot detach a running child context - stop and join it first.");
    }

    return std::move(_childContext);
}

bool GameContext::hasChildContext() const {
    return (_childContext != nullptr);
}

bool GameContext::isChildContextJoinable() const {
    if (!hasChildContext()) {
        HG_THROW_TRACED(hg::TracedLogicError, 0,
                        "No child context is currently attached.");
    }
    return _childContextThread.joinable();
}

GameContext* GameContext::getChildContext() const {
    return _childContext.get();
}

void GameContext::startChildContext(int aSteps) {
    if (!hasChildContext()) {
        HG_THROW_TRACED(hg::TracedLogicError, 0,
                        "No child context is currently attached");
    }

    if (_childContextThread.joinable()) {
        HG_THROW_TRACED(hg::TracedLogicError, 0,
                        "The previous child context must be stopped and joined first.");
    }

    _childContext->_quit.store(false);
    _childContextThread =
        std::thread{_runImpl, _childContext.get(), aSteps, &_childContextReturnValue, false};
}

int GameContext::stopAndJoinChildContext() {
    if (!hasChildContext()) {
        return _childContextReturnValue;
    }

    _childContext->stop();

    if (_childContextThread.joinable()) {
        _childContextThread.join();
    }

    return _childContextReturnValue;
}

///////////////////////////////////////////////////////////////////////////
// PRIVATE METHODS                                                       //
///////////////////////////////////////////////////////////////////////////

namespace {

#ifdef NDEBUG
    // If we're in Debug mode, we don't want to catch and handle
    // exceptions, but rather let the IDE break the program.
    #define CATCH_EXCEPTIONS_TOP_LEVEL
#endif // !NDEBUG

int DoSingleQaoIteration(hg::QAO_Runtime& runtime, std::int32_t eventFlags) {
    runtime.startStep();
    bool done = false;
    do {
    #ifdef CATCH_EXCEPTIONS_TOP_LEVEL
        try {
            runtime.advanceStep(done, eventFlags);
        }
        catch (std::exception& ex) {
            HG_LOG_ERROR(LOG_ID, "Exception caught: {}", ex.what());
            return 1;
        }
        catch (...) {
            HG_LOG_ERROR(LOG_ID, "Unknown exception caught!");
            return 2;
        }
    #else
        runtime.advanceStep(done, eventFlags);
    #endif
    } while (!done);

    return 0;
}

using hg::QAO_Event;
using hg::QAO_ALL_EVENT_FLAGS;

constexpr std::int32_t ToEventMask(QAO_Event::Enum ev) {
    return (1 << static_cast<std::int32_t>(ev));
}

constexpr std::int32_t QAO_EVENT_MASK_ALL_DRAWS = ToEventMask(QAO_Event::Draw1)
                                                | ToEventMask(QAO_Event::Draw2)
                                                | ToEventMask(QAO_Event::DrawGUI);

constexpr std::int32_t QAO_EVENT_MASK_FINALIZE = ToEventMask(QAO_Event::FinalizeFrame);

constexpr std::int32_t QAO_EVENT_MASK_ALL_EXCEPT_DRAW = QAO_ALL_EVENT_FLAGS & ~QAO_EVENT_MASK_ALL_DRAWS;

constexpr std::int32_t QAO_EVENT_MASK_ALL_EXCEPT_FINALIZE = QAO_ALL_EVENT_FLAGS & ~QAO_EVENT_MASK_FINALIZE;

constexpr std::int32_t QAO_EVENT_MASK_ALL_EXCEPT_DRAW_AND_FINALIZE = QAO_EVENT_MASK_ALL_EXCEPT_DRAW
                                                                   & QAO_EVENT_MASK_ALL_EXCEPT_FINALIZE;

using TimingDuration = std::chrono::duration<double, std::micro>;
using std::chrono::milliseconds;
using std::chrono::microseconds;
using std::chrono::steady_clock;
using std::chrono::duration_cast;

template <class taDuration>
double MsCount(taDuration aDuration) {
    return static_cast<double>(duration_cast<microseconds>(aDuration).count() / 1000.0);
}
} // namespace

void GameContext::_runImpl(hg::NotNull<GameContext*> aContext,
                           int aMaxSteps,
                           hg::NotNull<int*> aReturnValue,
                           bool aDebugLoggingActive) {
    using hobgoblin::util::Stopwatch;
    #define DebugLog(...) \
        do { if (aDebugLoggingActive) HG_LOG_DEBUG(LOG_ID, __VA_ARGS__); } while (false)

    if (aMaxSteps == 0) {
        (*aReturnValue) = 0;
        return;
    }

    const auto maxConsecutiveSteps = aContext->_runtimeConfig.maxFramesBetweenDisplays;
    const TimingDuration deltaTime = aContext->_runtimeConfig.deltaTime;

    DebugLog("_runImpl - CONFIG - maxConsecutiveSteps={}, deltaTime={}ms.",
             maxConsecutiveSteps,
             MsCount(deltaTime.count()));

    hg::PZInteger stepsCovered = 0;
    TimingDuration accumulator = deltaTime;
    Stopwatch frameToFrameStopwatch;

    while (true) {    
        DebugLog("_runImpl - CYCLE start");

        if (aMaxSteps > 0 && stepsCovered >= aMaxSteps) {
            DebugLog("_runImpl - Outer while loop breaking because aMaxSteps was reached.");
            break;
        }
        if (aContext->_quit.load()) {
            DebugLog("_runImpl - Outer while loop breaking because _quit was set to true.");
            break;
        }

        bool didAtLeastOneStep = false;
        for (int i = 0; i < maxConsecutiveSteps; i += 1) {
            if (aMaxSteps > 0 && stepsCovered >= aMaxSteps) {
                DebugLog("_runImpl - Inner for loop breaking because aMaxSteps was reached.");
                break;
            }
            if (accumulator < deltaTime) {
                DebugLog("_runImpl - Inner for loop breaking because accumulator is too low ({}ms).",
                         MsCount(accumulator.count()));
                break;
            }
            if (aContext->_quit.load()) {
                DebugLog("_runImpl - Inner for loop breaking because _quit was set to true.");
                break;
            }

            // STEP
            DebugLog("_runImpl - STEP start");
            (*aReturnValue) = DoSingleQaoIteration(aContext->_qaoRuntime,
                                                   QAO_EVENT_MASK_ALL_EXCEPT_DRAW_AND_FINALIZE);
            DebugLog("_runImpl - STEP end (status = {})", *aReturnValue);
            if ((*aReturnValue) != 0) {
                return;
            }

            didAtLeastOneStep = true;
            accumulator -= deltaTime;

            stepsCovered += 1;
            aContext->_stepOrdinal += 1;
            
            // TODO(poll post step actions)
        } // End for

        if (didAtLeastOneStep && !aContext->isHeadless()) {
            // DRAW
            DebugLog("_runImpl - DRAW start");
            (*aReturnValue) = DoSingleQaoIteration(aContext->_qaoRuntime,
                                                   QAO_EVENT_MASK_ALL_DRAWS);
            DebugLog("_runImpl - DRAW end (status = {})", *aReturnValue);
            if ((*aReturnValue) != 0) {
                return;
            }
        }

        if (didAtLeastOneStep) {
             // Prevent excessive buildup in accumulator in case
             // the program is not meeting time requirements
             const auto accBefore = accumulator;
             accumulator = std::min(accumulator, deltaTime * 0.5);
             if (accumulator != accBefore) {
                 DebugLog("_runImpl - Accumulator clamped from {}ms to {}ms.",
                          MsCount(accBefore.count()),
                          MsCount(accumulator.count()));
             }
        }

        // DISPLAY
        DebugLog("_runImpl - DISPLAY start");
        (*aReturnValue) = DoSingleQaoIteration(aContext->_qaoRuntime, QAO_EVENT_MASK_FINALIZE);
        DebugLog("_runImpl - DISPLAY end (status = {})", *aReturnValue);
        if ((*aReturnValue) != 0) {
            return;
        }

        // 'Refill' accumulator
        const auto elapsedTime = frameToFrameStopwatch.restart<TimingDuration>();
        accumulator += elapsedTime;
        DebugLog("_runImpl - Accumulator increased by {}ms (new value: {}ms).",
                 MsCount(elapsedTime.count()),
                 MsCount(accumulator.count()));

        DebugLog("_runImpl - CYCLE end");
    } // End while

    (*aReturnValue) = 0;
    #undef DebugLog
}

} // namespace spempe
} // namespace jbatnozic
