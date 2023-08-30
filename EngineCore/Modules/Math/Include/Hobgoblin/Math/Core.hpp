#ifndef UHOBGOBLIN_UTIL_MATH_HPP
#define UHOBGOBLIN_UTIL_MATH_HPP

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

template <class taArithmetic>
taArithmetic EuclideanDist(const Vector2<taArithmetic>& p1, const Vector2<taArithmetic>& p2) {
    return std::sqrt(Sqr(p2.x - p1.x) + Sqr(p2.y - p1.y));
}

template <class taArithmetic>
constexpr typename std::enable_if_t<std::is_integral_v<taArithmetic>, taArithmetic> IntegralCeilDiv(taArithmetic dividend, taArithmetic divisor) {
    return dividend / divisor - ((-(dividend % divisor)) >> (sizeof(taArithmetic) * CHAR_BIT - 1));
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