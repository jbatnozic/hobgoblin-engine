
#include <Hobgoblin/Common.hpp>
#include <Hobgoblin/Math.hpp>

#include "Vector.hpp"

//#include <iostream> // TEMP
#include <vector>

namespace jbatnozic {
namespace raynor {

using hg::PZInteger;

#define TERN(_cond_, _then_, _else_) ((_cond_)?(_then_):(_else_))

template <class taReal>
std::vector<Vec3<taReal>> ConstructCameraAtOrigin(PZInteger aWidth, PZInteger aHeight, hg::math::Angle aFieldOfView) {
    using Real = taReal;
    constexpr Real ONE  = Real{1.0};
    constexpr Real ZERO = Real{0.0};

    const Real aspectRatio = static_cast<Real>(aWidth) / aHeight;
    // startX and startZ don't account for the 0.5 pixel shift (to make the rays go through the
    // centre of each pixel)
    const Real startX = (-ONE * TERN(aspectRatio > ONE, aspectRatio, ONE)) * std::tan(aFieldOfView.asRadians() * 0.5);
    const Real startZ = (+ONE / TERN(aspectRatio < ONE, aspectRatio, ONE)) * std::tan(aFieldOfView.asRadians() * 0.5);
    // TODO ^ std::tan doesn't account for the actual type of Real (nor does the Angle class itself)
    const Real deltaX = (Real{-2.0} * startX) / aWidth;
    const Real deltaZ = (Real{-2.0} * startZ) / aHeight;
    const Real adjustedStartX = startX + deltaX / Real{2.0};
    const Real adjustedStartZ = startZ + deltaZ / Real{2.0};

    constexpr auto ORIGIN = Vec3<Real>{ZERO, ZERO, ZERO};

    Real x = adjustedStartX;
    Real y = -ONE;
    Real z = adjustedStartZ;

    std::vector<Vec3<taReal>> result;
    for (PZInteger countZ = 0; countZ < aHeight; countZ += 1) {
        for (PZInteger countX = 0; countX < aWidth; countX += 1) {
            //printf("Point @ %d,%d: x = %f, y = %f, z = %f \n", countZ, countX, (float)x, (float)y, (float)z);
            result.push_back(Normalize(Vec3<taReal>{x, y, z}));
            x += deltaX;
        }
        x = adjustedStartX;
        z += deltaZ;
    }

    return result;
}

#undef TERN

} // namespace raynor
} // namespace jbatnozic