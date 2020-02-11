#ifndef UHOBGOBLIN_UTIL_NO_COPY_NO_MOVE_HPP
#define UHOBGOBLIN_UTIL_NO_COPY_NO_MOVE_HPP

#include <Hobgoblin_include/Private/pmacro_define.hpp>

HOBGOBLIN_NAMESPACE_START
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

#include <Hobgoblin_include/Private/pmacro_undef.hpp>

#endif // !UHOBGOBLIN_UTIL_NO_COPY_NO_MOVE_HPP
