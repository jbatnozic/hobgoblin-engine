#ifndef UHOBGOBLIN_UTIL_STOPWATCH_HPP
#define UHOBGOBLIN_UTIL_STOPWATCH_HPP

#include <chrono>

#include <Hobgoblin/Private/Pmacro_define.hpp>

HOBGOBLIN_NAMESPACE_START
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

} // namespace util
HOBGOBLIN_NAMESPACE_END

#include <Hobgoblin/Private/Pmacro_undef.hpp>
#include <Hobgoblin/Private/Short_namespace.hpp>

#endif // !UHOBGOBLIN_UTIL_STOPWATCH_HPP