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

//! At the end of the scope where this macro is placed, logs a message, as if a `HG_LOG_*` macro
//! had been used.
//!
//! \param _severity_ severity for the message being logged. One of: HPDEB, DEBUG, INFO, WARN, ERROR,
//!                   FATAL (without quotes and all uppercase).
//! \param _log_id_ log ID, same as for any `HG_LOG_*` macro.
//! \param ... the first parameter of the variadic parameter pack must be the format string (as for
//!            the fmt library), the rest are optional parameters to replace placeholders (such as {})
//!            from the format string. Local variables available in the scope where this macro is placed
//!            are available here, as well as a special identifier `elapsed_time_ms` which is of type
//!            `double` and its value is the number of milliseconds that have elapsed between the line
//!            of code where this macro is placed and its invocation at the end of the current scope.
//!    
//! Example of usage:
//! {
//!     const char* resourceName = GetResourceName();
//!     {
//!         HG_LOG_WITH_SCOPED_STOPWATCH_MS(
//!             INFO, 
//!             "MyGame", 
//!             "Loading of resource {} took {}ms.", resourceName, elapsed_time_ms);
//!         LoadResource(resourceName);
//!     }
//!     // ...
//! }
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
