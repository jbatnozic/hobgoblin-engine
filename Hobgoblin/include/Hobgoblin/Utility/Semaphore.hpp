#ifndef UHOBGOBLIN_UTIL_SEMAPHORE_HPP
#define UHOBGOBLIN_UTIL_SEMAPHORE_HPP

#include <mutex>
#include <condition_variable>

#include <Hobgoblin/Private/Pmacro_define.hpp>

HOBGOBLIN_NAMESPACE_START
namespace util {

class Semaphore {
public:

    Semaphore(int initialValue = 0)
        : _val{initialValue}
    {
    }

    void signal() {
        std::unique_lock<decltype(_mutex)> lock(_mutex);
        _val += 1;
        _condition.notify_one();
    }

    void wait() {
        std::unique_lock<decltype(_mutex)> lock(_mutex);
        while (!(_val > 0)) {
            _condition.wait(lock);
        }
        _val -= 1;
    }

    bool tryWait() {
        std::unique_lock<decltype(_mutex)> lock(_mutex);
        if (_val) {
            _val -= 1;
            return true;
        }
        return false;
    }

    int getValue() const {
        return _val;
    }

private:
    std::mutex _mutex;
    std::condition_variable _condition;
    int _val;
};

} // namespace util
HOBGOBLIN_NAMESPACE_END

#include <Hobgoblin/Private/Pmacro_undef.hpp>
#include <Hobgoblin/Private/Short_namespace.hpp>

#endif // !UHOBGOBLIN_UTIL_SEMAPHORE_HPP