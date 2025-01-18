// Copyright 2024 Jovan Batnozic. Released under MS-PL licence in Serbia.
// See https://github.com/jbatnozic/Hobgoblin?tab=readme-ov-file#licence

#pragma once

#include <GridGoblin/Model/Cell_model.hpp>
#include <GridGoblin/Private/Cell_grid.hpp>
#include <GridGoblin/Private/Cell_model_ext.hpp>

namespace jbatnozic {
namespace gridgoblin {

namespace hg = jbatnozic::hobgoblin;

namespace detail {
class ChunkStorageHandler;
} // namespace detail

class ChunkExtensionInterface;

//! \note definitions of some methods are intentionally left right here in the header,
//!       to help the compiler inline and optimize calls to them.
class Chunk {
public:
    // clang-format off
    // Default construction (constructs an empty chunk)
    Chunk()                        = default;
    // No copying
    Chunk(const Chunk&)            = delete;
    Chunk& operator=(const Chunk&) = delete;
    // Moving is OK
    Chunk(Chunk&&)                 = default;
    Chunk& operator=(Chunk&&)      = default;
    // clang-format on

    Chunk(hg::PZInteger aWidth, hg::PZInteger aHeight);
    ~Chunk();

    bool isEmpty() const {
        return _cells.getWidth() == 0;
    }

    void makeEmpty();

    ///////////////////////////////////////////////////////////////////////////
    // CELLS                                                                 //
    ///////////////////////////////////////////////////////////////////////////

    hg::PZInteger getCellCountX() const {
        return _cells.getWidth();
    }

    hg::PZInteger getCellCountY() const {
        return _cells.getHeight();
    }

    const CellModel& getCellAtUnchecked(hg::PZInteger aX, hg::PZInteger aY) const {
        return _cells[aY][aX];
    }

    CellModel& getCellAtUnchecked(hg::PZInteger aX, hg::PZInteger aY) {
        return _cells[aY][aX];
    }

    //! Set all the cells in the chunk to the given value, unless the chunk is empty. Utility method.
    void setAll(const CellModel& aCell);

    ///////////////////////////////////////////////////////////////////////////
    // EXTENSIONS                                                            //
    ///////////////////////////////////////////////////////////////////////////

    //! Set a new extension for the chunk, destroying the old one (if any).
    void setExtension(std::unique_ptr<ChunkExtensionInterface> aChunkExtension);

    //! Return a mutable but non-owning pointer to the chunk's extension (if any; returns
    //! `nullptr` if no extension).
    ChunkExtensionInterface* getExtension() const;

    //! Detach the chunk's extension and return it owned by a `std::unique_ptr` (if any; returns
    //! `nullptr` if no extension).
    std::unique_ptr<ChunkExtensionInterface> releaseExtension();

private:
    friend class detail::ChunkStorageHandler;

    detail::CellGrid _cells;

    detail::CellModelExt& _getCellExtAtUnchecked(hg::PZInteger aX, hg::PZInteger aY) {
        return _cells[aY][aX];
    }

    const detail::CellModelExt& _getCellExtAtUnchecked(hg::PZInteger aX, hg::PZInteger aY) const {
        return _cells[aY][aX];
    }

    void _storeChunkExtensionPointer(ChunkExtensionInterface* aChunkExtensionPointer);
    ChunkExtensionInterface* _loadChunkExtensionPointer() const;
};

namespace detail {
bool AreCellsEqual(const Chunk& aChunk1, const Chunk& aChunk2);
} // namespace detail

} // namespace gridgoblin
} // namespace jbatnozic
