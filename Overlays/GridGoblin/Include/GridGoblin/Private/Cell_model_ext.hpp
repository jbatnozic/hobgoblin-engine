// Copyright 2024 Jovan Batnozic. Released under MS-PL licence in Serbia.
// See https://github.com/jbatnozic/Hobgoblin?tab=readme-ov-file#licence

#pragma once

#include <GridGoblin/Model/Cell_model.hpp>
#include <GridGoblin/Spatial/Position_in_world.hpp>

#include <Hobgoblin/Math/Vector.hpp>

#include <cassert>
#include <cstdint>

namespace jbatnozic {
namespace gridgoblin {

namespace hg = jbatnozic::hobgoblin;

class ChunkExtensionInterface;

namespace detail {

//! Information about how a specific cell should be drawn.
struct DrawingData {
    enum State {
        NONE,    //!< Do not draw this cell (for example, if blocked from all sides)
        REDUCED, //!< Draw cell with the wall reduced (lowered or transparent); ONLY FOR DIMETRIC MODE
        FULL,    //!< Draw cell with the wall in full height and opacity
    };

    State state; // TODO -> drawSuggestion

    // TODO: information about LoS blocking
    // TODO: light pickup locations
};

//! Cell model extended with runtime optimization data.
class CellModelExt : public CellModel {
public:
    class ExtensionData {
    public:
        ExtensionData();

        void setRendererMask(std::uint16_t aMask);
        std::uint16_t getRendererMask() const;

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
        DrawingData getDrawingData(float           aCellResolution,
                                   PositionInWorld aCellTopLeft,
                                   PositionInWorld aPointOfView) const;

    private:
        using DrawingDataPredicate = DrawingData (*)(float           aCellResolution,
                                                     PositionInWorld aCellTopLeft,
                                                     PositionInWorld aPointOfView);

        union {
            DrawingDataPredicate     drawingDataPredicate;
            ChunkExtensionInterface* chunkExtension;
        } _pointerStorage;

        bool _holdingExtension = false;
        
        std::uint16_t _rendererMask = 0;
    };

    static_assert(sizeof(ExtensionData) <= 2 * sizeof(void*));

    mutable ExtensionData mutableExtensionData;
};

} // namespace detail
} // namespace gridgoblin
} // namespace jbatnozic
