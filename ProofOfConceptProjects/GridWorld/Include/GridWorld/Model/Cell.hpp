#pragma once

#include <GridWorld/Model/Sprites.hpp>

#include <array>
#include <optional>
#include <unordered_map>

namespace gridworld {

namespace hg = jbatnozic::hobgoblin;

namespace model {

struct Cell {
    struct Floor {
        SpriteId spriteId;
    };

    struct Wall {
        SpriteId spriteId;
        SpriteId spriteId_lowered;
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

    void refresh(const Cell* aNorthNeighbour,
                 const Cell* aWestNeighbour,
                 const Cell* aEastNeighbour,
                 const Cell* aSouthNeighbour); // TODO
};

} // namespace model
} // namespace gridw
