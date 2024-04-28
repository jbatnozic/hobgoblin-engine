// Copyright 2024 Jovan Batnozic. Released under MS-PL licence in Serbia.
// See https://github.com/jbatnozic/Hobgoblin?tab=readme-ov-file#licence

#ifndef UHOBGOBLIN_UTIL_MONITOR_HPP
#define UHOBGOBLIN_UTIL_MONITOR_HPP

#include <condition_variable>
#include <mutex>
#include <utility>

#include <Hobgoblin/Private/Pmacro_define.hpp>

HOBGOBLIN_NAMESPACE_BEGIN
namespace util {

//! This class template allows to protect an object (that is otherwise not thread
//! safe) against access from multiple threads at the same time.
//! It is somewhat similar to std::atomic<>, but it allows custom operations to be
//! done atomically, and unlike std::atomic<>, the held object is not required to
//! have a copy constructor.
template <class T, class Mutex = std::mutex>
class Monitor {
public:
    //! Constructs the monitor and the protected object. Provide arguments
    //! as you would for T.
    template <class... Args>
    Monitor(Args&&... aArgs) : _protectedObject{std::forward<Args>(aArgs)...} {}

    //! Synchronize access to the protected object before destruction.
    ~Monitor() {
        // Locking needed to make TSAN errors go away
        std::lock_guard<decltype(_mutex)> lock{_mutex};
    }

    Monitor(const Monitor&) = delete;
    Monitor& operator=(const Monitor&) = delete;
    Monitor(Monitor&&) = delete;
    Monitor& operator=(Monitor&&) = delete;

    //! Calls the provided callable (providing a reference to the protected
    //! object) while holding the internal mutex locked.
    template <class taCallable>
    auto Do(taCallable&& aCallable) {
        std::lock_guard<decltype(_mutex)> lock{_mutex};
        T& ref = _protectedObject;
        return aCallable(ref);
    }

    //! Calls the provided callable (providing a const reference to the protected
    //! object) while holding the internal mutex locked.
    template <class taCallable>
    auto Do(taCallable&& aCallable) const {
        std::lock_guard<decltype(_mutex)> lock{_mutex};
        const T& ref = _protectedObject;
        return aCallable(ref);
    }

    //! Sets the protected object to a new value and returns the old one.
    T Exchange(T aNewValue) {
        std::lock_guard<decltype(_mutex)> lock{_mutex};
        auto tmp = std::move(_protectedObject);
        _protectedObject = std::move(aNewValue);
        return tmp;
    }

    //! Returns a copy of the protected object.
    T Copy() const {
        std::unique_lock<decltype(_mutex)> lock{_mutex};
        T copy{_protectedObject};
        lock.unlock();
        return copy;
    }

private:
    mutable Mutex _mutex;
    T _protectedObject;
};

} // namespace util
HOBGOBLIN_NAMESPACE_END

#include <Hobgoblin/Private/Pmacro_undef.hpp>
#include <Hobgoblin/Private/Short_namespace.hpp>

#endif // !UHOBGOBLIN_UTIL_MONITOR_HPP
