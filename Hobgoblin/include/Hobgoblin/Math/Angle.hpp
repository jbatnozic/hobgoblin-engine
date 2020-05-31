#ifndef UHOBGOBLIN_MATH_ANGLE_HPP
#define UHOBGOBLIN_MATH_ANGLE_HPP

#include <Hobgoblin/Math/Core.hpp>

#include <cmath>

#include <Hobgoblin/Private/Pmacro_define.hpp>

HOBGOBLIN_NAMESPACE_START
namespace math {

constexpr double PI = 3.14159265358979323846;
constexpr double PI_INVERSE = 1.0 / PI;
constexpr double _180_DEGREES_INVERSE = 1.0 / 180.0;

inline
constexpr double DegToRad(double deg) {
    return deg * PI * _180_DEGREES_INVERSE;
}

inline
constexpr double RadToDeg(double rad) {
    return rad * 180.0 * PI_INVERSE;
}

inline
double ShortestDistanceBetweenAngles(double andle1InRadians, double angle2InRadians) {
    const double diff = std::fmod(angle2InRadians - andle1InRadians + PI, 2.0 * PI) - PI;
    return (diff < -PI) ? (diff + 2.0 * PI) : diff;
}

class Angle;
Angle PointDirection(double xFrom, double yFrom, double xTo, double yTo);

class Angle {
public:
    constexpr static Angle zero() {
        return Angle{0.0};
    }

    constexpr static Angle halfCircle() {
        return Angle{PI};
    }

    constexpr static Angle fromRadians(double angleInRadians) {
        return Angle{angleInRadians};
    }

    constexpr static Angle fromDegrees(double angleInDegrees) {
        return Angle{DegToRad(angleInDegrees)};
    }

    static Angle fromVector(double x, double y) {
        return PointDirection(0.0, 0.0, x, y);
    }

    static Angle fromVector(const Vector2d& vector) {
        return fromVector(vector.x, vector.y);
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
        return Angle{ShortestDistanceBetweenAngles(SELF._angleInRadians, other._angleInRadians)};
    }

    constexpr double asRadians() const noexcept {
        return _angleInRadians;
    }

    constexpr double asDegrees() const noexcept {
        return RadToDeg(_angleInRadians);
    }

    Vector2d asNormalizedVector() const {
        return Vector2d{std::cos(_angleInRadians), std::sin(_angleInRadians)};
    }

private:
    constexpr explicit Angle(double angleInRadians) noexcept 
        : _angleInRadians{angleInRadians} 
    {
    }

    double _angleInRadians;
};

template <class TVecFrom, class TVecTo>
Angle PointDirection(const TVecFrom& vecFrom, const TVecTo& vecTo) {
    const auto xDiff = static_cast<double>(vecTo.x) - static_cast<double>(vecFrom.x);
    const auto yDiff = static_cast<double>(vecTo.y) - static_cast<double>(vecFrom.y);
    return Angle::fromRadians(std::atan2(yDiff, xDiff));
}

inline
Angle PointDirection(double xFrom, double yFrom, double xTo, double yTo) {
    return PointDirection(Vector2d{xFrom, yFrom}, Vector2d{xTo, yTo});
}

} // namespace math
HOBGOBLIN_NAMESPACE_END

#include <Hobgoblin/Private/Pmacro_undef.hpp>
#include <Hobgoblin/Private/Short_namespace.hpp>

#endif // !UHOBGOBLIN_MATH_ANGLE_HPP