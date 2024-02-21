#pragma once

#include <Hobgoblin/Math.hpp>

#include <utility>

namespace gridworld {

namespace hg = jbatnozic::hobgoblin;

//! Represents a position in the game world.
//! TODO(unused)
struct WorldPosition {
    WorldPosition() = default;

    template <class... taArgs>
    WorldPosition(taArgs&&... aArgs)
        : pos{std::forward<taArgs>(aArgs)...}
    {
    }

    hg::math::Vector2<float> pos;

    hg::math::Vector2<float>& operator*() {
        return pos;
    }

    const hg::math::Vector2<float>& operator*() const {
        return pos;
    }
};

} // namespace gridworld
