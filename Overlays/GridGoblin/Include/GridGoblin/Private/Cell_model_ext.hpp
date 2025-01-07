// Copyright 2024 Jovan Batnozic. Released under MS-PL licence in Serbia.
// See https://github.com/jbatnozic/Hobgoblin?tab=readme-ov-file#licence

#pragma once

#include <GridGoblin/Model/Cell_model.hpp>

#include <Hobgoblin/Math/Vector.hpp>

#include <cassert>
#include <cstdint>

namespace jbatnozic {
namespace gridgoblin {

namespace hg = jbatnozic::hobgoblin;

class ChunkExtensionInterface;

namespace detail {

enum class DrawMode {
    NONE,
    LOWERED,
    FULL

    // TODO(graphics): LoS blocker bitmask
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

        void setChunkExtensionPointer(ChunkExtensionInterface* aChunkExtensionPointer);
        bool hasChunkExtensionPointer() const;
        ChunkExtensionInterface* getChunkExtensionPointer() const;

        //! Call when one of the neighbours changes shape.
        //! \warning caling this is UB when `hasChunkExtensionPointer()` returns `true`.
        void refresh(const CellModelExt* aNorthNeighbour,
                     const CellModelExt* aWestNeighbour,
                     const CellModelExt* aEastNeighbour,
                     const CellModelExt* aSouthNeighbour);

        //! \warning caling this is UB when `hasChunkExtensionPointer()` returns `true`.
        DrawMode determineDrawMode(float              aCellResolution,
                                   hg::math::Vector2f aCellPosition,
                                   hg::math::Vector2f aPointOfView)
            const; // TODO(graphics): needs also to return locations from which to pick up light

    private:
        union {
            DrawModePredicate        drawModePredicate;
            ChunkExtensionInterface* chunkExtension;
        } _pointerStorage;

        bool _holdingExtension = false;
        bool _visible          = false;
        bool _lowered          = false;
    };

    static_assert(sizeof(ExtensionData) <= 2 * sizeof(void*));

    mutable ExtensionData mutableExtensionData;
};

} // namespace detail
} // namespace gridgoblin
} // namespace jbatnozic
