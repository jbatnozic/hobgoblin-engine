#ifndef UHOBGOBLIN_MATH_ANGLE_HPP
#define UHOBGOBLIN_MATH_ANGLE_HPP

#include <cmath>

#include <Hobgoblin/Private/Pmacro_define.hpp>

HOBGOBLIN_NAMESPACE_START
namespace math {

constexpr double PI = 3.14159265358979323846;
constexpr double PI_INVERSE = 1.0 / PI;
constexpr double _180_DEGREES_INVERSE = 1.0 / 180.0;

inline
double DegToRad(double deg) {
    return deg * PI * _180_DEGREES_INVERSE;
}

inline
double RadToDeg(double rad) {
    return rad * 180.0 * PI_INVERSE;
}

class Angle {
public:
    constexpr static Angle zero() {
        return Angle{0.0};
    }

    constexpr static Angle fromRadians(double angleInRadians) {
        return Angle{angleInRadians};
    }

    constexpr static Angle fromDegrees(double angleInDegrees) {
        return Angle{DegToRad(angleInDegrees)};
    }

    constexpr Angle(const Angle& other) noexcept = default;
    constexpr Angle& operator=(const Angle& other) noexcept = default;

    // +, -, *, /, +=, -=, *=, /=

    constexpr Angle operator+(Angle other) const noexcept {
        return Angle{SELF._angleInRadians + other._angleInRadians};
    }

    constexpr Angle operator-(Angle other) const noexcept {
        return Angle{SELF._angleInRadians - other._angleInRadians};
    }

    constexpr Angle operator*(double scalar) const noexcept {
        return Angle{SELF._angleInRadians * scalar};
    }

    friend constexpr Angle operator*(double scalar, Angle angle) noexcept {
        return angle * scalar;
    }

    constexpr Angle operator/(double scalar) const noexcept {
        return Angle{SELF._angleInRadians / scalar};
    }

    constexpr Angle& operator+=(Angle other) noexcept {
        _angleInRadians += other._angleInRadians;
        return SELF;
    }

    constexpr Angle& operator-=(Angle other) noexcept {
        _angleInRadians -= other._angleInRadians;
        return SELF;
    }

    constexpr Angle& operator*=(double scalar) noexcept {
        _angleInRadians *= scalar;
        return SELF;
    }

    constexpr Angle& operator/=(double scalar) noexcept {
        _angleInRadians /= scalar;
        return SELF;
    }

    // <, <=, >, >=, ==, !=

    constexpr bool operator<(Angle other) const noexcept {
        return _angleInRadians < other._angleInRadians;
    }

    constexpr bool operator<=(Angle other) const noexcept {
        return _angleInRadians <= other._angleInRadians;
    }

    constexpr bool operator>(Angle other) const noexcept {
        return _angleInRadians > other._angleInRadians;
    }

    constexpr bool operator>=(Angle other) const noexcept {
        return _angleInRadians >= other._angleInRadians;
    }

    constexpr bool operator==(Angle other) const noexcept {
        return _angleInRadians == other._angleInRadians;
    }

    constexpr bool operator!=(Angle other) const noexcept {
        return _angleInRadians != other._angleInRadians;
    }

    constexpr bool isEpsilonEqualTo(Angle other, double epsilon) noexcept {
        return ((_angleInRadians - other._angleInRadians) <= +epsilon &&
                (_angleInRadians - other._angleInRadians) >= -epsilon);
    }

    constexpr Angle shortestDistanceTo(Angle other) const noexcept {
        // TODO
        return Angle{0.0};
    }

private:
    constexpr explicit Angle(double angleInRadians) noexcept 
        : _angleInRadians{angleInRadians} 
    {
    }

    double _angleInRadians;
};


} // namespace math
HOBGOBLIN_NAMESPACE_END

#include <Hobgoblin/Private/Pmacro_undef.hpp>
#include <Hobgoblin/Private/Short_namespace.hpp>

#endif // !UHOBGOBLIN_MATH_ANGLE_HPP