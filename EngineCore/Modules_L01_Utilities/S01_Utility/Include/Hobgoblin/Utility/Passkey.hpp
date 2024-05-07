// Copyright 2024 Jovan Batnozic. Released under MS-PL licence in Serbia.
// See https://github.com/jbatnozic/Hobgoblin?tab=readme-ov-file#licence

// clang-format off

#ifndef HOBGOBLIN_UTIL_PASSKEY_HPP
#define HOBGOBLIN_UTIL_PASSKEY_HPP

#include <Hobgoblin/Private/Pmacro_define.hpp>

HOBGOBLIN_NAMESPACE_BEGIN
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

#include <Hobgoblin/Private/Pmacro_undef.hpp>
#include <Hobgoblin/Private/Short_namespace.hpp>

#endif // !HOBGOBLIN_UTIL_PASSKEY_HPP

// clang-format on
