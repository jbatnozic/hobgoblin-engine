#ifndef UHOBGOBLIN_MATH_ANGLE_HPP
#define UHOBGOBLIN_MATH_ANGLE_HPP

#include <Hobgoblin/Math/Core.hpp>

#include <cmath>
#include <type_traits>

#include <Hobgoblin/Private/Pmacro_define.hpp>

HOBGOBLIN_NAMESPACE_BEGIN
namespace math {

constexpr double PI = 3.14159265358979323846;
constexpr double PI_INVERSE = 1.0 / PI;
constexpr double _180_DEGREES_INVERSE = 1.0 / 180.0;

constexpr float PI_F = 3.14159265358979323846f;
constexpr float PI_INVERSE_F = 1.f / PI_F;
constexpr float _180_DEGREES_INVERSE_F = 1.f / 180.f;

template <class taReal>
constexpr taReal Pi() {
    static_assert(std::is_same<taReal, double>::value || std::is_same<taReal, float>::value);

    if constexpr (std::is_same<taReal, double>::value) {
        return PI;
    }
    else if constexpr (std::is_same<taReal, float>::value) {
        return PI_F;
    }

    return 0;
}

template <class taReal>
constexpr taReal PiInverse() {
    static_assert(std::is_same<taReal, double>::value || std::is_same<taReal, float>::value);

    if constexpr (std::is_same<taReal, double>::value) {
        return PI_INVERSE;
    }
    else if constexpr (std::is_same<taReal, float>::value) {
        return PI_INVERSE_F;
    }

    return 0;
}

template <class taReal>
constexpr taReal HalfCircleDegreesInverse() {
    static_assert(std::is_same<taReal, double>::value || std::is_same<taReal, float>::value);

    if constexpr (std::is_same<taReal, double>::value) {
        return _180_DEGREES_INVERSE;
    }
    else if constexpr (std::is_same<taReal, float>::value) {
        return _180_DEGREES_INVERSE_F;
    }

    return 0;
}

template <class taReal>
constexpr taReal DegToRad(taReal deg) {
    return deg * Pi<taReal>() * HalfCircleDegreesInverse<taReal>();
}

template <class taReal>
constexpr taReal RadToDeg(taReal rad) {
    return rad * static_cast<taReal>(180) * PiInverse<taReal>();
}

template <class taReal>
taReal ShortestDistanceBetweenAngles(taReal andle1InRadians, taReal angle2InRadians) {
    const taReal pi = Pi<taReal>();
    const taReal diff = std::fmod<taReal>(angle2InRadians - andle1InRadians + pi, taReal{2} * pi) - pi;
    return (diff < -pi) ? (diff + taReal{2} * pi) : diff;
}

template <class taReal>
class Angle {
public:
    using Real = taReal;

    constexpr static Angle zero() {
        return Angle{static_cast<taReal>(0)};
    }

    constexpr static Angle halfCircle() {
        return Angle{Pi<taReal>()};
    }

    constexpr static Angle fromRadians(Real angleInRadians) {
        return Angle{angleInRadians};
    }

    constexpr static Angle fromDegrees(Real angleInDegrees) {
        return Angle{DegToRad(angleInDegrees)};
    }

    static Angle fromVector(Real x, Real y); // Implementation @ end of file

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

    constexpr Angle operator*(Real scalar) const noexcept {
        return Angle{SELF._angleInRadians * scalar};
    }

    friend constexpr Angle operator*(Real scalar, Angle angle) noexcept {
        return angle * scalar;
    }

    constexpr Angle operator/(Real scalar) const noexcept {
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

    constexpr Angle& operator*=(Real scalar) noexcept {
        _angleInRadians *= scalar;
        return SELF;
    }

    constexpr Angle& operator/=(Real scalar) noexcept {
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

    constexpr bool isEpsilonEqualTo(Angle other, Real epsilon) noexcept {
        return ((_angleInRadians - other._angleInRadians) <= +epsilon &&
                (_angleInRadians - other._angleInRadians) >= -epsilon);
    }

    constexpr Angle shortestDistanceTo(Angle other) const noexcept {
        return Angle{ShortestDistanceBetweenAngles(SELF._angleInRadians, other._angleInRadians)};
    }

    constexpr Real asRadians() const noexcept {
        return _angleInRadians;
    }

    constexpr Real asDegrees() const noexcept {
        return RadToDeg(_angleInRadians);
    }

    Vector2<Real> asNormalizedVector() const {
        return Vector2<Real>{std::cos(_angleInRadians), std::sin(_angleInRadians)};
    }

private:
    constexpr explicit Angle(Real angleInRadians) noexcept
        : _angleInRadians{angleInRadians} 
    {
    }

    Real _angleInRadians;
};

template <class taReal, class taVecFrom, class taVecTo>
Angle<taReal> PointDirection(const taVecFrom& vecFrom, const taVecTo& vecTo) {
    const auto xDiff = static_cast<taReal>(vecTo.x) - static_cast<taReal>(vecFrom.x);
    const auto yDiff = static_cast<taReal>(vecTo.y) - static_cast<taReal>(vecFrom.y);
    return Angle<taReal>::fromRadians(std::atan2(yDiff, xDiff));
}

template <class taReal>
Angle<taReal> PointDirection(taReal xFrom, taReal yFrom, taReal xTo, taReal yTo) {
    return PointDirection<taReal>(Vector2<taReal>{xFrom, yFrom}, Vector2<taReal>{xTo, yTo});
}

template <class taReal>
Angle<taReal> Angle<taReal>::fromVector(taReal x, taReal y) {
    return PointDirection<taReal>(0, 0, x, y);
}

using AngleF = Angle<float>;
using AngleD = Angle<double>;

} // namespace math
HOBGOBLIN_NAMESPACE_END

#include <Hobgoblin/Private/Pmacro_undef.hpp>
#include <Hobgoblin/Private/Short_namespace.hpp>

#endif // !UHOBGOBLIN_MATH_ANGLE_HPP