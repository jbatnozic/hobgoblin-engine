// Copyright 2024 Jovan Batnozic. Released under MS-PL licence in Serbia.
// See https://github.com/jbatnozic/Hobgoblin?tab=readme-ov-file#licence

// clang-format off


#define HOBGOBLIN_SHORT_NAMESPACE
#include <Hobgoblin/Utility/Time_utils.hpp>

#include <chrono>
#include <iostream>

using hg::util::Sleep;
using hg::util::PreciseSleep;
using hg::util::SuperPreciseSleep;
using hg::util::Stopwatch;

using std::chrono::milliseconds;
using std::chrono::microseconds;

int main() {
    SuperPreciseSleep(microseconds{0});

    microseconds arr[30];
    
    for (int i = 0; i < 30; i += 1) {
        Stopwatch stopwatch{};
        SuperPreciseSleep(microseconds(i * 100));
        arr[i] = stopwatch.getElapsedTime<std::chrono::microseconds>();
    }

    for (auto time : arr) {
        std::cout << "Slept for " << time.count() << "us\n";
    }

    return 0;
}

// clang-format on
