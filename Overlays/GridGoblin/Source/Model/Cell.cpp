// Copyright 2024 Jovan Batnozic. Released under MS-PL licence in Serbia.
// See https://github.com/jbatnozic/Hobgoblin?tab=readme-ov-file#licence

#include <GridGoblin/Model/Cell.hpp>

#include <Hobgoblin/HGExcept.hpp>

namespace jbatnozic {
namespace gridgoblin {
namespace detail {

namespace predicate {
namespace {

constexpr float Half(float aVal) {
    return aVal * 0.5;
}

bool IsPovCloserThanCell(hg::math::Vector2f aCellPosition, hg::math::Vector2f aPointOfView) {
    return ((aCellPosition.x - aCellPosition.y) > (aPointOfView.x - aPointOfView.y));
}

DrawMode FullWhenPovIsCloserToCamera(float              aCellResolution,
                                     hg::math::Vector2f aCellPosition,
                                     hg::math::Vector2f aPointOfView) {
    if (IsPovCloserThanCell(aCellPosition, aPointOfView)) {
        return DrawMode::FULL;
    }
    return DrawMode::LOWERED;
}

DrawMode X1(float aCellResolution, hg::math::Vector2f aCellPosition, hg::math::Vector2f aPointOfView) {
    if (aPointOfView.y <= aCellPosition.y || aPointOfView.x >= aCellPosition.x + aCellResolution) {
        return DrawMode::LOWERED;
    }
    return DrawMode::FULL;
}

DrawMode FullWhenPovIsSouthOrCloserToCamera(float              aCellResolution,
                                            hg::math::Vector2f aCellPosition,
                                            hg::math::Vector2f aPointOfView) {
    if ((aPointOfView.y >= aCellPosition.y + aCellResolution) ||
        IsPovCloserThanCell(aCellPosition, aPointOfView)) {
        return DrawMode::FULL;
    }
    return DrawMode::LOWERED;
}

DrawMode FullWhenPovIsWestOrCloserToCamera(float              aCellResolution,
                                           hg::math::Vector2f aCellPosition,
                                           hg::math::Vector2f aPointOfView) {
    if ((aPointOfView.x <= aCellPosition.x) || IsPovCloserThanCell(aCellPosition, aPointOfView)) {
        return DrawMode::FULL;
    }
    return DrawMode::LOWERED;
}

DrawMode FullWhenPovIsWestOrSouth(float              aCellResolution,
                                  hg::math::Vector2f aCellPosition,
                                  hg::math::Vector2f aPointOfView) {
    if ((aPointOfView.x <= aCellPosition.x) || (aPointOfView.y >= aCellPosition.y + aCellResolution)) {
        return DrawMode::FULL;
    }
    return DrawMode::LOWERED;
}

DrawMode FullWhenPovIsWest(float              aCellResolution,
                           hg::math::Vector2f aCellPosition,
                           hg::math::Vector2f aPointOfView) {
    if (aPointOfView.x <= aCellPosition.x) {
        return DrawMode::FULL;
    }
    return DrawMode::LOWERED;
}

DrawMode FullWhenPovIsWest2(float              aCellResolution,
                            hg::math::Vector2f aCellPosition,
                            hg::math::Vector2f aPointOfView) {
    if (aPointOfView.x <= aCellPosition.x + aCellResolution) {
        return DrawMode::FULL;
    }
    return DrawMode::LOWERED;
}

DrawMode FullWhenPovIsSouth(float              aCellResolution,
                            hg::math::Vector2f aCellPosition,
                            hg::math::Vector2f aPointOfView) {
    if (aPointOfView.y >= aCellPosition.y + aCellResolution) {
        return DrawMode::FULL;
    }
    return DrawMode::LOWERED;
}

DrawMode FullWhenPovIsSouth2(float              aCellResolution,
                             hg::math::Vector2f aCellPosition,
                             hg::math::Vector2f aPointOfView) {
    if (aPointOfView.y >= aCellPosition.y + aCellResolution) {
        return DrawMode::FULL;
    }
    return DrawMode::LOWERED;
}

DrawMode FullWhenPovIsSouthAndCloserToCamera(float              aCellResolution,
                                             hg::math::Vector2f aCellPosition,
                                             hg::math::Vector2f aPointOfView) {
    if ((aPointOfView.y >= aCellPosition.y + aCellResolution) &&
        IsPovCloserThanCell(aCellPosition, aPointOfView)) {
        return DrawMode::FULL;
    }
    return DrawMode::LOWERED;
}

DrawMode FullWhenPovIsWestAndCloserToCamera(float              aCellResolution,
                                            hg::math::Vector2f aCellPosition,
                                            hg::math::Vector2f aPointOfView) {
    if ((aPointOfView.x <= aCellPosition.x) && IsPovCloserThanCell(aCellPosition, aPointOfView)) {
        return DrawMode::FULL;
    }
    return DrawMode::LOWERED;
}

DrawMode AlwaysLowered(float              aCellResolution,
                       hg::math::Vector2f aCellPosition,
                       hg::math::Vector2f aPointOfView) {
    return DrawMode::LOWERED;
}

DrawMode AlwaysNone(float              aCellResolution,
                    hg::math::Vector2f aCellPosition,
                    hg::math::Vector2f aPointOfView) {
    return DrawMode::NONE;
}

std::array<decltype(&AlwaysNone), 16> SELECTION_TABLE = {
    // b0000
    //   _
    // _ O _
    //   _
    //&FullWhenPovIsCloserToCamera,
    &X1,

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
    //&FullWhenPovIsSouth,
    &FullWhenPovIsSouth2,

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
    //&FullWhenPovIsWest,
    &FullWhenPovIsWest2,

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

CellModelExt::ExtensionData::ExtensionData()
    : _pointerStorage{&predicate::AlwaysNone} {}

void CellModelExt::ExtensionData::setVisible(bool aIsVisible) {
    _visible = aIsVisible;
}

bool CellModelExt::ExtensionData::isVisible() const {
    return _visible;
}

void CellModelExt::ExtensionData::setLowered(bool aIsLowered) {
    _lowered = aIsLowered;
}

bool CellModelExt::ExtensionData::isLowered() const {
    return _lowered;
}

void CellModelExt::ExtensionData::setChunkExtensionPointer(
    ChunkExtensionInterface* aChunkExtensionPointer)
//
{
    _pointerStorage.chunkExtension = aChunkExtensionPointer;
    _holdingExtension              = true;
}

bool CellModelExt::ExtensionData::hasChunkExtensionPointer() const {
    return _holdingExtension;
}

ChunkExtensionInterface* CellModelExt::ExtensionData::getChunkExtensionPointer() const {
    return _pointerStorage.chunkExtension;
}

void CellModelExt::ExtensionData::refresh(const CellModelExt* aNorthNeighbour,
                                          const CellModelExt* aWestNeighbour,
                                          const CellModelExt* aEastNeighbour,
                                          const CellModelExt* aSouthNeighbour) {
    HG_ASSERT(!_holdingExtension);

    // clang-format off
    const bool blockedFromNorth = !aNorthNeighbour || aNorthNeighbour->isWallInitialized(); // TODO(temporary)
    const bool blockedFromWest  = !aWestNeighbour  || aWestNeighbour->isWallInitialized();  // TODO(temporary)
    const bool blockedFromEast  = !aEastNeighbour  || aEastNeighbour->isWallInitialized();  // TODO(temporary)
    const bool blockedFromSouth = !aSouthNeighbour || aSouthNeighbour->isWallInitialized(); // TODO(temporary)

    const auto selector = (blockedFromEast ? 0x01 : 0) | (blockedFromNorth ? 0x02 : 0) |
                          (blockedFromWest ? 0x04 : 0) | (blockedFromSouth ? 0x08 : 0);

    _pointerStorage.drawModePredicate = predicate::SELECTION_TABLE[static_cast<std::size_t>(selector)];
    // clang-format on
}

DrawMode CellModelExt::ExtensionData::determineDrawMode(float              aCellResolution,
                                                        hg::math::Vector2f aCellPosition,
                                                        hg::math::Vector2f aPointOfView) const {
    HG_ASSERT(!_holdingExtension);

    return _pointerStorage.drawModePredicate(aCellResolution, aCellPosition, aPointOfView);
}

} // namespace detail
} // namespace gridgoblin
}
