// Copyright 2024 Jovan Batnozic. Released under MS-PL licence in Serbia.
// See https://github.com/jbatnozic/Hobgoblin?tab=readme-ov-file#licence

#pragma once

#include <Hobgoblin/Math.hpp>

namespace griddy {

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

} // namespace griddy
