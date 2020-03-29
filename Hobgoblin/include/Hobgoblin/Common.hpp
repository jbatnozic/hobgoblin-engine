#ifndef UHOBGOBLIN_COMMON_HPP
#define UHOBGOBLIN_COMMON_HPP

#include <cstddef>

#include <Hobgoblin/Private/Pmacro_define.hpp>

HOBGOBLIN_NAMESPACE_START

// Positive-or-Zero Integer
// TODO In debug mode, use a wrapper class with asserts which check for negative values
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

HOBGOBLIN_NAMESPACE_END

#include <Hobgoblin/Private/Pmacro_undef.hpp>
#include <Hobgoblin/Private/Short_namespace.hpp>

#endif // !UHOBGOBLIN_COMMON_HPP