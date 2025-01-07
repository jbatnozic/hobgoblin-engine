// Copyright 2024 Jovan Batnozic. Released under MS-PL licence in Serbia.
// See https://github.com/jbatnozic/Hobgoblin?tab=readme-ov-file#licence

#pragma once

#include <Hobgoblin/Math/Vector.hpp>

namespace jbatnozic {
namespace gridgoblin {

namespace hg = ::jbatnozic::hobgoblin;

// TODO: rename -> PositionInWorld
class WorldPosition {
public:
    WorldPosition()                                = default;
    WorldPosition(const WorldPosition&)            = default;
    WorldPosition& operator=(const WorldPosition&) = default;
    WorldPosition(WorldPosition&&)                 = default;
    WorldPosition& operator=(WorldPosition&&)      = default;

    WorldPosition(float aX, float aY)
        : pos{aX, aY} {}

    explicit WorldPosition(hg::math::Vector2f aPos)
        : pos{aPos} {}

    hg::math::Vector2f pos;

    hg::math::Vector2f& operator*() {
        return pos;
    }

    const hg::math::Vector2f& operator*() const {
        return pos;
    }

    hg::math::Vector2f* operator->() {
        return &pos;
    }

    const hg::math::Vector2f* operator->() const {
        return &pos;
    }
};

static_assert(sizeof(WorldPosition) <= sizeof(void*));

} // namespace gridgoblin
} // namespace jbatnozic
