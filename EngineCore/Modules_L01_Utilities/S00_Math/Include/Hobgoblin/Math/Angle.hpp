// Copyright 2024 Jovan Batnozic. Released under MS-PL licence in Serbia.
// See https://github.com/jbatnozic/Hobgoblin?tab=readme-ov-file#licence

// clang-format off

#ifndef UHOBGOBLIN_MATH_ANGLE_HPP
#define UHOBGOBLIN_MATH_ANGLE_HPP

#include <Hobgoblin/Math/Vector.hpp>

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

//! Returns Pi as either float or double.
template <class taReal>
constexpr taReal Pi() {
    static_assert(std::is_same<taReal, double>::value || std::is_same<taReal, float>::value);

    if constexpr (std::is_same<taReal, double>::value) {
        return PI;
    }
    else if constexpr (std::is_same<taReal, float>::value) {
        return PI_F;
    }

    return 0.f;
}

//! Returns 1.0/Pi as either float or double.
template <class taReal>
constexpr taReal PiInverse() {
    static_assert(std::is_same<taReal, double>::value || std::is_same<taReal, float>::value);

    if constexpr (std::is_same<taReal, double>::value) {
        return PI_INVERSE;
    }
    else if constexpr (std::is_same<taReal, float>::value) {
        return PI_INVERSE_F;
    }

    return 0.f;
}

//! Returns 1.0/180.0 as either float or double.
template <class taReal>
constexpr taReal HalfCircleDegreesInverse() {
    static_assert(std::is_same<taReal, double>::value || std::is_same<taReal, float>::value);

    if constexpr (std::is_same<taReal, double>::value) {
        return _180_DEGREES_INVERSE;
    }
    else if constexpr (std::is_same<taReal, float>::value) {
        return _180_DEGREES_INVERSE_F;
    }

    return 0.f;
}

template <class taReal>
constexpr taReal DegToRad(taReal deg) {
    return deg * Pi<taReal>() * HalfCircleDegreesInverse<taReal>();
}

template <class taReal>
constexpr taReal RadToDeg(taReal rad) {
    return rad * static_cast<taReal>(180.f) * PiInverse<taReal>();
}

template <class taReal>
taReal ShortestDistanceBetweenAngles(taReal andle1InRadians, taReal angle2InRadians) {
    const auto two = static_cast<taReal>(2.f);
    const auto pi = Pi<taReal>();
    const taReal diff = std::fmod<taReal>(angle2InRadians - andle1InRadians + pi, two * pi) - pi;
    return (diff < -pi) ? (diff + two * pi) : diff;
}

//! Note: The underlying representation of the angle is always in radians.
template <class taReal>
class Angle {
public:
    using Real = taReal;

    //! Default constructor. Contructs a 0-degree angle.
    constexpr Angle() : _angleInRadians{static_cast<taReal>(0.f)} {}

    constexpr static Angle zero() {
        return Angle{static_cast<taReal>(0.f)};
    }

    //! Half circle == 180 degrees == pi radians.
    constexpr static Angle halfCircle() {
        return Angle{Pi<taReal>()};
    }

    //! Full circle == 360 degrees == 2*pi radians.
    constexpr static Angle fullCircle() {
        return Angle{Pi<taReal>() * static_cast<taReal>(2.f)};
    }

    constexpr static Angle fromRadians(Real angleInRadians) {
        return Angle{angleInRadians};
    }

    constexpr static Angle fromRad(Real angleInRadians) {
        return fromRadians(angleInRadians);
    }

    constexpr static Angle fromDegrees(Real angleInDegrees) {
        return Angle{DegToRad(angleInDegrees)};
    }

    constexpr static Angle fromDeg(Real angleInDegrees) {
        return fromDegrees(angleInDegrees);
    }

    static Angle fromVector(Real x, Real y); // Implementation @ end of file

    static Angle fromVector(const Vector2d& vector) {
        return fromVector(vector.x, vector.y);
    }

    ~Angle() = default;

