
#include <Hobgoblin/Common.hpp>
#include <Hobgoblin/Utility/Exceptions.hpp>
#include <SPeMPE/Include/Game_context.hpp>

#include <algorithm>
#include <cassert>

namespace spempe {

namespace {

#define CATCH_EXCEPTIONS_TOP_LEVEL

int DoSingleQaoIteration(hg::QAO_Runtime& runtime, std::int32_t eventFlags) {
    runtime.startStep();
    bool done = false;
    do {
#ifdef CATCH_EXCEPTIONS_TOP_LEVEL
        try {
            runtime.advanceStep(done, eventFlags);
        }
        catch (std::exception& ex) {
            std::cout << "Exception caught: " << ex.what() << '\n';
            return 1;
        }
        catch (...) {
            std::cout << "Unknown exception caught!\n";
            return 2;
        }
#else
        runtime.advanceStep(done, eventFlags);
#endif
    } while (!done);

    return 0;
}

} // namespace

GameContext::RuntimeConfig::RuntimeConfig(hg::PZInteger targetFramerate, hg::PZInteger maxFramesBetweenDisplays)
    : _targetFramerate{targetFramerate}
    , _deltaTime{1.0 / static_cast<double>(targetFramerate)}
    , _maxFramesBetweenDisplays{maxFramesBetweenDisplays}
{
    assert(targetFramerate > 0);
    assert(maxFramesBetweenDisplays > 0);
}

hg::PZInteger GameContext::RuntimeConfig::getTargetFramerate() const noexcept {
    return _targetFramerate;
}

std::chrono::duration<double> GameContext::RuntimeConfig::getDeltaTime() const noexcept {
    return _deltaTime;
}

hg::PZInteger GameContext::RuntimeConfig::getMaxFramesBetweenDisplays() const noexcept {
    return _maxFramesBetweenDisplays;
}

GameContextExtensionData::~GameContextExtensionData() {
}

// ///////////////////////////////////////////////////////////////////////////////////////////// //

GameContext::GameContext(const ResourceConfig& resourceConfig, const RuntimeConfig& runtimeConfig)
    : _resourceConfig{resourceConfig}
    , _runtimeConfig{runtimeConfig}
    , _qaoRuntime{this}
    , _windowManager{_qaoRuntime.nonOwning()}
    , _networkingManager{_qaoRuntime.nonOwning()}
    , _syncObjReg{_networkingManager.getNode()}
    , _extensionData{nullptr}
{
    _networkingManager.getNode().setUserData(this);
}

GameContext::~GameContext() {
    _qaoRuntime.eraseAllNonOwnedObjects();
    _extensionData.reset();
    _postStepActions.clear();
}

void GameContext::configure(Mode mode) {
    _mode = mode;

    // Configure local player index:
    if (_mode == Mode::Server || _mode == Mode::Initial) {
        _localPlayerIndex = PLAYER_INDEX_NONE;
    }
    else if (_mode == Mode::Client) {
        _localPlayerIndex = PLAYER_INDEX_UNKNOWN;
    }
    else {
        _localPlayerIndex = PLAYER_INDEX_LOCAL_PLAYER;
    }

    // Configure NetworkingManager:
    if (hasNetworking()) {
        if (isPrivileged()) {
            _networkingManager.initializeAsServer();
        }
        else {
            _networkingManager.initializeAsClient();
        }
    }
    else {
        // TODO _networkingManager.reset();
    }
    _syncObjReg.setNode(_networkingManager.getNode());

    // Configure WindowManager:
    if (isHeadless()) {
        _windowManager.initAsHeadless();
    }
    else {
        _windowManager.create();
    }
}

bool GameContext::isPrivileged() const {
    return ((static_cast<int>(_mode) & F_PRIVILEGED) != 0);
}

bool GameContext::isHeadless() const {
    return ((static_cast<int>(_mode) & F_HEADLESS) != 0);
}

bool GameContext::hasNetworking() const {
    return ((static_cast<int>(_mode) & F_NETWORKING) != 0);
}

const GameContext::ResourceConfig& GameContext::getResourceConfig() const {
    return _resourceConfig;
}

const GameContext::RuntimeConfig& GameContext::getRuntimeConfig() const {
    return _runtimeConfig;
}

const GameContext::PerformanceInfo& GameContext::getPerformanceInfo() const {
    return _performanceInfo;
}

void GameContext::addPostStepAction(hg::PZInteger delay, std::function<void(GameContext&)> action) {
    if (isPrivileged()) {
        throw hg::util::TracedLogicError{"Cannot add a post step action on host context without the "
                                         "ALLOW_ON_HOST switch."};
    }

    _insertPostStepAction(std::move(action), delay);
}

void GameContext::addPostStepAction(hg::PZInteger delay, GameContext_AllowOnHost_Type,
                                    std::function<void(GameContext&)> action) {
    _insertPostStepAction(std::move(action), delay);
}

int GameContext::getCurrentStepOrdinal() const {
    return _stepOrdinal;
}

void GameContext::setLocalPlayerIndex(int index) {
    _localPlayerIndex = index;
}

int GameContext::getLocalPlayerIndex() const {
    return _localPlayerIndex;
}

void GameContext::setExtensionData(std::unique_ptr<GameContextExtensionData> extData) {
    _extensionData = std::move(extData);
}

GameContextExtensionData* GameContext::getExtensionData() const {
    return _extensionData.get();
}

hg::QAO_Runtime& GameContext::getQaoRuntime() {
    return _qaoRuntime;
}

WindowManager& GameContext::getWindowManager() {
    return _windowManager;
}

NetworkingManager& GameContext::getNetworkingManager() {
    return _networkingManager;
}

SynchronizedObjectRegistry& GameContext::getSyncObjReg() {
    return _syncObjReg;
}

int GameContext::run() {
    int rv;
    _runImpl(this, &rv);
    return rv;
}

void GameContext::stop() {
    if (hasChildContext()) {
        auto childRv = stopChildContext();
        std::cout << "Child context stopped with exit code " << childRv << '\n';
    }

    _quit = true;
}

bool GameContext::hasChildContext() {
    return (_childContext != nullptr);
}

GameContext* GameContext::getChildContext() const {
    return _childContext.get();
}

int GameContext::stopChildContext() {
    assert(hasChildContext());

    _childContext->stop();
    _childContextThread.join();
    _childContext.reset();

    return _childContextReturnValue;
}

void GameContext::runChildContext(std::unique_ptr<GameContext> childContext) {
    assert(!hasChildContext());

    _childContext = std::move(childContext);
    _childContext->_parentContext = this;
    _childContextThread = std::thread{_runImpl, _childContext.get(), &_childContextReturnValue};
}

namespace {

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

} // namespace

void GameContext::_runImpl(hg::not_null<GameContext*> context, hg::not_null<int*> retVal) {
    const TimingDuration deltaTime = context->getRuntimeConfig().getDeltaTime();
    TimingDuration accumulatorTime = deltaTime;
    auto currentTime = steady_clock::now();

    PerformanceInfo perfInfo;
    hg::util::Stopwatch frameToFrameStopwatch;

    while (!context->_quit) {
        perfInfo.frameToFrameTime = frameToFrameStopwatch.restart<microseconds>();

        context->_stepOrdinal += 1;

        auto now = steady_clock::now();
        accumulatorTime += duration_cast<TimingDuration>(now - currentTime);
        currentTime = now;

        for (int i = 0; i < context->getRuntimeConfig().getMaxFramesBetweenDisplays(); i += 1) {
            if (accumulatorTime < deltaTime) {
                break;
            }

            // Run all events except FinalizeFrame (and Draws if headless):
            {
                hg::util::Stopwatch stopwatch;
                if (context->isHeadless()) {
                    (*retVal) = DoSingleQaoIteration(context->_qaoRuntime, QAO_EVENT_MASK_ALL_EXCEPT_DRAW_AND_FINALIZE);
                }
                else {
                    (*retVal) = DoSingleQaoIteration(context->_qaoRuntime, QAO_EVENT_MASK_ALL_EXCEPT_FINALIZE);
                }
                if ((*retVal) != 0) {
                    return;
                }
                perfInfo.updateAndDrawTime = stopwatch.getElapsedTime<microseconds>();
            }

            perfInfo.consecutiveUpdateLoops = i + 1;
            accumulatorTime -= deltaTime;
        } // End for

        // Prevent buildup in accumulator in case the program is not meeting time requirements
        accumulatorTime = std::min(accumulatorTime, deltaTime * 0.5);

        // FinalizeFrame event:
        {
            hg::util::Stopwatch stopwatch;
            (*retVal) = DoSingleQaoIteration(context->_qaoRuntime, QAO_EVENT_MASK_FINALIZE);
            if ((*retVal) != 0) {
                return;
            }
            perfInfo.finalizeTime = stopwatch.getElapsedTime<microseconds>();
        }

        // Do post step actions:
        context->_pollPostStepActions();

        // Record performance data:
        perfInfo.totalTime = perfInfo.updateAndDrawTime + perfInfo.finalizeTime;
        context->_performanceInfo = perfInfo;
    } // End while

    (*retVal) = 0;
}

void GameContext::_insertPostStepAction(std::function<void(GameContext&)> action, hg::PZInteger delay) {
    const auto listIndex = hg::ToSz(delay);
    if (listIndex >= _postStepActions.size()) {
        _postStepActions.resize(listIndex + 1);
    }

    auto& list = _postStepActions[listIndex];

    list.push_back(action);
}

void GameContext::_pollPostStepActions() {
    if (_postStepActions.empty()) {
        return;
    }

    auto& list = _postStepActions.front();

    for (auto& action : list) {
        action(*this);
    }

    _postStepActions.pop_front();
}

}