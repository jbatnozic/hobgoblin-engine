#ifndef UHOBGOBLIN_COMMON_HPP
#define UHOBGOBLIN_COMMON_HPP

#include <Hobgoblin/Private/Pmacro_define.hpp>

HOBGOBLIN_NAMESPACE_START

// Positive-or-Zero Integer
// TODO In debug mode, use a wrapper class with asserts which check for negative values
using PZInteger = int;

HOBGOBLIN_NAMESPACE_END

#include <Hobgoblin/Private/Pmacro_undef.hpp>
#include <Hobgoblin/Private/Short_namespace.hpp>

#endif // !UHOBGOBLIN_COMMON_HPP