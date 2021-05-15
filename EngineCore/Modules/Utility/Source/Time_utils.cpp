
#include <Hobgoblin/Utility/Time_utils.hpp>

#include <cstdint>
#include <thread>

#ifdef _WIN32
#  define WIN32_LEAN_AND_MEAN
#  include <windows.h>

#else
#  include <time.h>
#  include <errno.h>

#  ifdef __APPLE__
#    include <mach/clock.h>
#    include <mach/mach.h>
#  endif
#endif // _WIN32

#include <Hobgoblin/Private/Pmacro_define.hpp>

HOBGOBLIN_NAMESPACE_BEGIN
namespace util {

using std::chrono::milliseconds;
using std::chrono::microseconds;

void Sleep(milliseconds timeToSleep) {
    std::this_thread::sleep_for(timeToSleep);
}

#ifdef _WIN32
// ****************** WINDOWS ******************

void PreciseSleep(microseconds timeToSleep) {
    ::LARGE_INTEGER ft;
    ft.QuadPart = -static_cast<std::int64_t>(timeToSleep.count()) * 10; // '-' for using relative time

    ::HANDLE timer = ::CreateWaitableTimer(NULL, TRUE, NULL);
    ::SetWaitableTimer(timer, &ft, 0, NULL, NULL, FALSE);
    ::WaitForSingleObject(timer, INFINITE);
    ::CloseHandle(timer);
}

#else
// ****************** UNIX ******************

void PreciseSleep(microseconds timeToSleep) {
    struct timespec ts;
    ts.tv_sec  = static_cast<decltype(ts.tv_sec)>(timeToSleep.count() / 1'000'000);
    ts.tv_nsec = static_cast<decltype(ts.tv_nsec)>(timeToSleep.count() % 1'000'000 * 1000);

    while (nanosleep(&ts, &ts) == -1 && errno == EINTR);
}

#endif

void SuperPreciseSleep(microseconds timeToSleep) {
    if (timeToSleep.count() == 0) {
        return;
    }

    static constexpr microseconds PRECISE_SLEEP_GRANULARITY{1000};

    Stopwatch stopwatch{};

    if (timeToSleep > PRECISE_SLEEP_GRANULARITY) {
        PreciseSleep(timeToSleep - PRECISE_SLEEP_GRANULARITY);
    }

    while (stopwatch.getElapsedTime<std::chrono::microseconds>() < timeToSleep);
}

} // namespace util
HOBGOBLIN_NAMESPACE_END

#include <Hobgoblin/Private/Pmacro_undef.hpp>