#ifndef UHOBGOBLIN_UTIL_NO_COPY_NO_MOVE_HPP
#define UHOBGOBLIN_UTIL_NO_COPY_NO_MOVE_HPP

#include <Hobgoblin/Private/Pmacro_define.hpp>

HOBGOBLIN_NAMESPACE_BEGIN
namespace util {

class NonCopyable {
public:
    NonCopyable(const NonCopyable&) = delete;
    NonCopyable& operator=(const NonCopyable&) = delete;

protected:
    NonCopyable() = default;
    ~NonCopyable() = default;
};

class NonMoveable {
public:
    NonMoveable(NonMoveable&&) = delete;
    NonMoveable& operator=(NonMoveable&&) = delete;

protected:
    NonMoveable() = default;
    ~NonMoveable() = default;
};

}
HOBGOBLIN_NAMESPACE_END

#include <Hobgoblin/Private/Pmacro_undef.hpp>
#include <Hobgoblin/Private/Short_namespace.hpp>

#endif // !UHOBGOBLIN_UTIL_NO_COPY_NO_MOVE_HPP
