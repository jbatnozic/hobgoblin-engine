#pragma once

#include <Hobgoblin/Graphics/Render_texture.hpp>
#include <Hobgoblin/Math.hpp>

#include <GridWorld/Model/Sprites.hpp>

#include <cstdint>

namespace gridworld {

namespace hg = jbatnozic::hobgoblin;

//! Identifies a single Light within a World.
using LightId = std::int32_t;

struct LightModel {
    SpriteId           spriteId = 0;
    hg::math::AngleF   angle    = hg::math::AngleF::zero();
    hg::math::Vector2f position = {0.f, 0.f};
};

namespace detail {

//! Light model extended with optimization data.
class LightModelExt : public LightModel {
public:
    class ExtensionData {
    public:
        hg::gr::RenderTexture texture;
    };

    mutable ExtensionData mutableExtensionData;
};

} // namespace detail

} // namespace gridworld
