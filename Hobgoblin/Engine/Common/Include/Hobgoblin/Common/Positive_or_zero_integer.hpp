#ifndef UHOBGOBLIN_COMMON_POSITIVE_OR_ZERO_INTEGER_HPP
#define UHOBGOBLIN_COMMON_POSITIVE_OR_ZERO_INTEGER_HPP

#include <cstddef>
#include <utility>

#include <Hobgoblin/Private/Pmacro_define.hpp>

HOBGOBLIN_NAMESPACE_START

// Positive-or-Zero Integer
using PZInteger = int;

// std::size_t to PZInteger
inline
PZInteger stopz(std::size_t value) {
    return static_cast<PZInteger>(value);
}

// PZInteger to std::size_t
inline
std::size_t pztos(PZInteger value) {
    return static_cast<std::size_t>(value);
}

template <class T>
PZInteger ToPz(T&& value) {
    return static_cast<PZInteger>(std::forward<T>(value));
}

template <class T>
std::size_t ToSz(T&& value) {
    return static_cast<std::size_t>(std::forward<T>(value));
}

HOBGOBLIN_NAMESPACE_END

#include <Hobgoblin/Private/Pmacro_undef.hpp>
#include <Hobgoblin/Private/Short_namespace.hpp>

#endif // !UHOBGOBLIN_COMMON_POSITIVE_OR_ZERO_INTEGER_HPP