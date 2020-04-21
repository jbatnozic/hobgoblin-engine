
#include <Hobgoblin/QAO.hpp>
#include <Hobgoblin/Utility/Exceptions.hpp>
#include <Hobgoblin/Utility/Stopwatch.hpp>

#include <cassert>
#include <chrono>
#include <iostream>
#include <stdexcept>

#include <new>

#include "Game_context.hpp"

namespace {

#define CATCH_EXCEPTIONS_TOP_LEVEL

int DoSingleQaoIteration(GameContext& ctx, std::int32_t eventFlags) {
    QAO_Runtime& runtime = ctx.qaoRuntime;

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

using Duration = std::chrono::duration<double, std::micro>;
using std::chrono::milliseconds;
using std::chrono::steady_clock;
using std::chrono::duration_cast;

} // namespace

void GameContext::fullStart() {
    switch (_mode) {
    case Mode::Solo:
        windowMgr.create();
        break;

    case Mode::Server:
        windowMgr.create(); // TODO Temp.
        netMgr.convertToServer();
        netMgr.getServer().start(networkingLocalPort, NETWORKING_PASSPHRASE);
        netMgr.getServer().setTimeoutLimit(std::chrono::seconds{5});
        std::cout << "Server started on port " << netMgr.getServer().getLocalPort() << '\n';
        break;

    case Mode::Client:
        windowMgr.create();
        netMgr.getClient().connect(networkingLocalPort, networkingServerIp, 
                                   networkingServerPort, NETWORKING_PASSPHRASE);
        netMgr.getClient().setTimeoutLimit(std::chrono::seconds{5});

        std::cout << "Client connecting to port " << networkingServerPort << '\n';
        break;

    case Mode::GameMaster:
        netMgr.convertToServer();
        netMgr.getServer().start(networkingLocalPort, NETWORKING_PASSPHRASE);
        netMgr.getServer().setTimeoutLimit(std::chrono::seconds{5});
        std::cout << "Server started on port " << netMgr.getServer().getLocalPort() << '\n';
        windowMgr.create();
        break;

    default:
        throw hg::util::TracedLogicError("Invalid start configuration");
    }
}

void GameContext::changeMode(Mode newMode) {
    if (_mode != Mode::Initial) {
        throw hg::util::TracedLogicError("GameContext can change mode only while initial mode");
    }

    if (newMode == Mode::Client || newMode == Mode::Solo || newMode == Mode::GameMaster) {
        _mode = newMode;
    }
    else {
        throw hg::util::TracedLogicError("Invalid mode change");
    }
}

void GameContext::run(GameContext* context, int* retVal) {
    assert(context != nullptr);
    assert(retVal != nullptr);

    const Duration deltaTime = Duration{1'000'000} / 60; // TODO Temp.
    Duration accumulatorTime = deltaTime;
    auto currentTime = steady_clock::now();

    int itercnt = 0;
    int cnt = 0;
    while (!context->quit) {
        auto now = steady_clock::now();
        accumulatorTime += duration_cast<Duration>(now - currentTime);
        currentTime = now;

        for (int i = 0; i < /*FACTOR*/ 2; i += 1) { // TODO
            if (accumulatorTime < deltaTime) {
                break;
            }

            itercnt += 1;

            if (i == /* FACTOR - 1*/ 1) {
                cnt += 1;
            }

            // All steps except Display:
            hg::util::Stopwatch stopwatch{};
            *retVal = DoSingleQaoIteration(*context, ~(1 << static_cast<std::int32_t>(QAO_Event::Render)));
            if (*retVal != 0) {
                return;
            }

            // TODO Temp.
            if (sf::Keyboard::isKeyPressed(sf::Keyboard::T)) {
                auto time = stopwatch.getElapsedTime<std::chrono::microseconds>().count();
                std::cout << "Time for step: " << time << "us\n";
            }

            accumulatorTime -= deltaTime;
        } // End for

        if (accumulatorTime >= deltaTime / 2) {
            accumulatorTime = deltaTime / 2;
        }

        // Display step:
        *retVal = DoSingleQaoIteration(*context, (1 << static_cast<std::int32_t>(QAO_Event::Render)));
        if (*retVal != 0) {
            return;
        }
    } // End while

    std::cout << "double updates = " << cnt << '\n';
    std::cout << "itercnt = " << itercnt << '\n';

    *retVal = 0;
}