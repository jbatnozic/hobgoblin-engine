#pragma once

#include <Hobgoblin/Math.hpp>

namespace gridworld {

namespace hg = jbatnozic::hobgoblin;

// TODO: rename to dimetric
hg::math::Vector2<float> ScreenCoordinatesToIsometric(hg::math::Vector2<float> aScreenCoordinates);

// TODO: rename to dimetric
hg::math::Vector2<float> IsometricCoordinatesToScreen(hg::math::Vector2<float> aIsometricCoordinates);

class DimetricProjection {
public:
    hg::math::Vector2<float> ScreenCoordinatesToWorld(hg::math::Vector2<float>); // TODO
    hg::math::Vector2<float> WorldCoordinatesToScreen(hg::math::Vector2<float>); // TODO
};

} // namespace gridworld
