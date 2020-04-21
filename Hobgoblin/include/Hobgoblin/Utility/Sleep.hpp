#ifndef UHOBGOBLIN_UTIL_SLEEP_HPP
#define UHOBGOBLIN_UTIL_SLEEP_HPP

#include <chrono>

#include <Hobgoblin/Private/Pmacro_define.hpp>

HOBGOBLIN_NAMESPACE_START
namespace util {

// Max error: ~15ms
void Sleep(std::chrono::milliseconds timeToSleep);

// Max error: ~1ms
void PreciseSleep(std::chrono::microseconds timeToSleep);

// Max error: usually ~1us; rarely up to 20us
// Note: does busy waiting for up to a millisecond
void SuperPreciseSleep(std::chrono::microseconds timeToSleep);

} // namespace util
HOBGOBLIN_NAMESPACE_END

#include <Hobgoblin/Private/Pmacro_undef.hpp>
#include <Hobgoblin/Private/Short_namespace.hpp>

#endif // !UHOBGOBLIN_UTIL_SLEEP_HPP