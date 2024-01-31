#pragma once

#include <Hobgoblin/Graphics/Render_texture.hpp>
#include <Hobgoblin/Math.hpp>
#include <Hobgoblin/Utility/Grids.hpp>

#include <GridWorld/Model/Sprites.hpp>

#include <cstdint>
#include <unordered_map>

namespace gridworld {

namespace hg = jbatnozic::hobgoblin;

namespace model {

//! Identifies a single Light within a World.
using LightId = std::int32_t;

struct LightData {
    SpriteId spriteId = 0;
    hg::math::AngleF angle = hg::math::AngleF::zero();
    hg::math::Vector2f position = {0.f, 0.f};

    mutable hg::gr::RenderTexture texture;
};

using LightMap = std::unordered_map<LightId, LightData>;

using LightDataMapConstIterator = LightMap::const_iterator;

} // namespace model
} // namespace gridworld
