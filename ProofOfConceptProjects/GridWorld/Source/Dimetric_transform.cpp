#pragma once

#include <GridWorld/Dimetric_transform.hpp>

namespace gridworld {

namespace hg = jbatnozic::hobgoblin;

// clang-format off
const hg::gr::Transform DIMETRIC_TRANSFORM {
     1.0f, 1.0f, 0.f,
    -0.5f, 0.5f, 0.f,
      0.f, 0.0f, 1.f
};
// clang-format on

} // namespace gridworld