    constexpr Angle(const Angle& other) noexcept = default;
    constexpr Angle& operator=(const Angle& other) noexcept = default;
    constexpr Angle(Angle&& other) noexcept = default;
    constexpr Angle& operator=(Angle&& other) noexcept = default;

    // +, -, *, /, +=, -=, *=, /=

    constexpr Angle operator+(Angle other) const noexcept {
        return Angle{SELF._angleInRadians + other._angleInRadians};
    }

    constexpr Angle operator+() const noexcept {
        return Angle{+(SELF._angleInRadians)};
    }

    constexpr Angle operator-(Angle other) const noexcept {
        return Angle{SELF._angleInRadians - other._angleInRadians};
    }

    constexpr Angle operator-() const noexcept {
        return Angle{-(SELF._angleInRadians)};
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

    constexpr Real operator/(Angle other) const noexcept {
        return SELF._angleInRadians / other._angleInRadians;
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

    //! Note: Epsilon is expressed in radians.
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

    constexpr Real asRad() const noexcept {
        return _angleInRadians;
    }

    constexpr Real asDegrees() const noexcept {
        return RadToDeg(_angleInRadians);
    }

    constexpr Real asDeg() const noexcept {
        return asDegrees();
    }

    Vector2<Real> asNormalizedVector() const {
        return Vector2<Real>{std::cos(_angleInRadians), std::sin(_angleInRadians)};
    }

    Angle normalize(Angle aRangeLow = zero(), Angle aRangeHigh = fullCircle()) {
        const auto offsetValue = (SELF       - aRangeLow)._angleInRadians;
        const auto width       = (aRangeHigh - aRangeLow)._angleInRadians;

        using std::floor;
        return Angle::fromRad(offsetValue - (floor(offsetValue / width) * width)) + aRangeLow;
    }

    constexpr Real sin() const {
        return std::sin(_angleInRadians);
    }

    constexpr Real cos() const {
        return std::cos(_angleInRadians);
    }

    constexpr Real tan() const {
        return std::tan(_angleInRadians);
    }

private:
    constexpr explicit Angle(Real angleInRadians) noexcept
        : _angleInRadians{angleInRadians} 
    {
    }

    Real _angleInRadians;
};

//! Angle<> using float as the underlying data type.
using AngleF = Angle<float>;

//! Angle<> using double as the underlying data type.
using AngleD = Angle<double>;

//! Returns the direction from position marked by aOrigin to the position marked by aTarget, as if the
//! centre of the coordinate system (marked by O in the below diagram) were in the aOrigin position.
//!          +90
//!           |
//!           |
//! +180 ---- O ---- 0
//!           |
//!           |
//!         +270
template <class taOriginVector, class taTargetVector>
auto PointDirection(const taOriginVector& aOrigin, const taTargetVector& aTarget) -> Angle<decltype(aOrigin.x)> {
    static_assert(std::is_same<decltype(aOrigin.x), decltype(aOrigin.y)>::value &&
                  std::is_same<decltype(aTarget.x), decltype(aTarget.y)>::value &&
                  std::is_same<decltype(aOrigin.x), decltype(aTarget.y)>::value, 
                  "x and y values of both vectors must all be of the same type");

    using Real = decltype(aOrigin.x);
    const auto xDiff = aTarget.x - aOrigin.x;
    const auto yDiff = aTarget.y - aOrigin.y;
    return Angle<Real>::fromRadians(static_cast<Real>(std::atan2(-yDiff, xDiff))).normalize();
}

//! Returns the direction from position marked by (aOriginX, aOriginY) to the position marked by
//! (aTargetX, aTargetY), as if the centre of the coordinate system (marked by O in the below
//! diagram) were in the (aOriginX, aOriginY) position.
//!          +90
//!           |
//!           |
//! +180 ---- O ---- 0
//!           |
//!           |
//!         +270
template <class taReal>
Angle<taReal> PointDirection(taReal aOriginX, taReal aOriginY, taReal aTargetX, taReal aTargetY) {
    return PointDirection(Vector2<taReal>{aOriginX, aOriginY}, Vector2<taReal>{aTargetX, aTargetY});
}

template <class taReal>
Angle<taReal> Angle<taReal>::fromVector(taReal x, taReal y) {
    return PointDirection<taReal>(0, 0, x, y);
}

//! User-defined literals for Angle<double>.
namespace angle_literals {
inline
AngleD operator "" _rad(long double aAngleInRadians) {
    return AngleD::fromRadians(static_cast<double>(aAngleInRadians));
}

inline
AngleD operator "" _deg(long double aAngleInDegrees) {
    return AngleD::fromDegrees(static_cast<double>(aAngleInDegrees));
}
} // namespace angle_literals

  //! User-defined literals for Angle<float>.
namespace angle_literals_f {
inline
AngleF operator "" _rad(long double aAngleInRadians) {
    return AngleF::fromRadians(static_cast<float>(aAngleInRadians));
}

inline
AngleF operator "" _deg(long double aAngleInDegrees) {
    return AngleF::fromDegrees(static_cast<float>(aAngleInDegrees));
}
} // namespace angle_literals_f


//! Rotates a vector by a scalar angle (expressed in radians).
//! Note: This function operates in the Hobgoblin coordinate system, where the X axis grows
//! from left to right, and the Y axis grown from up to down.
template <class taAxis, class taAngle,
          T_ENABLE_IF(std::is_arithmetic<taAngle>::value)>
Vector2<taAxis> RotateVector(taAxis aVectorX, taAxis aVectorY, taAngle aAngle) {
    aVectorY = -aVectorY;
    Vector2<taAxis> result;
    #define TODBL(_x_) static_cast<double>(_x_)
    const auto _sin_ = std::sin(TODBL(aAngle));
    const auto _cos_ = std::cos(TODBL(aAngle));
    result.x = +static_cast<taAxis>(_cos_ * TODBL(aVectorX) - _sin_ * TODBL(aVectorY));
    result.y = -static_cast<taAxis>(_sin_ * TODBL(aVectorX) + _cos_ * TODBL(aVectorY));
    #undef TODBL
    return result;
}

//! Rotates a vector by a scalar angle (expressed in radians).
//! Note: This function operates in the Hobgoblin coordinate system, where the X axis grows
//! from left to right, and the Y axis grown from up to down.
template <class taVector, class taAngle,
          T_ENABLE_IF(std::is_arithmetic<taAngle>::value)>
taVector RotateVector(const taVector& aVector, taAngle aAngle) {
    const auto temp = RotateVector(aVector.x, aVector.y, aAngle);
    taVector result;
    result.x = temp.x;
    result.y = temp.y;
    return result;
}

//! Rotates a vector by an angle expressed by the class Angle<>.
//! Note: This function operates in the Hobgoblin coordinate system, where the X axis grows
//! from left to right, and the Y axis grown from up to down.
template <class taAxis, class taAngle,
          T_ENABLE_IF(std::is_class<taAngle>::value)>
Vector2<taAxis> RotateVector(taAxis aVectorX, taAxis aVectorY, taAngle aAngle) {
    return RotateVector<taAxis, typename taAngle::Real>(aVectorX, aVectorY, aAngle.asRad());
}

//! Rotates a vector by an angle expressed by the class Angle<>.
//! Note: This function operates in the Hobgoblin coordinate system, where the X axis grows
//! from left to right, and the Y axis grown from up to down.
template <class taVector, class taAngle,
          T_ENABLE_IF(std::is_class<taAngle>::value)>
taVector RotateVector(const taVector& aVector, taAngle aAngle) {
    return RotateVector<taVector, typename taAngle::Real>(aVector, aAngle.asRad());
}


} // namespace math
HOBGOBLIN_NAMESPACE_END

#include <Hobgoblin/Private/Pmacro_undef.hpp>
#include <Hobgoblin/Private/Short_namespace.hpp>

#endif // !UHOBGOBLIN_MATH_ANGLE_HPP

// clang-format on
