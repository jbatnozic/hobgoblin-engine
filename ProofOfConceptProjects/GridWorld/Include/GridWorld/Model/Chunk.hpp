#pragma once

#include <GridWorld/Model/Cell.hpp>
#include <GridWorld/Private/Cell_grid.hpp>

namespace gridworld {

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

    ///////////////////////////////////////////////////////////////////////////
    // EXTENSIONS                                                            //
    ///////////////////////////////////////////////////////////////////////////

    //! TODO
    void setExtension(std::unique_ptr<ChunkExtensionInterface> aChunkExtension);

    //! TODO
    ChunkExtensionInterface* getExtension() const;

    //! TODO
    std::unique_ptr<ChunkExtensionInterface> releaseExtension();

    ///////////////////////////////////////////////////////////////////////////
    // OPERATORS                                                             //
    ///////////////////////////////////////////////////////////////////////////

#if 0 // TODO: Why would chunk equality even be relevant?
    friend bool operator==(const Chunk& aLhs, const Chunk& aRhs) {
        if (aLhs.getCellCountX() != aRhs.getCellCountX() ||
            aLhs.getCellCountY() != aRhs.getCellCountY()) {
            return false;
        }

        for (hg::PZInteger y = 0; y < aLhs.getCellCountY(); y += 1) {
            for (hg::PZInteger x = 0; x < aRhs.getCellCountX(); x += 1) {
                const auto& lhsCell = static_cast<const CellModel&>(aLhs._cells[y][x]);
                const auto& rhsCell = static_cast<const CellModel&>(aRhs._cells[y][x]);
                if (lhsCell != rhsCell) {
                    return false;
                }
            }
        }

        // TBD: take into consideration chunk extensions?

        return true;
    }

    friend bool operator!=(const Chunk& aLhs, const Chunk& aRhs) {
        return !(aLhs == aRhs);
    }
#endif

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

} // namespace gridworld
