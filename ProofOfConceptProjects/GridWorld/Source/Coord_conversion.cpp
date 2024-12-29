
#include <GridWorld/Coord_conversion.hpp>

namespace gridworld {

hg::math::Vector2<float> ScreenCoordinatesToIsometric(hg::math::Vector2<float> aScreenCoordinates) {
    hg::math::Vector2<float> iso;
    iso.x = aScreenCoordinates.x / 2.f - aScreenCoordinates.y;
    iso.y = aScreenCoordinates.x / 2.f + aScreenCoordinates.y;
    return iso;
}

hg::math::Vector2<float> IsometricCoordinatesToScreen(hg::math::Vector2<float> aIsometricCoordinates) {
    hg::math::Vector2<float> scr;
    scr.x = aIsometricCoordinates.x + aIsometricCoordinates.y;
    scr.y = (aIsometricCoordinates.y - aIsometricCoordinates.x) / 2.f;
    return scr;
}

} // namespace gridworld
