
#pragma once

#include <Hobgoblin/Math/Core.hpp>

#include <Raynor/Vector.hpp>

namespace jbatnozic {
namespace raynor {

template <class taReal>
bool RayIntersectsSphere(const Vec3<taReal>& aRayOrigin, 
                         const Vec3<taReal>& aRayDirection,
                         const Vec3<taReal>& aSphereOrigin,
                         taReal aSphereRadiusSquared,
                         taReal& aDistance) {
    taReal t0, t1; // solutions for t if the ray intersects 

    const Vec3<taReal> L = aRayOrigin - aSphereOrigin;
    const taReal a = Dot(aRayDirection, aRayDirection);
    const taReal b = taReal{2} * Dot(aRayDirection, L);
    const taReal c = Dot(L, L) - aSphereRadiusSquared;

    if (!hg::math::SolveQuadratic(a, b, c, t0, t1)) {
        return false;
    }

    if (t0 > t1) {
        std::swap(t0, t1);
    }

    if (t0 < taReal{0}) {
        t0 = t1; // if t0 is negative, let's use t1 instead 
        if (t0 < taReal{0}) {
            return false; // both t0 and t1 are negative 
        }
    }

    aDistance = t0;

    return true;
}

} // namespace raynor
} // namespace jbatnozic