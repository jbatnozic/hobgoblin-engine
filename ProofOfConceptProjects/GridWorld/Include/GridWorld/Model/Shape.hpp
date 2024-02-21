#pragma once

#include <cstdint>

namespace gridworld {

enum class Shape : std::int8_t {
    EMPTY,
    CIRCLE,
    FULL_SQUARE
};

} // namespace gridworld
