#pragma once

#include <GridWorld/Model/Shape.hpp>
#include <GridWorld/Model/Sprites.hpp>

#include <Hobgoblin/Math/Vector.hpp>

#include <array>
#include <cassert>
#include <cstdint>
#include <optional>
#include <tuple>
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

class CellModel {
public:
    struct Floor {
        SpriteId spriteId; // TBD: spriteId to 16 bit?
    };

    struct Wall {
        SpriteId spriteId;
        SpriteId spriteId_lowered;
        Shape    shape;
    };

    enum Flags : std::uint8_t {
        FLOOR_INITIALIZED = 0x01,
        WALL_INITIALIZED  = 0x02
    };

    std::uint8_t getFlags() const;

    //! Returns true if the Floor structure of this Cell is initialized.
    //! \note equivalent to `(getFlags() & FLOOR_INITIALIZED) != 0`.
    bool isFloorInitialized() const;

    //! Returns true if the Floor structure of this Cell is initialized.
    //! \note equivalent to `(getFlags() & FLOOR_INITIALIZED) != 0`.
    bool isWallInitialized() const;

    //! Returns the Floor structure of this Cell.
    //! \warning it is undefined what is returned if `isFloorInitialized() == false`!
    const Floor& getFloor() const;

    //! Returns the Wall structure of this Cell.
    //! \warning it is undefined what is returned if `isWallInitialized() == false`!
    const Wall& getWall() const;

    //! Sets a new value for the Floor structure of this Cell
    //! and marks it as initialized.
    void setFloor(Floor aFloor);

    //! Uninitializes the Floor structure of this Cell.
    void resetFloor();

    //! Sets a new value for the Wall structure of this Cell
    //! and marks it as initialized.
    void setWall(Wall aWall);

    //! Uninitializes the Wall structure of this Cell.
    void resetWall();

private:
    Floor _floor;
    Wall  _wall;
    int   _flags = 0;
};

//! CellModel::Floor equality operator.
inline bool operator==(const CellModel::Floor& aLhs, const CellModel::Floor& aRhs) {
    return aLhs.spriteId == aRhs.spriteId;
}

//! CellModel::Floor inequality operator.
inline bool operator!=(const CellModel::Floor& aLhs, const CellModel::Floor& aRhs) {
    return !(aLhs == aRhs);
}

//! CellModel::Wall equality operator.
inline bool operator==(const CellModel::Wall& aLhs, const CellModel::Wall& aRhs) {
    return std::tie(aLhs.spriteId, aLhs.spriteId_lowered, aLhs.shape) ==
           std::tie(aRhs.spriteId, aRhs.spriteId_lowered, aRhs.shape);
}

//! CellModel::Wall inequality operator.
inline bool operator!=(const CellModel::Wall& aLhs, const CellModel::Wall& aRhs) {
    return !(aLhs == aRhs);
}

//! CellModel equality operator.
inline bool operator==(const CellModel& aLhs, const CellModel& aRhs) {
    if (aLhs.getFlags() != aRhs.getFlags()) {
        return false;
    }

    if (aLhs.isFloorInitialized() && (aLhs.getFloor() != aRhs.getFloor())) {
        return false;
    }

    if (aLhs.isWallInitialized() && (aLhs.getWall() != aRhs.getWall())) {
        return false;
    }

    return true;
}

//! CellModel inequality operator.
inline bool operator!=(const CellModel& aLhs, const CellModel& aRhs) {
    return !(aLhs == aRhs);
}

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

//! Cell model extended with runtime optimization data.
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

inline std::uint8_t CellModel::getFlags() const {
    return _flags;
}

inline bool CellModel::isFloorInitialized() const {
    return (_flags & FLOOR_INITIALIZED) != 0;
}

inline bool CellModel::isWallInitialized() const {
    return (_flags & FLOOR_INITIALIZED) != 0;
}

inline const CellModel::Floor& CellModel::getFloor() const {
    assert(isFloorInitialized());
    return _floor;
}

inline const CellModel::Wall& CellModel::getWall() const {
    assert(isWallInitialized());
    return _wall;
}

inline void CellModel::setFloor(Floor aFloor) {
    _floor = aFloor;
    _flags |= FLOOR_INITIALIZED;
}

inline void CellModel::resetFloor() {
    _flags &= ~FLOOR_INITIALIZED;
}

inline void CellModel::setWall(Wall aWall) {
    _wall = aWall;
    _flags |= WALL_INITIALIZED;
}

inline void CellModel::resetWall() {
    _flags &= ~WALL_INITIALIZED;
}

} // namespace gridworld
