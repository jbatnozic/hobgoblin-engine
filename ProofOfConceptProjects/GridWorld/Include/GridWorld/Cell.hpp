#pragma once

#include <Hobgoblin/Common.hpp>
#include <Hobgoblin/Graphics/Sprite_loader.hpp>
#include <Hobgoblin/Graphics/Render_texture.hpp>
#include <Hobgoblin/Math.hpp>
#include <Hobgoblin/Utility/Grids.hpp>

#include <optional>
#include <unordered_map>

namespace gridworld {

namespace hg = jbatnozic::hobgoblin;

struct Cell {
    struct Floor {
        SpriteId spriteId;
    };

    struct Wall {
        SpriteId spriteId;
        SpriteId spriteId_lowered;
    };

    Floor floor;
    std::optional<Wall> wall;
};

} // namespace gridw
