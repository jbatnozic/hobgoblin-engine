
#pragma once

#include <Hobgoblin/Math.hpp>
#include <SFML/Graphics.hpp> // TODO TEMP

#include <Raynor/Geometry.hpp>
#include <Raynor/Vector.hpp>

#include <cmath>

namespace jbatnozic {
namespace raynor {

template <class taReal>
struct BoundingSphere {
    Vec3<taReal> centre;
    taReal radius;
};

template <class taReal>
class SceneLight {
};

template <class taReal>
class SceneObject {
public:
    using Real = taReal;

    virtual ~SceneObject() = default;

    virtual bool intersectsRay(const Vec3<Real>& aRayOrigin,
                               const Vec3<Real>& aRayDirection,
                               Real& aDistance) const = 0;

    virtual const BoundingSphere<Real>& getBoundingSphere() const = 0;

    virtual sf::Color getColor() const = 0;

    // *** COPY/PASTED BEGIN
    virtual void getSurfaceData(const Vec3<Real>& aPointOfIntersection,
                                Vec3<Real>& aNormalAtIntersection,
                                Vec3<Real>& aTextureCoordinates) const = 0;
    // *** COPY/PASTED END
};

template <class taReal>
class Sphere : public SceneObject<taReal> {
public:
    using Real = taReal;

    Sphere(Vec3<Real> aCentre, Real aRadius, sf::Color color)
        : _sphere{aCentre, aRadius}
        , _radius2{aRadius * aRadius}
        , _color{color}
    {
    }

    bool intersectsRay(const Vec3<Real>& aOrigin, const Vec3<Real>& aDirection, Real& t) const override {
        return RayIntersectsSphere(aOrigin, aDirection, _sphere.centre, _radius2, t);
    }

    const BoundingSphere<Real>& getBoundingSphere() const override {
        return _sphere;
    }

    sf::Color getColor() const override {
        return _color;
    }

    // *** COPY/PASTED BEGIN
    void getSurfaceData(const Vec3<Real>& aPointOfIntersection,
                        Vec3<Real>& aNormalAtIntersection,
                        Vec3<Real>& aTextureCoordinates) const override {
        aNormalAtIntersection = Normalize(aPointOfIntersection - _sphere.centre);
        // In this particular case, the normal is simular to a point on a unit sphere
        // centred around the origin. We can thus use the normal coordinates to compute
        // the spherical coordinates of Phit.
        // atan2 returns a value in the range [-pi, pi] and we need to remap it to range [0, 1]
        // acosf returns a value in the range [0, pi] and we also need to remap it to the range [0, 1]
        // tex.x = (1 + atan2(Nhit.z, Nhit.x) / M_PI) * 0.5;
        // tex.y = acosf(Nhit.y) / M_PI;
    }
    // *** COPY/PASTED END

private:
    BoundingSphere<Real> _sphere;
    Real _radius2;
    sf::Color _color;
};

} // namespace raynor
} // namespace jbatnozic