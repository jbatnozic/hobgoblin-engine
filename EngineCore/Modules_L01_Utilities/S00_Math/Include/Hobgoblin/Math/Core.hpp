// Copyright 2024 Jovan Batnozic. Released under MS-PL licence in Serbia.
// See https://github.com/jbatnozic/Hobgoblin?tab=readme-ov-file#licence

// clang-format off

#ifndef UHOBGOBLIN_UTIL_MATH_HPP
#define UHOBGOBLIN_UTIL_MATH_HPP

#include <Hobgoblin/Math/Vector.hpp>

#include <algorithm>
#include <cmath>
#include <climits>
#include <type_traits>

#include <Hobgoblin/Private/Pmacro_define.hpp>

HOBGOBLIN_NAMESPACE_BEGIN
namespace math {

template <class taArithmetic>
taArithmetic Clamp(const taArithmetic& value, const taArithmetic& low, const taArithmetic& high) {
    return std::min(high, std::max(value, low));
}

template <class taArithmetic>
taArithmetic Sqr(taArithmetic value) {
    return value * value;
}

//! Calculates the Euclidean distance between 2 points (aOrigin and aTarget).
template <class taOriginVector, class taTargetVector>
auto EuclideanDist(const taOriginVector& aOrigin, const taTargetVector& aTarget) -> decltype(aOrigin.x) {
    static_assert(std::is_same<decltype(aOrigin.x), decltype(aOrigin.y)>::value &&
                  std::is_same<decltype(aTarget.x), decltype(aTarget.y)>::value &&
                  std::is_same<decltype(aOrigin.x), decltype(aTarget.y)>::value, 
                  "x and y values of both vectors must all be of the same type");

    const auto xDiff = aTarget.x - aOrigin.x;
    const auto yDiff = aTarget.y - aOrigin.y;
    return std::sqrt(Sqr(xDiff) + Sqr(yDiff));
}

//! Calculates the Euclidean distance between 2 points - (aOriginX, aOriginY) and (aTargetX, aTargetY).
template <class taReal>
taReal EuclideanDist(taReal aOriginX, taReal aOriginY, taReal aTargetX, taReal aTargetY) {
    return EuclideanDist(Vector2<taReal>{aOriginX, aOriginY}, Vector2<taReal>{aTargetX, aTargetY});
}

//! Calculates (divident / divisor), rounding UP.
template <class T>
constexpr typename std::enable_if_t<std::is_integral_v<T>, T> IntegralCeilDiv(T dividend, T divisor) {
    return dividend / divisor - ((-(dividend % divisor)) >> (sizeof(T) * CHAR_BIT - 1));
}

// Solves a quadratic equation
// aX0 will contain the lower solution, aX1 will contain the larger solution (if any)
// Returns false if can't be solved in real numbers
template <class taReal>
bool SolveQuadratic(taReal aA, taReal aB, taReal aC, taReal& aX0, taReal& aX1)
{
    using Real = taReal;

    const Real discriminant = (aB * aB) - (Real{4} * aA * aC);
    if (discriminant < Real{0}) {
        return false;
    }

    if (discriminant == Real{0}) {
        aX0 = aX1 = Real{-0.5} * aB / aA;
    }
    else {
        const Real q = (aB > Real{0}) ? -0.5 * (aB + std::sqrt(discriminant))
                                      : -0.5 * (aB - std::sqrt(discriminant));
        aX0 = q / aA;
        aX1 = aC / q;
    }

    if (aX0 > aX1) {
        std::swap(aX0, aX1);
    }

    return true;
}

} // namespace math
HOBGOBLIN_NAMESPACE_END

#include <Hobgoblin/Private/Pmacro_undef.hpp>
#include <Hobgoblin/Private/Short_namespace.hpp>

#endif // !UHOBGOBLIN_UTIL_MATH_HPP

// clang-format on
