#pragma once

#include <GridWorld/Model/Sprites.hpp>

#include <Hobgoblin/Math/Vector.hpp>

#include <array>
#include <cstdint>
#include <optional>
#include <unordered_map>

namespace gridworld {

namespace hg = jbatnozic::hobgoblin;

namespace model {

enum NeighbourIndex {
    IDX_NORTH,
    IDX_WEST,
    IDX_EAST,
    IDX_SOUTH
};

enum class Shape : std::int8_t {
    EMPTY,
    FULL_SQUARE
};

enum class DrawMode {
    NONE,
    LOWERED,
    FULL
};

struct Cell;

using DrawModePredicate = DrawMode(*)(float aCellResolution,
                                      hg::math::Vector2f aCellPosition,
                                      hg::math::Vector2f aPointOfView);

struct Cell {
    struct Floor {
        SpriteId spriteId;
    };

    struct Wall {
        SpriteId spriteId;
        SpriteId spriteId_lowered;
        Shape    shape;
    };

    std::optional<Floor> floor;
    std::optional<Wall>  wall;

    //! Shows which neighbouring tiles block this tile
    //! [0] - north neighbour
    //! [1] - west neighbour
    //! [2] - east neighbour
    //! [3] - south neighbour
    //! (scanline pattern)
    //! * edge of grid is considered a blocker
    std::array<bool, 4> blockers = {false, false, false, false};

    DrawModePredicate drawModePredicate;

    Cell();

    void refresh(const Cell* aNorthNeighbour,
                 const Cell* aWestNeighbour,
                 const Cell* aEastNeighbour,
                 const Cell* aSouthNeighbour);
};

} // namespace model
} // namespace gridw
