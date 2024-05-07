// Copyright 2024 Jovan Batnozic. Released under MS-PL licence in Serbia.
// See https://github.com/jbatnozic/Hobgoblin?tab=readme-ov-file#licence

// clang-format off


#pragma once

#include <Hobgoblin/Math.hpp>

#include <cmath>

namespace jbatnozic {
namespace raynor {

template <class taReal>
class Vec3 {
public:
    using Real = taReal;

    explicit constexpr Vec3() noexcept
        : x{Real{0.0}}, y{Real{0.0}}, z{Real{0.0}}
    {
    }

    explicit constexpr Vec3(Real aVal) noexcept
        : x{aVal}, y{aVal}, z{aVal}
    {
    }

    explicit constexpr Vec3(Real aX, Real aY, Real aZ) noexcept
        : x{aX}, y{aY}, z{aZ}
    {
    }

    constexpr Vec3<Real> operator+(const Vec3<Real>& aOther) const noexcept {
        return Vec3<Real>{x + aOther.x, y + aOther.y, z + aOther.z};
    }

    constexpr Vec3<Real>& operator+=(const Vec3<Real>& aOther) noexcept {
        x += aOther.x;
        y += aOther.y;
        z += aOther.z;
        return *this;
    }

    constexpr Vec3<Real> operator-(const Vec3<Real>& aOther) const noexcept {
        return Vec3<Real>{x - aOther.x, y - aOther.y, z - aOther.z};
    }

    constexpr Vec3<Real>& operator-=(const Vec3<Real>& aOther) noexcept {
        x -= aOther.x;
        y -= aOther.y;
        z -= aOther.z;
        return *this;
    }

    constexpr Vec3<Real> operator*(Real aScalar) const noexcept {
        return Vec3<Real>{x * aScalar, y * aScalar, z * aScalar};
    }

    constexpr Vec3<Real>& operator*=(Real aScalar) noexcept {
        x *= aScalar;
        y *= aScalar;
        z *= aScalar;
        return *this;
    }

    constexpr Real magnitude() const {
        using hg::math::Sqr;
        return std::sqrt(Sqr(x) + Sqr(y) + Sqr(z));
    }

    Real x, y, z;
};

template <class taReal>
[[nodiscard]] constexpr Vec3<taReal> Normalize(const Vec3<taReal>& aVec) {
    auto magnitude = aVec.magnitude();
    if (magnitude != taReal{0.0}) {
        auto inverseMagnitude = taReal{1.0} / magnitude;
        return Vec3<taReal>{
            aVec.x * inverseMagnitude,
            aVec.y * inverseMagnitude,
            aVec.z * inverseMagnitude};
    }
    else {
        return aVec;
    }
}

template <class taReal>
[[nodiscard]] constexpr taReal Dot(const Vec3<taReal>& aVec1, const Vec3<taReal>& aVec2) {
    return aVec1.x * aVec2.x + aVec1.y * aVec2.y + aVec1.z * aVec2.z;
}

template <class taReal>
[[nodiscard]] constexpr hg::math::Angle<taReal> AngleBetween(const Vec3<taReal>& aVec1, const Vec3<taReal>& aVec2) {
    auto dotOfNormalized = Dot(Normalize(aVec1), Normalize(aVec2));
    return hg::math::Angle<taReal>::fromRadians(std::acos(dotOfNormalized));
}

template <class taReal>
[[nodiscard]] constexpr Vec3<taReal> Cross(const Vec3<taReal>& aVec1, const Vec3<taReal>& aVec2)
{
    return Vec3<taReal>{
        aVec1.y* aVec2.z - aVec1.z * aVec2.y,
        aVec1.z* aVec2.x - aVec1.x * aVec2.z,
        aVec1.x* aVec2.y - aVec1.y * aVec2.x};
}

}
}

// clang-format on
