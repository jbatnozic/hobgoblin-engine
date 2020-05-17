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

//class PeriodicCounter {
//public:
//    bool advanceAndTest(int period) {
//        _count += 1;
//        if (_count >= period) {
//            _count = 0;
//            return true;
//        }
//        return false;
//    }
//
//    void reset() {
//        _count = 0;
//    }
//
//private:
//    int _count = 0;
//};

#endif // !UTIL_PERIODIC_COUNTER_HPP
