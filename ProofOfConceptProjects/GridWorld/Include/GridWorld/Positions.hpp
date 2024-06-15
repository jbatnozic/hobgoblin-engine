#pragma once

#include <Hobgoblin/Math/Vector.hpp>

#include <utility>

namespace gridworld {

namespace hg = jbatnozic::hobgoblin;

//! Represents a position in the game world.
struct WorldPosition {
    WorldPosition() = default;

    ~WorldPosition() = default;

    WorldPosition(const WorldPosition&) = default;

    WorldPosition& operator=(const WorldPosition&) = default;

    WorldPosition(WorldPosition&&) = default;

    WorldPosition& operator=(WorldPosition&&) = default;

    WorldPosition(hg::math::Vector2f aPos)
        : pos{aPos} {}

    // template <class... taArgs>
    // WorldPosition(taArgs&&... aArgs)
    //     : pos{std::forward<taArgs>(aArgs)...}
    //{
    // }

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

static_assert(sizeof(WorldPosition) <= sizeof(void*), "TODO");

} // namespace gridworld
