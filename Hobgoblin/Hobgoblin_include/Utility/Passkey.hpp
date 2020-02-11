#ifndef HOBGOBLIN_UTIL_PASSKEY_HPP
#define HOBGOBLIN_UTIL_PASSKEY_HPP

#include <Hobgoblin_include/Private/pmacro_define.hpp>

HOBGOBLIN_NAMESPACE_START
namespace util {

template <class T>
struct Passkey {
private:
    friend T;
    Passkey() {}
    Passkey(const Passkey&) {}
    Passkey& operator=(const Passkey&) = delete;
};

}
HOBGOBLIN_NAMESPACE_END

#include <Hobgoblin_include/Private/pmacro_undef.hpp>

#endif // !HOBGOBLIN_UTIL_PASSKEY_HPP