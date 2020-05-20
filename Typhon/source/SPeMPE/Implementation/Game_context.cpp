
#include <SPeMPE/Include/Game_context.hpp>

namespace spempe {

namespace {

//#define CATCH_EXCEPTIONS_TOP_LEVEL

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

void GameContext::configure(Mode mode) {
    // TODO I never did like this method... (it does too much)
    _mode = mode;

    switch (mode) {
    case Mode::Server:
        _localPlayerIndex = PLAYER_INDEX_NONE;
        _windowManager.create(); // TODO Temp.
        _networkingManager.initializeAsServer();
        //_networkingManager.getServer().start(_networkConfig.localPort, _networkConfig.passphrase);
        //_networkingManager.getServer().resize(_networkConfig.clientCount);
        //_networkingManager.getServer().setTimeoutLimit(std::chrono::seconds{5}); // TODO Magic number
        _syncObjReg.setNode(_networkingManager.getNode());
        //std::cout << "Server started on port " << _networkingManager.getServer().getLocalPort() << '\n';

        //{
        //    std::cout << "Generating terrain...\n";
        //    hg::util::Stopwatch stopwatch;
        //    envMgr.generate(100, 100, 32.f);
        //    std::cout << "DONE! Terrain generated (took " << stopwatch.getElapsedTime().count() << "ms)\n";
        //}
        break;

    case Mode::Client:
        _localPlayerIndex = PLAYER_INDEX_UNKNOWN;
        _windowManager.create();
        _networkingManager.initializeAsClient();
        //_networkingManager.getClient().connect(_networkConfig.localPort, _networkConfig.serverIp,
        //                                       _networkConfig.serverPort, _networkConfig.passphrase);
        //_networkingManager.getClient().setTimeoutLimit(std::chrono::seconds{5});
        _syncObjReg.setNode(_networkingManager.getNode());
        break;

    case Mode::Solo:
        _localPlayerIndex = 0;
        _windowManager.create();
        break;

    case Mode::GameMaster:
        _localPlayerIndex = 0;
        _windowManager.create();
        _networkingManager.initializeAsServer();
        //_networkingManager.getServer().start(_networkConfig.localPort, _networkConfig.passphrase);
        //_networkingManager.getServer().resize(_networkConfig.clientCount);
        //_networkingManager.getServer().setTimeoutLimit(std::chrono::seconds{5});
        _syncObjReg.setNode(_networkingManager.getNode());
        //std::cout << "Server started on port " << _networkingManager.getServer().getLocalPort() << '\n';
        break;

    default:
        throw hg::util::TracedLogicError("Invalid configuration");
    }
}

GameContext::GameContext(const ResourceConfig& resourceConfig, const RuntimeConfig& runtimeConfig)
    : _resourceConfig{resourceConfig}
    , _runtimeConfig{runtimeConfig}
    , _qaoRuntime{this}
    , _windowManager{_qaoRuntime.nonOwning()}
    , _networkingManager{_qaoRuntime.nonOwning()}
    , _syncObjReg{_networkingManager.getNode()}
    , _extensionData{nullptr}
{
    // TODO _networkingManager.setUserData(this);
    _networkingManager.getNode().setUserData(this);
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

int GameContext::run() {
    int rv;
    runImpl(this, &rv);
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
    _childContextThread = std::thread{runImpl, _childContext.get(), &_childContextReturnValue};
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

constexpr std::int32_t QAO_EVENT_MASK_DISPLAY = ToEventMask(QAO_Event::Render);

constexpr std::int32_t QAO_EVENT_MASK_ALL_EXCEPT_DRAW = QAO_ALL_EVENT_FLAGS & ~QAO_EVENT_MASK_ALL_DRAWS;

constexpr std::int32_t QAO_EVENT_MASK_ALL_EXCEPT_DISPLAY = QAO_ALL_EVENT_FLAGS & ~QAO_EVENT_MASK_DISPLAY;

constexpr std::int32_t QAO_EVENT_MASK_ALL_EXCEPT_DRAW_AND_DISPLAY = QAO_EVENT_MASK_ALL_EXCEPT_DRAW
                                                                  & QAO_EVENT_MASK_ALL_EXCEPT_DISPLAY;

using Duration = std::chrono::duration<double, std::micro>;
using std::chrono::milliseconds;
using std::chrono::steady_clock;
using std::chrono::duration_cast;

} // namespace

void GameContext::runImpl(GameContext* context, int* retVal) {
    assert(context != nullptr);
    assert(retVal != nullptr);

    const Duration deltaTime{1000.0 / 60.0}; // context->getDeltaTime(); TODO
    Duration accumulatorTime = deltaTime;
    auto currentTime = steady_clock::now();

    while (!context->_quit) {
        auto now = steady_clock::now();
        accumulatorTime += duration_cast<Duration>(now - currentTime);
        currentTime = now;

        for (int i = 0; i < /*FACTOR*/ 2; i += 1) { // TODO
            if (accumulatorTime < deltaTime) {
                break;
            }

            // All steps except Display:
            if (context->isHeadless()) {
                (*retVal) = DoSingleQaoIteration(context->_qaoRuntime, QAO_EVENT_MASK_ALL_EXCEPT_DRAW_AND_DISPLAY);
            }
            else {
                (*retVal) = DoSingleQaoIteration(context->_qaoRuntime, QAO_EVENT_MASK_ALL_EXCEPT_DISPLAY);
            }
            if (*retVal != 0) {
                return;
            }

            accumulatorTime -= deltaTime;
        } // End for

        // TODO Refactor magic
        if (accumulatorTime >= deltaTime / 2) {
            accumulatorTime = deltaTime / 2;
        }

        // Display step:
        (*retVal) = DoSingleQaoIteration(context->_qaoRuntime, QAO_EVENT_MASK_DISPLAY);
        if (*retVal != 0) {
            return;
        }
    } // End while

    (*retVal) = 0;
}

}