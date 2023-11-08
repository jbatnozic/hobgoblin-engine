
#include <Hobgoblin/Utility/Time_utils.hpp>

#include <Hobgoblin/Common.hpp>
#include <Hobgoblin/Utility/Semaphore.hpp>

#include <cassert>
#include <cstdint>
#include <thread>

#ifdef _WIN32
#  define WIN32_LEAN_AND_MEAN
#  include <windows.h>
#  include <Mmsystem.h>

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

using std::chrono::duration_cast;
using std::chrono::milliseconds;
using std::chrono::microseconds;

void Sleep(milliseconds timeToSleep) {
    std::this_thread::sleep_for(timeToSleep);
}

#ifdef _WIN32
// ****************** WINDOWS ******************

namespace detail {
void CALLBACK SignallingTimerCallback(UINT      timerID,
                                      UINT      /* reserved */,
                                      DWORD_PTR userData,
                                      DWORD_PTR /* reserved */,
                                      DWORD_PTR /* reserved */) {
    static_assert(sizeof(DWORD_PTR) == sizeof(void*));
    auto* sem = reinterpret_cast<Semaphore*>(userData);
    sem->signal();
}
} // namespace detail

void PreciseSleep(milliseconds timeToSleep) {
    if (timeToSleep.count() <= 0) {
        return;
    }

#ifndef NDEBUG
    TIMECAPS tc;
    if (timeGetDevCaps(&tc, sizeof(TIMECAPS)) != TIMERR_NOERROR) {
        assert(false && "Win32 API call 'timeGetDevCaps()' failed");
    }
    assert(tc.wPeriodMin == 1);
#endif // !NDEBUG

    Semaphore sem{0};
    auto mmresult = timeSetEvent(timeToSleep.count(),
                                 1 /* minimal resolution in milliseconds */,
                                 &detail::SignallingTimerCallback,
                                 reinterpret_cast<DWORD_PTR>(&sem),
                                 TIME_CALLBACK_FUNCTION);
    if (mmresult == NULL) {
        throw TracedRuntimeError{"Windows Multimedia timer could not be initialized"};
    }
    sem.wait();
}

#else
// ****************** UNIX ******************

void PreciseSleep(milliseconds timeToSleep) {
    struct timespec ts;
    ts.tv_sec  = static_cast<decltype(ts.tv_sec)>(timeToSleep.count() / 1'000);
    ts.tv_nsec = static_cast<decltype(ts.tv_nsec)>((timeToSleep.count() % 1'000) * 1000);

    while (nanosleep(&ts, &ts) == -1 && errno == EINTR) {}
}

#endif

void SuperPreciseSleep(microseconds timeToSleep) {
    if (timeToSleep.count() <= 0) {
        return;
    }

    static constexpr microseconds PRECISE_SLEEP_GRANULARITY{1000};

    Stopwatch stopwatch{};

    if (timeToSleep > PRECISE_SLEEP_GRANULARITY) {
        PreciseSleep(duration_cast<milliseconds>(timeToSleep - PRECISE_SLEEP_GRANULARITY));
    }

    while (stopwatch.getElapsedTime<std::chrono::microseconds>() < timeToSleep) {
        // Busy wait for the remainder of the time. yield() provides a hint to the
        // implementation to reschedule the execution of threads, allowing other
        // threads to run.
        std::this_thread::yield();
    }
}

} // namespace util
HOBGOBLIN_NAMESPACE_END

#include <Hobgoblin/Private/Pmacro_undef.hpp>