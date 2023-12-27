#ifndef UHOBGOBLIN_UTIL_TIME_UTILS_HPP
#define UHOBGOBLIN_UTIL_TIME_UTILS_HPP

#include <chrono>

#include <Hobgoblin/Private/Pmacro_define.hpp>

HOBGOBLIN_NAMESPACE_BEGIN
namespace util {

class Stopwatch {
public:
    using ClockType = std::chrono::steady_clock;

    Stopwatch();

    template <class T = std::chrono::milliseconds>
    T restart();

    template <class T = std::chrono::milliseconds>
    T getElapsedTime() const;

private:
    decltype(ClockType::now()) _startTime;
};

inline
Stopwatch::Stopwatch() {
    restart();
}

template <class T>
T Stopwatch::restart() {
    auto now = ClockType::now();
    auto elapsed = now - _startTime;
    _startTime = now;
    return std::chrono::duration_cast<T>(elapsed);
}

template <class T>
T Stopwatch::getElapsedTime() const {
    return std::chrono::duration_cast<T>(ClockType::now() - _startTime);
}

// Max error: ~15ms
void Sleep(std::chrono::milliseconds timeToSleep);

// Max error: ~1ms
void PreciseSleep(std::chrono::milliseconds timeToSleep);

// Max error: usually ~1us; rarely up to 20us
// Note: does busy waiting for up to a millisecond
void SuperPreciseSleep(std::chrono::microseconds timeToSleep);

} // namespace util
HOBGOBLIN_NAMESPACE_END

#include <Hobgoblin/Private/Pmacro_undef.hpp>
#include <Hobgoblin/Private/Short_namespace.hpp>

#endif // !UHOBGOBLIN_UTIL_TIME_UTILS_HPP