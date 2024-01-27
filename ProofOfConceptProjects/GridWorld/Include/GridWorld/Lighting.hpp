#pragma once

#include <Hobgoblin/Graphics/Render_texture.hpp>
#include <Hobgoblin/Math.hpp>
#include <Hobgoblin/Utility/Grids.hpp>

#include <GridWorld/Sprites.hpp>

#include <cstdint>
#include <unordered_map>

namespace gridworld {

namespace hg = jbatnozic::hobgoblin;

using LightId = std::int32_t;

namespace detail {

struct LightData {
    SpriteId spriteId = 0;
    hg::math::AngleF angle = hg::math::AngleF::zero();
    hg::math::Vector2f position = {0.f, 0.f};

    mutable hg::gr::RenderTexture texture;
};

using LightMap = std::unordered_map<LightId, LightData>;

using LightDataConstIterator = LightMap::const_iterator;

} // namespace detail

} // namespace gridworld
