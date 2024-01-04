#pragma once

#include <Hobgoblin/Math.hpp>

namespace gridworld {

namespace hg = jbatnozic::hobgoblin;

hg::math::Vector2<float> ScreenCoordinatesToIsometric(hg::math::Vector2<float> aScreenCoordinates);

hg::math::Vector2<float> IsometricCoordinatesToScreen(hg::math::Vector2<float> aIsometricCoordinates);

} // namespace gridworld
