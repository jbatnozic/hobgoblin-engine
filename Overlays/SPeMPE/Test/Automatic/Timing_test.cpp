// Copyright 2024 Jovan Batnozic. Released under MS-PL licence in Serbia.
// See https://github.com/jbatnozic/Hobgoblin?tab=readme-ov-file#licence

// clang-format off


#include <SPeMPE/SPeMPE.hpp>
#include <Hobgoblin/Logging.hpp>
#include <Hobgoblin/Utility/Time_utils.hpp>

#include <gtest/gtest.h>

#include <chrono>
#include <cstdint>
#include <thread>

namespace jbatnozic {
namespace spempe {

static constexpr auto LOG_ID = "SPeMPE.AutomaticTest";

template <class taDuration>
double SecondsCnt(taDuration aDuration) {
    using namespace std::chrono;
    return static_cast<double>(duration_cast<microseconds>(aDuration).count() / 1'000'000.0);
}

TEST(SPeMPE_TimingTest,
     RunningContextProducesCorrectTickRateWithoutWindowManager_1) {
    hobgoblin::log::SetMinimalLogSeverity(hobgoblin::log::Severity::Info);

    static constexpr auto TEST_DURATION = std::chrono::seconds{2};
    static constexpr auto TICK_RATE     = 60;

    GameContext::RuntimeConfig runtimeConfig{TickRate{TICK_RATE}};
    GameContext context{runtimeConfig};

    hobgoblin::util::Stopwatch stopwatch;
    // Need to add +1 because the delta time comes *between* every two ticks,
    // so in order to have a delay of N delta times, we need N+1 ticks.
    context.runFor(static_cast<int>(SecondsCnt(TEST_DURATION) * TICK_RATE) + 1);
    const auto elapsedTime = stopwatch.getElapsedTime<std::chrono::microseconds>();

    EXPECT_NEAR(
        SecondsCnt(elapsedTime),
        SecondsCnt(TEST_DURATION),
        0.035 // 35ms tolerance
    );

    HG_LOG_INFO(LOG_ID, "elapsedTime={}ms.", SecondsCnt(elapsedTime) * 1000.0);
}

TEST(SPeMPE_TimingTest,
     RunningContextProducesCorrectTickRateWithoutWindowManager_2) {
    hobgoblin::log::SetMinimalLogSeverity(hobgoblin::log::Severity::Info);

    static constexpr auto TEST_DURATION = std::chrono::seconds{2};
    static constexpr auto TICK_RATE     = 60;

    GameContext::RuntimeConfig runtimeConfig{TickRate{TICK_RATE}};
    GameContext context{runtimeConfig};

    auto helper = std::thread{[&]() {
      hobgoblin::util::PreciseSleep(TEST_DURATION);
      context.stop();
    }};
    
    EXPECT_EQ(context.runFor(-1), 0);

    EXPECT_NEAR(
        context.getCurrentIterationOrdinal(),
        // Need to add +1 because the delta time comes *between* every two ticks,
        // so in a timeframe of N delta times, we get N+1 ticks.
        static_cast<int>(SecondsCnt(TEST_DURATION) * TICK_RATE) + 1,
        2 // 2 ticks tolerance
    );

    helper.join();

    HG_LOG_INFO(LOG_ID, "stepOrdinal={}.", context.getCurrentIterationOrdinal());
}

} // spempe
} // jbatnozic

// clang-format on
