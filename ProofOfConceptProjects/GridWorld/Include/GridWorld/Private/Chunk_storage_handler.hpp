#pragma once

#include <Hobgoblin/Common.hpp>

#include <GridWorld/Model/Chunk.hpp>
#include <GridWorld/World/Chunk_id.hpp>
#include <GridWorld/Private/Chunk_spooler_interface.hpp>

#include <Hobgoblin/Utility/Grids.hpp>

#include <memory>
#include <unordered_map>

namespace gridworld {

namespace hg = jbatnozic::hobgoblin;

namespace detail {

// TODO: move definitions somewhere else

#if defined(__GNUC__) && !defined(__clang__)
#define HG_LIKELY_CONDITION(...)   __builtin_expect(!!(__VA_ARGS__), 1)
#define HG_UNLIKELY_CONDITION(...) __builtin_expect(!!(__VA_ARGS__), 0)
#else
#define HG_LIKELY_CONDITION(...)   __VA_ARGS__
#define HG_UNLIKELY_CONDITION(...) __VA_ARGS__
#endif

#define HG_LIKELY_BRANCH   [[likely]]
#define HG_UNLIKELY_BRANCH [[unlikely]]

HG_DECLARE_TAG_TYPE(LOAD_IF_MISSING);

//! This class handles the storage of chunks both in RAM and in on-disk caches.
class ChunkStorageHandler {
private:
    using Self = ChunkStorageHandler;

public:
    ChunkStorageHandler(ChunkSpoolerInterface& aChunkSpooler,
                        hg::PZInteger          aChunkWidth,
                        hg::PZInteger          aChunkHeight);

    //! Returns the number of chunks along the X axis.
    hg::PZInteger getChunkCountX() const {
        return _chunks.getWidth();
    }

    //! Returns the number of chunks along the Y axis.
    hg::PZInteger getChunkCountY() const {
        return _chunks.getHeight();
    }

    //! Returns a mutable reference to the cell at (aX, aY) WITHOUT CHECKING BOUNDS.
    //! The containing chunk will be loaded if it isn't already, and will remain loaded until
    //! the next call to `prune()`.
    CellModelExt& getCellAtUnchecked(hg::PZInteger aX, hg::PZInteger aY, LOAD_IF_MISSING_Tag) {
        const auto chunkX = aX / _chunkWidth;
        const auto chunkY = aY / _chunkHeight;

        auto& chunk = _chunks[chunkY][chunkX];
        if (HG_LIKELY_CONDITION(chunk != nullptr)) {
            HG_LIKELY_BRANCH;
            return chunk->getCellAtUnchecked(aX % _chunkWidth, aY % _chunkHeight);
        } else {
            HG_UNLIKELY_BRANCH;
            _loadChunk(chunkX, chunkY);
            return chunk->getCellAtUnchecked(aX % _chunkWidth, aY % _chunkHeight);
        }
    }

    //! Returns a const reference to the cell at (aX, aY) WITHOUT CHECKING BOUNDS.
    //! The containing chunk will be loaded if it isn't already, and will remain loaded until
    //! the next call to `prune()`.
    const CellModelExt& getCellAtUnchecked(hg::PZInteger aX,
                                           hg::PZInteger aY,
                                           LOAD_IF_MISSING_Tag) const {
        return const_cast<Self*>(this)->getCellAtUnchecked(aX, aY, LOAD_IF_MISSING);
    }

    //! Returns a mutable pointer to the cell at (aX, aY) WITHOUT CHECKING BOUNDS.
    //! If the containing chunk is already loaded, it will remain loaded until the next call
    //! to `prune()`. Otherwise, `nullptr` will be returned.
    CellModelExt* getCellAtUnchecked(hg::PZInteger aX, hg::PZInteger aY) {
        const auto chunkX = aX / _chunkWidth;
        const auto chunkY = aY / _chunkHeight;

        auto& chunk = _chunks[chunkY][chunkX];
        if (HG_LIKELY_CONDITION(chunk != nullptr)) {
            HG_LIKELY_BRANCH;
            return &(chunk->getCellAtUnchecked(aX % _chunkWidth, aY % _chunkHeight));
        } else {
            HG_UNLIKELY_BRANCH;
            return nullptr;
        }
    }

    //! Returns a const pointer to the cell at (aX, aY) WITHOUT CHECKING BOUNDS.
    //! If the containing chunk is already loaded, it will remain loaded until the next call
    //! to `prune()`. Otherwise, `nullptr` will be returned.
    const CellModelExt* getCellAtUnchecked(hg::PZInteger aX, hg::PZInteger aY) const {
        return const_cast<Self*>(this)->getCellAtUnchecked(aX, aY);
    }

    void prune();

private:
    ChunkSpoolerInterface& _chunkSpooler;

    mutable hg::util::RowMajorGrid<std::unique_ptr<Chunk>> _chunks; // TODO: make Chunk nullable and ditch the pointer
    // clang-format off
    mutable std::unordered_map<ChunkId, 
                               std::shared_ptr<ChunkSpoolerInterface::RequestHandleInterface>>
        _chunkRequestHandles;
    // clang-format on

    struct ChunkControlBlock {
        // PZInteger usageCount
        // std::shared_ptr<ChunkSpoolerInterface::RequestHandleInterface> request handle
    };
    // TODO: active areas

    hg::PZInteger _chunkWidth;
    hg::PZInteger _chunkHeight;

    /*
        TODO:
            pause spooler
            load chunk
            unpause

        ALSO:
            create (based on defaults) if missing
    */
    void _loadChunk(hg::PZInteger aChunkX, hg::PZInteger aChunkY);
};

} // namespace detail

} // namespace gridworld
