// Copyright 2024 Jovan Batnozic. Released under MS-PL licence in Serbia.
// See https://github.com/jbatnozic/Hobgoblin?tab=readme-ov-file#licence

// clang-format off

#ifndef UTIL_PERIODIC_COUNTER_HPP
#define UTIL_PERIODIC_COUNTER_HPP

template <class T>
bool CountPeriodic(T* counterVariable, T period, bool predicate) {
    if (!predicate) {
        (*counterVariable) = 0;
        return false;
    }

    (*counterVariable) += 1;
    if (*counterVariable >= period) {
        (*counterVariable) = 0;
        return true;
    }
    return false;
}

#endif // !UTIL_PERIODIC_COUNTER_HPP

// clang-format on
