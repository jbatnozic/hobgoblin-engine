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
    //std::array<bool, 4> blockers = {false, false, false, false};

    //mutable bool lowered = false;

    // TODO: bool isEligibleToBeDrawn (not if totally blocked)
};

} // namespace model

namespace detail {
enum class DrawMode {
    NONE,
    LOWERED,
    FULL

    // TODO: height
    // TODO: LoS blocker bitmask
};

using DrawModePredicate = DrawMode(*)(float aCellResolution,
                                      hg::math::Vector2f aCellPosition,
                                      hg::math::Vector2f aPointOfView);

//! Cell model extended with optimization data.
class CellModelExt : public model::Cell {
public:
    class ExtensionData {
    public:
        ExtensionData();

        void setVisible(bool aIsVisible);

        bool isVisible() const;

        void setLowered(bool aIsLowered);

        bool isLowered() const;

        //! Call when one of the neighbours changes shape.
        void refresh(const CellModelExt* aNorthNeighbour,
                     const CellModelExt* aWestNeighbour,
                     const CellModelExt* aEastNeighbour,
                     const CellModelExt* aSouthNeighbour);

        DrawMode determineDrawMode(float aCellResolution,
                                   hg::math::Vector2f aCellPosition,
                                   hg::math::Vector2f aPointOfView) const; // TODO: needs also to return locations from which to pick up light

    private:
        DrawModePredicate _drawModePredicate;
        bool _visible = false;
        bool _lowered = false;
    };

    mutable ExtensionData mutableExtensionData;
};

} // namespace detail

} // namespace gridw
