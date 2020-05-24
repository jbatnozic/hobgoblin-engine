#ifndef UHOBGOBLIN_UTIL_MATH_HPP
#define UHOBGOBLIN_UTIL_MATH_HPP

#include <SFML/System/Vector2.hpp>
#include <SFML/System/Vector3.hpp>

#include <algorithm>
#include <cmath>
#include <type_traits>

#include <Hobgoblin/Private/Pmacro_define.hpp>

HOBGOBLIN_NAMESPACE_START
namespace util {

using sf::Vector2;
using sf::Vector2i;
using sf::Vector2u;
using sf::Vector2f;
using Vector2d = Vector2<double>;

using sf::Vector3;
using sf::Vector3i;
using Vector3u = Vector3<unsigned>;
using sf::Vector3f;
using Vector3d = Vector3<double>;

template <class T>
T Clamp(const T& value, const T& low, const T& high) {
    return std::min(high, std::max(value, low));
}

template <class T>
T Sqr(T value) {
    return value * value;
}

template <class T>
float EuclideanDist(const Vector2<T>& p1, const Vector2<T>& p2) {
    return std::sqrt(Sqr(p2.x - p1.x) + Sqr(p2.y - p1.y));
}

template <class T>
constexpr typename std::enable_if_t<std::is_integral_v<T>, T> IntegralCeilDiv(T dividend, T divisor) {
    return dividend / divisor - ((-(dividend % divisor)) >> (sizeof(T) * CHAR_BIT - 1));
}

// ///////////////////////////////////////////////////////////////////////////////////////////// //

constexpr double PI = 3.14159265358979323846;

// TODO Rename to Angle with factory methods fromRadians, fromDegrees
// add getters getRadians, getDegrees
struct Radians {
    double rad;

    constexpr Radians() : rad{} {}   
    constexpr explicit Radians(double rad) noexcept : rad{rad} {}

    constexpr Radians(const Radians& other) noexcept = default;
    constexpr Radians& operator=(const Radians& other) noexcept = default;

    // +, -, *, /, +=, -=, *=, /=
    constexpr Radians operator+(Radians other) const noexcept {
        return Radians{SELF.rad + other.rad};
    }

    constexpr Radians operator-(Radians other) const noexcept {
        return Radians{SELF.rad - other.rad};
    }

    constexpr Radians operator*(double scalar) const noexcept {
        return Radians{SELF.rad * scalar};
    }

    friend constexpr Radians operator*(double scalar, Radians angle) noexcept {
        return angle * scalar;
    }

    constexpr Radians operator/(double scalar) const noexcept {
        return Radians{SELF.rad / scalar};
    }

    constexpr Radians& operator+=(Radians other) noexcept {
        rad += other.rad;
        return SELF;
    }

    constexpr Radians& operator-=(Radians other) noexcept {
        rad -= other.rad;
        return SELF;
    }

    constexpr Radians& operator*=(double scalar) noexcept {
        rad *= scalar;
        return SELF;
    }

    constexpr Radians& operator/=(double scalar) noexcept {
        rad /= scalar;
        return SELF;
    }

    // <, <=, >, >=, ==, !=
    constexpr bool operator<(Radians other) const noexcept {
        return rad < other.rad;
    }

    constexpr bool operator<=(Radians other) const noexcept {
        return rad <= other.rad;
    }

    constexpr bool operator>(Radians other) const noexcept {
        return rad > other.rad;
    }

    constexpr bool operator>=(Radians other) const noexcept {
        return rad >= other.rad;
    }

    constexpr bool operator==(Radians other) const noexcept {
        return rad == other.rad;
    }

    constexpr bool operator!=(Radians other) const noexcept {
        return rad != other.rad;
    }

    constexpr bool isEpsilonEqualTo(Radians other, double epsilon) noexcept {
        return (std::abs(rad - other.rad) <= epsilon);
    }

    constexpr Radians shortestDistanceTo(Radians other) const noexcept {
        // TODO
    }
};

//struct Degrees {
//    Real deg;
//    Degrees() = default;
//    constexpr explicit Degrees(Real deg) : deg(deg) {}
//};

} // namespace util
HOBGOBLIN_NAMESPACE_END

#include <Hobgoblin/Private/Pmacro_undef.hpp>
#include <Hobgoblin/Private/Short_namespace.hpp>

#endif // !UHOBGOBLIN_UTIL_MATH_HPP