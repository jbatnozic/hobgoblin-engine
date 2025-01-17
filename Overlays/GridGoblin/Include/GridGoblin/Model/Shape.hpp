// Copyright 2024 Jovan Batnozic. Released under MS-PL licence in Serbia.
// See https://github.com/jbatnozic/Hobgoblin?tab=readme-ov-file#licence

#pragma once

#include <cstdint>

namespace jbatnozic {
namespace gridgoblin {

enum class Shape : std::int8_t {
    EMPTY,
    CIRCLE,
    FULL_SQUARE
};

} // namespace gridgoblin
} // namespace jbatnozic
