// Copyright 2024 Jovan Batnozic. Released under MS-PL licence in Serbia.
// See https://github.com/jbatnozic/Hobgoblin?tab=readme-ov-file#licence

// clang-format off

#ifndef SPEMPE_UTILITY_EVENT_LOOP_TIMING_REPORTER_HPP
#define SPEMPE_UTILITY_EVENT_LOOP_TIMING_REPORTER_HPP

#include <Hobgoblin/Common.hpp>
#include <Hobgoblin/Utility/Time_utils.hpp>

#include <SPeMPE/GameContext/Game_context.hpp>
#include <SPeMPE/GameObjectFramework/Game_object_bases.hpp>

#include <chrono>
#include <cstdint>

namespace jbatnozic {
namespace spempe {

//! Periodically prints a report on the performance and timing of
//! the game's main event loop (that runs when you run a GameContext).
class EventLoopTimingReporter : public NonstateObject {
public:
    struct Config {
        //! The value has to be greater than 0.
        //! - If the value is 1, a report will be logged every iteration;
        //! - If the value is 2, a report will be logged every other iteration;
        //! And so on...
        hobgoblin::PZInteger cycleLength = 300;
    };

    EventLoopTimingReporter(
        hobgoblin::QAO_RuntimeRef aRuntimeRef,
        int aExecutionPriority,
        const Config& aConfig
    );

private:
    hobgoblin::util::Stopwatch _stopwatch;

    hobgoblin::PZInteger _cycleLength;
    hobgoblin::PZInteger _counter;

    std::uint64_t _ordinalAtStartOfCycle;
    std::chrono::microseconds _totalUpdateTime;
    std::chrono::microseconds _totalDrawTime;
    std::chrono::microseconds _totalDisplayTime;
    hobgoblin::PZInteger _catchUpIterationCount;

    void _eventPreUpdate() override;

    void _resetCounters();
};

} // namespace spempe
} // namespace jbatnozic

#endif // !SPEMPE_UTILITY_EVENT_LOOP_TIMING_REPORTER_HPP

// clang-format on
