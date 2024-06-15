#pragma once

#include <GridWorld/Model/Shape.hpp>
#include <GridWorld/Model/Sprites.hpp>

#include <Hobgoblin/Math/Vector.hpp>

#include <array>
#include <cstdint>
#include <optional>
#include <unordered_map>

namespace gridworld {

namespace hg = jbatnozic::hobgoblin;

#ifdef FUTURE
enum NeighbourIndex {
    IDX_NORTH,
    IDX_WEST,
    IDX_EAST,
    IDX_SOUTH
};
#endif

struct CellModel {
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
};

// TODO
std::optional<CellModel::Floor> MakeFloor();
// TODO
std::optional<CellModel::Wall> MakeWall();

namespace detail {
enum class DrawMode {
    NONE,
    LOWERED,
    FULL

    // TODO: height
    // TODO: LoS blocker bitmask
};

using DrawModePredicate = DrawMode (*)(float              aCellResolution,
                                       hg::math::Vector2f aCellPosition,
                                       hg::math::Vector2f aPointOfView);

//! Cell model extended with optimization data.
class CellModelExt : public CellModel {
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

        DrawMode determineDrawMode(float              aCellResolution,
                                   hg::math::Vector2f aCellPosition,
                                   hg::math::Vector2f aPointOfView)
            const; // TODO: needs also to return locations from which to pick up light

    private:
        DrawModePredicate _drawModePredicate;
        bool              _visible = false;
        bool              _lowered = false;
    };

    mutable ExtensionData mutableExtensionData;
};

} // namespace detail

} // namespace gridworld
