// Copyright 2024 Jovan Batnozic. Released under MS-PL licence in Serbia.
// See https://github.com/jbatnozic/Hobgoblin?tab=readme-ov-file#licence

#ifndef UHOBGOBLIN_UTIL_INVOKING_SCOPED_STOPWATCH_HPP
#define UHOBGOBLIN_UTIL_INVOKING_SCOPED_STOPWATCH_HPP

#include <Hobgoblin/Logging.hpp>

#include <Hobgoblin/Utility/Time_utils.hpp>

#include <chrono>

#include <Hobgoblin/Private/Pmacro_define.hpp>

HOBGOBLIN_NAMESPACE_BEGIN
namespace util {

//! An instance of this class will invoke a callable of type `taCallable` upon destruction, while
//! providing a single argument: time elapsed since construction as `taDuration`.
template <class taCallable, class taDuration = std::chrono::milliseconds>
class InvokingScopedStopwatch {
public:
    InvokingScopedStopwatch(taCallable&& aContext)
        : _context(std::move(aContext)) {}

    ~InvokingScopedStopwatch() {
        const auto duration_ms = _stopwatch.getElapsedTime<taDuration>();
        _context(duration_ms);
    }

private:
    Stopwatch  _stopwatch;
    taCallable _context;
};

//!
//!
//!  TODO(description)
//!
//!
#define HG_LOG_WITH_SCOPED_STOPWATCH_MS(_severity_, _log_id_, ...) \
    UHOBGOBLIN_LOG_WITH_SCOPED_STOPWATCH_MS_MIDDLE(__LINE__, _severity_, _log_id_, __VA_ARGS__)

// ===== Implementation detail below =====

#define UHOBGOBLIN_LOG_WITH_SCOPED_STOPWATCH_MS_IMPL(_line_, _logger_, _log_id_, ...)  \
    auto UHOBGOBLIN_logWithScopedStopwatchMsCallable##_line_ =                         \
        [&](std::chrono::nanoseconds UHOBGOBLIN_elapsedTimeNs) -> void {               \
        const double elapsed_time_ms = UHOBGOBLIN_elapsedTimeNs.count() / 1'000'000.0; \
        static_cast<void>(elapsed_time_ms);                                            \
        _logger_(_log_id_, __VA_ARGS__);                                               \
    };                                                                                 \
    ::jbatnozic::hobgoblin::util::InvokingScopedStopwatch<                             \
        decltype(UHOBGOBLIN_logWithScopedStopwatchMsCallable##_line_),                 \
        std::chrono::nanoseconds>                                                      \
        UHOBGOBLIN_invokingScopedStopwatch##_line_ {                                   \
        std::move(UHOBGOBLIN_logWithScopedStopwatchMsCallable##_line_)                 \
    }

#define UHOBGOBLIN_LOG_WITH_SCOPED_STOPWATCH_MS_MIDDLE(_line_, _severity_, _log_id_, ...) \
    UHOBGOBLIN_LOG_WITH_SCOPED_STOPWATCH_MS_IMPL(_line_, HG_LOG_##_severity_, _log_id_, __VA_ARGS__)

} // namespace util
HOBGOBLIN_NAMESPACE_END

#include <Hobgoblin/Private/Pmacro_undef.hpp>
#include <Hobgoblin/Private/Short_namespace.hpp>

#endif // !UHOBGOBLIN_UTIL_INVOKING_SCOPED_STOPWATCH_HPP
