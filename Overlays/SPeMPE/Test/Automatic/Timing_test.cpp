#include "Hobgoblin/Logging/Severity.hpp"
#include <SPeMPE/SPeMPE.hpp>
#include <Hobgoblin/Logging.hpp>
#include <Hobgoblin/Utility/Time_utils.hpp>

#include <gtest/gtest.h>

#include <chrono>
#include <cstdint>
#include <thread>

namespace jbatnozic {
namespace spempe {

static constexpr auto LOG_ID = "SPeMPE.Test";

template <class taDuration>
double SecondsCnt(taDuration aDuration) {
    using namespace std::chrono;
    return static_cast<double>(duration_cast<microseconds>(aDuration).count() / 1'000'000.0);
}

TEST(SPeMPE_TimingTest,
     RunningContextProducesCorrectFramerateWithoutWindowManager_1) {
    hobgoblin::log::SetMinimalLogSeverity(hobgoblin::log::Severity::Info);

    static constexpr auto TEST_DURATION = std::chrono::seconds{2};
    static constexpr auto DESIRED_FRAMERATE = 60.0;

    GameContext::RuntimeConfig runtimeConfig;
    runtimeConfig.deltaTime = std::chrono::duration<double>(1.0 / DESIRED_FRAMERATE);
    GameContext context{runtimeConfig};

    hobgoblin::util::Stopwatch stopwatch;
    // Need to add +1 because the delta time comes *between* every two steps,
    // so in order to have a delay of N delta times, we need N+1 steps.
    context.runFor(static_cast<int>(SecondsCnt(TEST_DURATION) * DESIRED_FRAMERATE) + 1);
    const auto elapsedTime = stopwatch.getElapsedTime<std::chrono::microseconds>();

    EXPECT_NEAR(
        SecondsCnt(elapsedTime),
        SecondsCnt(TEST_DURATION),
        0.035 // 35ms tolerance
    );

    HG_LOG_INFO(LOG_ID, "elapsedTime={}ms.", SecondsCnt(elapsedTime) * 1000.0);
}

TEST(SPeMPE_TimingTest,
     RunningContextProducesCorrectFramerateWithoutWindowManager_2) {
    hobgoblin::log::SetMinimalLogSeverity(hobgoblin::log::Severity::Info);

    static constexpr auto TEST_DURATION = std::chrono::seconds{2};
    static constexpr auto DESIRED_FRAMERATE = 60.0;

    GameContext::RuntimeConfig runtimeConfig;
    runtimeConfig.deltaTime = std::chrono::duration<double>(1.0 / DESIRED_FRAMERATE);
    GameContext context{runtimeConfig};

    auto helper = std::thread{[&]() {
      hobgoblin::util::PreciseSleep(TEST_DURATION);
      context.stop();
    }};
    
    EXPECT_EQ(context.runFor(-1), 0);

    EXPECT_NEAR(
        context.getCurrentIterationOrdinal(),
        // Need to add +1 because the delta time comes *between* every two steps,
        // so in a timeframe of N delta times, we get N+1 steps.
        static_cast<int>(SecondsCnt(TEST_DURATION) * DESIRED_FRAMERATE) + 1,
        2 // 2 steps tolerance
    );

    helper.join();

    HG_LOG_INFO(LOG_ID, "stepOrdinal={}.", context.getCurrentIterationOrdinal());
}

} // spempe
} // jbatnozic
