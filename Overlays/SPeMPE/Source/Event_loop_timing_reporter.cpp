// Copyright 2024 Jovan Batnozic. Released under MS-PL licence in Serbia.
// See https://github.com/jbatnozic/Hobgoblin?tab=readme-ov-file#licence

// clang-format off


#include <SPeMPE/Utility/Event_loop_timing_reporter.hpp>

#include <Hobgoblin/HGExcept.hpp>
#include <Hobgoblin/Logging.hpp>

#include <cmath>

namespace jbatnozic {
namespace spempe {

namespace {
constexpr auto LOG_ID = "SPeMPE";

template <class taDuration>
double MsCount(taDuration aDuration) {
    static constexpr double NANOSECONDS_PER_MILLISECOND = 1000'000.0;
    namespace chr = std::chrono;
    return chr::duration_cast<chr::nanoseconds>(aDuration).count() / NANOSECONDS_PER_MILLISECOND;
}
} // namespace

EventLoopTimingReporter::EventLoopTimingReporter(hobgoblin::QAO_RuntimeRef aRuntimeRef,
                                                 int aExecutionPriority,
                                                 const Config& aConfig)
    : NonstateObject{aRuntimeRef,
                     SPEMPE_TYPEID_SELF,
                     aExecutionPriority,
                     "EventLoopTimingReporter"}
    , _cycleLength{aConfig.cycleLength}
    , _counter{0}
{
    HG_VALIDATE_ARGUMENT(aConfig.cycleLength > 0);
    _resetCounters();
}

void EventLoopTimingReporter::_eventPreUpdate() {
    const auto& perfInfo = ctx().getPerformanceInfo();

    _totalUpdateTime  += perfInfo.updateTime;
    _totalDrawTime    += perfInfo.drawTime;
    _totalDisplayTime += perfInfo.displayTime;
    if (perfInfo.consecutiveUpdateSteps > 1) {
        _catchUpIterationCount += 1;
    }

    _counter += 1;
    if (_counter < _cycleLength) {
        return;
    }

    const auto elapsedTime = _stopwatch.restart();
    const auto expectedDuration =
        ctx().getRuntimeConfig().tickRate.getDeltaTime() * 
            (ctx().getCurrentIterationOrdinal() - _ordinalAtStartOfCycle);
    const auto percentAccurracy =
        (1.0 - std::abs(MsCount(elapsedTime) - MsCount(expectedDuration)) / MsCount(expectedDuration)) * 100.0;

    HG_LOG_INFO(
        LOG_ID,
        "Performance report for the last {} iteration(s):\n"
        "    TIME SINCE LAST REPORT: {}ms ({}ms expected; {:3.2f}% accurracy)\n"
        "         AVG.  UPDATE TIME: {:3.6f}ms\n"
        "         AVG.    DRAW TIME: {:3.6f}ms\n"
        "         AVG. DISPLAY TIME: {:3.6f}ms\n"
        "       CATCH-UP ITERATIONS: {}",
        _cycleLength,
        MsCount(elapsedTime), MsCount(expectedDuration), percentAccurracy,
        MsCount(_totalUpdateTime) / static_cast<double>(_cycleLength),
        MsCount(_totalDrawTime) / static_cast<double>(_cycleLength),
        MsCount(_totalDisplayTime) / static_cast<double>(_cycleLength),
        _catchUpIterationCount
    );
    
    _resetCounters();
    _counter = 0;
}

void EventLoopTimingReporter::_resetCounters() {
    _ordinalAtStartOfCycle = ctx().getCurrentIterationOrdinal();
    _totalUpdateTime       = std::chrono::microseconds{0};
    _totalDrawTime         = std::chrono::microseconds{0};
    _totalDisplayTime      = std::chrono::microseconds{0};
    _catchUpIterationCount = 0;
}

} // namespace spempe
} // namespace jbatnozic

// clang-format on
