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

TEST(SPeMPE_TimingTest,
     RunningContextProducesCorrectFramerateWithoutWindowManager_1) {
    hobgoblin::log::SetMinimalLogSeverity(hobgoblin::log::Severity::Info);

    static constexpr auto TEST_DURATION = std::chrono::seconds{2};
    static constexpr auto DESIRED_FRAMERATE = 60.0;

    GameContext::RuntimeConfig runtimeConfig;
    runtimeConfig.deltaTime = std::chrono::duration<double>(1.0 / DESIRED_FRAMERATE);
    GameContext context{runtimeConfig};

    hobgoblin::util::Stopwatch stopwatch;
    context.runFor(static_cast<int>(TEST_DURATION.count() * DESIRED_FRAMERATE));
    const auto elapsedTime = stopwatch.getElapsedTime<std::chrono::microseconds>();

    EXPECT_NEAR(
        elapsedTime.count() / 1'000'000.0,
        (double)TEST_DURATION.count(),
        0.05
    );

    HG_LOG_INFO(LOG_ID, "elapsedTime={}ms.", elapsedTime.count() / 1000.0);
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
        context.getCurrentStepOrdinal(),
        static_cast<int>(TEST_DURATION.count() * DESIRED_FRAMERATE),
        4
    );

    helper.join();

    HG_LOG_INFO(LOG_ID, "stepOrdinal={}.", context.getCurrentStepOrdinal());
}

} // spempe
} // jbatnozic
