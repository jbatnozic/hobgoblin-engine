#pragma once

#include <Hobgoblin/Math.hpp>

namespace gridworld {

namespace hg = jbatnozic::hobgoblin;

// TODO: rename to dimetric
hg::math::Vector2<float> ScreenCoordinatesToIsometric(hg::math::Vector2<float> aScreenCoordinates);

// TODO: rename to dimetric
hg::math::Vector2<float> IsometricCoordinatesToScreen(hg::math::Vector2<float> aIsometricCoordinates);

} // namespace gridworld
