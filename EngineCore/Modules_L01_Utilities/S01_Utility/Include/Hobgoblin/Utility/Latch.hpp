// Copyright 2024 Jovan Batnozic. Released under MS-PL licence in Serbia.
// See https://github.com/jbatnozic/Hobgoblin?tab=readme-ov-file#licence

#ifndef UHOBGOBLIN_UTIL_LATCH_HPP
#define UHOBGOBLIN_UTIL_LATCH_HPP

#include <condition_variable>
#include <mutex>

#include <Hobgoblin/Private/Pmacro_define.hpp>

HOBGOBLIN_NAMESPACE_BEGIN
namespace util {

class Latch {
public:
    static constexpr bool OPEN   = true;
    static constexpr bool CLOSED = false;

    Latch(bool aInitialValue = CLOSED)
        : _val{aInitialValue} {}

    void open() {
        std::unique_lock<decltype(_mutex)> lock(_mutex);
        _val = OPEN;
        _condition.notify_all();
    }

    void close() {
        std::unique_lock<decltype(_mutex)> lock(_mutex);
        _val = CLOSED;
    }

    void wait() {
        std::unique_lock<decltype(_mutex)> lock(_mutex);
        while (_val == CLOSED) {
            _condition.wait(lock);
        }
    }

    bool tryWait() {
        std::unique_lock<decltype(_mutex)> lock(_mutex);
        if (_val == OPEN) {
            return true;
        }
        return false;
    }

    bool getValue() const {
        bool result;
        {
            std::unique_lock<decltype(_mutex)> lock(_mutex);
            result = _val;
        }
        return result;
    }

private:
    mutable std::mutex      _mutex;
    std::condition_variable _condition;
    bool                    _val;
};

} // namespace util
HOBGOBLIN_NAMESPACE_END

#include <Hobgoblin/Private/Pmacro_undef.hpp>
#include <Hobgoblin/Private/Short_namespace.hpp>

#endif // !UHOBGOBLIN_UTIL_LATCH_HPP
