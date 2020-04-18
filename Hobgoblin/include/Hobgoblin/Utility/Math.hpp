#ifndef UHOBGOBLIN_UTIL_MATH_HPP
#define UHOBGOBLIN_UTIL_MATH_HPP

#include <algorithm>

#include <Hobgoblin/Private/Pmacro_define.hpp>

HOBGOBLIN_NAMESPACE_START
namespace util {

template <class T>
const T& Clamp(const T& value, const T& low, const T& high) {
    return std::min(high, std::max(value, low));
}

template <class T>
T Sqr(T value) {
    return value * value;
}

} // namespace util
HOBGOBLIN_NAMESPACE_END

#include <Hobgoblin/Private/Pmacro_undef.hpp>
#include <Hobgoblin/Private/Short_namespace.hpp>

#endif // !UHOBGOBLIN_UTIL_MATH_HPP