#pragma once

#include <GridWorld/Model/Cell.hpp>

namespace gridworld {
namespace model {

namespace predicate {
namespace {
//if (overlaps(aOther)) {
//    return ((centre.x - centre.y) > (aOther.centre.x - aOther.centre.y)) ? DRAW_THIS_FIRST : DRAW_OTHER_FIRST;
//}
bool IsPovCloserThanCell(hg::math::Vector2f aCellPosition,
                         hg::math::Vector2f aPointOfView) {
    return ((aCellPosition.x - aCellPosition.y) > (aPointOfView.x - aPointOfView.y));
}

DrawMode FullWhenPovIsCloserToCamera(
    float aCellResolution,
    hg::math::Vector2f aCellPosition,
    hg::math::Vector2f aPointOfView)
{
    if (IsPovCloserThanCell(aCellPosition, aPointOfView)) {
        return DrawMode::FULL;
    }
    return DrawMode::LOWERED;
}

DrawMode FullWhenPovIsSouthOrCloserToCamera(
    float aCellResolution,
    hg::math::Vector2f aCellPosition,
    hg::math::Vector2f aPointOfView)
{
    if ((aPointOfView.y >= aCellPosition.y + aCellResolution) ||
        IsPovCloserThanCell(aCellPosition, aPointOfView)) {
        return DrawMode::FULL;
    }
    return DrawMode::LOWERED;
}

DrawMode FullWhenPovIsWestOrCloserToCamera(
    float aCellResolution,
    hg::math::Vector2f aCellPosition,
    hg::math::Vector2f aPointOfView)
{
    if ((aPointOfView.x <= aCellPosition.x) ||
        IsPovCloserThanCell(aCellPosition, aPointOfView)) {
        return DrawMode::FULL;
    }
    return DrawMode::LOWERED;
}

DrawMode FullWhenPovIsWestOrSouth(
    float aCellResolution,
    hg::math::Vector2f aCellPosition,
    hg::math::Vector2f aPointOfView)
{
    if ((aPointOfView.x <= aCellPosition.x) ||
        (aPointOfView.y >= aCellPosition.y + aCellResolution)) {
        return DrawMode::FULL;
    }
    return DrawMode::LOWERED;
}

DrawMode FullWhenPovIsWest(
    float aCellResolution,
    hg::math::Vector2f aCellPosition,
    hg::math::Vector2f aPointOfView)
{
    if (aPointOfView.x <= aCellPosition.x) {
        return DrawMode::FULL;
    }
    return DrawMode::LOWERED;
}

DrawMode FullWhenPovIsSouth(
    float aCellResolution,
    hg::math::Vector2f aCellPosition,
    hg::math::Vector2f aPointOfView)
{
    if (aPointOfView.y >= aCellPosition.y + aCellResolution) {
        return DrawMode::FULL;
    }
    return DrawMode::LOWERED;
}

DrawMode FullWhenPovIsSouthAndCloserToCamera(
    float aCellResolution,
    hg::math::Vector2f aCellPosition,
    hg::math::Vector2f aPointOfView)
{
    if ((aPointOfView.y >= aCellPosition.y + aCellResolution) &&
        IsPovCloserThanCell(aCellPosition, aPointOfView)) {
        return DrawMode::FULL;
    }
    return DrawMode::LOWERED;
}

DrawMode FullWhenPovIsWestAndCloserToCamera(
    float aCellResolution,
    hg::math::Vector2f aCellPosition,
    hg::math::Vector2f aPointOfView)
{
    if ((aPointOfView.x <= aCellPosition.x) &&
        IsPovCloserThanCell(aCellPosition, aPointOfView)) {
        return DrawMode::FULL;
    }
    return DrawMode::LOWERED;
}

DrawMode AlwaysLowered(
    float aCellResolution,
    hg::math::Vector2f aCellPosition,
    hg::math::Vector2f aPointOfView)
{
    return DrawMode::LOWERED;
}

DrawMode AlwaysNone(
    float aCellResolution,
    hg::math::Vector2f aCellPosition,
    hg::math::Vector2f aPointOfView)
{
    return DrawMode::NONE;
}

std::array<DrawModePredicate, 16> SELCTION_TABLE = {
    // b0000
    //   _
    // _ O _
    //   _
    &FullWhenPovIsCloserToCamera,

    // b0001
    //   _
    // _ O X
    //   _
    &FullWhenPovIsSouthOrCloserToCamera,

    // b0010
    //   X
    // _ O _
    //   _
    &FullWhenPovIsWestOrCloserToCamera,

    // b0011
    //   X
    // _ O X
    //   _
    &FullWhenPovIsWestOrSouth,

    // b0100
    //   _
    // X O _
    //   _
    &FullWhenPovIsSouth,

    // b0101
    //   _
    // X O X
    //   _
    &FullWhenPovIsSouth,

    // b0110
    //   X
    // X O _
    //   _
    &FullWhenPovIsSouthAndCloserToCamera,

    // b0111
    //   X
    // X O X
    //   _
    &FullWhenPovIsSouth,

    // b1000
    //   _
    // _ O _
    //   X
    &FullWhenPovIsWestAndCloserToCamera,

    // b1001
    //   _
    // _ O X
    //   X
    &FullWhenPovIsWestAndCloserToCamera,

    // b1010
    //   X
    // _ O _
    //   X
    &FullWhenPovIsWest,

    // b1011
    //   X
    // _ O X
    //   X
    &FullWhenPovIsWest,

    // b1100
    //   _
    // X O _
    //   X
    &AlwaysLowered,

    // b1101
    //   _
    // X O X
    //   X
    &AlwaysLowered,

    // b1110
    //   X
    // X O _
    //   X
    &AlwaysLowered,

    // b1111
    //   X
    // X O X
    //   X
    &AlwaysNone,
};
} // namespace
} // namespace predicate

Cell::Cell()
    : drawModePredicate{&predicate::AlwaysNone}
{
}

void Cell::refresh(const Cell* aNorthNeighbour,
                   const Cell* aWestNeighbour,
                   const Cell* aEastNeighbour,
                   const Cell* aSouthNeighbour)
{
    const bool blockedFromNorth = !aNorthNeighbour || aNorthNeighbour->wall.has_value(); // TODO(temporary)
    const bool blockedFromWest  = !aWestNeighbour  || aWestNeighbour->wall.has_value();  // TODO(temporary)
    const bool blockedFromEast  = !aEastNeighbour  || aEastNeighbour->wall.has_value();  // TODO(temporary)
    const bool blockedFromSouth = !aSouthNeighbour || aSouthNeighbour->wall.has_value(); // TODO(temporary)

    const auto selector =
        (blockedFromEast  ? 0x01 : 0) |
        (blockedFromNorth ? 0x02 : 0) |
        (blockedFromWest  ? 0x04 : 0) |
        (blockedFromSouth ? 0x08 : 0);

    drawModePredicate = predicate::SELCTION_TABLE[static_cast<std::size_t>(selector)];
}

} // namespace model
} // namespace gridw
