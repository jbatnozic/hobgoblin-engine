// Copyright 2024 Jovan Batnozic. Released under MS-PL licence in Serbia.
// See https://github.com/jbatnozic/Hobgoblin?tab=readme-ov-file#licence

#pragma once

#include <Hobgoblin/Common.hpp>

#include <GridGoblin/Model/Chunk_id.hpp>

#include <functional>
#include <vector>

namespace jbatnozic {
namespace gridgoblin {

namespace detail {
class ChunkStorageHandler;
} // namespace detail

namespace hg = jbatnozic::hobgoblin;

//! This class is closely related to a GridGoblin World and its chunks.
//!
//! An active area represents an area of the world where 'stuff is happening' right now, or it's likely
//! to be so in the near future. In game terms, think of this as areas around the player character(s),
//! or around important NPCs which do something even when the player is not around.
//!
//! The role of an active area is twofold:
//! 1) chunks inside of an active area will be loaded by the world in the background by a worker thread;
//! 2) chunks inside of an active area will never be unloaded by the world.
//!
//! A note relevant to point 1): any interaction with a chunk or its cells, even just attempting to read
//! their values, will force the world to load the chunk, if it isn't loaded already, and block the
//! calling thread until it does so. Thus it is preferrable for one or more active areas to be large
//! enough and placed in such positions that all (or almost all) of the chunks required by the game will
//! essentially be loaded proactively, ahead of time.
class ActiveArea {
public:
    //! Destructor.
    ~ActiveArea();

    //! Sets the active area to contain no chunks.
    void setToNone();

    //! Sets the active area to an area defined by a central chunk and a square around it comprised of
    //! a number of rings. The 1st ring is the 8 chunks around the central chunks, the 2nd ring is the
    //! 16 chunks around the 1st ring, and so on (the ASCII art below serves as an illustration).
    //!
    //! 2 2 2 2 2
    //! 2 1 1 1 2
    //! 2 1 0 1 2
    //! 2 1 1 1 2
    //! 2 2 2 2 2
    //!
    //! \param aCentralChunk ID of the central chunks.
    //! \param aRingCount number of rings. If 0, only the central chunk will be included. If 1, the
    //!                   1st ring will be included. If 2, the 1st and 2nd rings will be included, and
    //!                   so on.
    //! \param aGetLoadPriority functor which will be 'asked' for the loading prioriy of each of the
    //!                         chunks in the area (except those that are outside of the world). If
    //!                         ommitted, the default scheme will be used (0 for the central chunks,
    //!                         N for chunks of Nth ring).
    //!
    //! \note chunks that are out of the bounds of the world will be handled properly and without
    //!       crashing, even if the central chunk is out of bounds.
    void setToChunkRingSquare(ChunkId                                      aCentralChunk,
                              hg::PZInteger                                aRingCount,
                              const std::function<hg::PZInteger(ChunkId)>& aGetLoadPriority = nullptr);

    //! Sets the active area to an area defined by a central chunk and a diamond shape around it
    //! comprised of a number of rings. The 1st ring is the 4 chunks around the central chunks, the 2nd
    //! ring is the 8 chunks around the 1st right, and so on (the ASCII art below serves as an 
    //! illustration).
    //!
    //!     2    
    //!   2 1 2  
    //! 2 1 0 1 2
    //!   2 1 2  
    //!     2   
    //!
    //! \param aCentralChunk ID of the central chunks.
    //! \param aRingCount number of rings. If 0, only the central chunk will be included. If 1, the
    //!                   1st ring will be included. If 2, the 1st and 2nd rings will be included, and
    //!                   so on.
    //! \param aGetLoadPriority functor which will be 'asked' for the loading prioriy of each of the
    //!                         chunks in the area (except those that are outside of the world). If
    //!                         ommitted, the default scheme will be used (0 for the central chunks,
    //!                         N for chunks of Nth ring).
    //!
    //! \note chunks that are out of the bounds of the world will be handled properly and without
    //!       crashing, even if the central chunk is out of bounds.
    void setToChunkRingDiamond(ChunkId                                      aCentralChunk,
                               hg::PZInteger                                aRingCount,
                               const std::function<hg::PZInteger(ChunkId)>& aGetLoadPriority = nullptr);

    //! Sets the active area to the provided list of chunks. If the function object to determine
    //! priority of chunks is not provided, they will be loaded in order off appearance in the list.
    void setToChunkList(std::vector<ChunkId>                         aChunkList,
                        const std::function<hg::PZInteger(ChunkId)>& aGetLoadPriority = nullptr);

    //! Returns a list of IDs of chunks that are contained in this active area.
    const std::vector<ChunkId>& getChunkList() const;

private:
    friend class detail::ChunkStorageHandler;
    explicit ActiveArea(hg::NeverNull<detail::ChunkStorageHandler*> aStorageHandler)
        : _storageHandler{aStorageHandler} {}

    hg::NeverNull<detail::ChunkStorageHandler*> _storageHandler;
    std::vector<ChunkId>                        _chunkList;
};

namespace detail {
struct ChunkUsageChange {
    ChunkId       chunkId;
    int           usageDelta;   //!< Value must be between  -1 and 1
    hg::PZInteger loadPriority; //!< Relevant only if usageDelta > 0
};

//! Sets `aCurrentChunkList` to `aNewChunkList`.
//! \returns changes between the two lists.
std::vector<ChunkUsageChange> UpdateChunkList(
    std::vector<ChunkId>&                        aCurrentChunkList,
    std::vector<ChunkId>                         aNewChunkList,
    const std::function<hg::PZInteger(ChunkId)>& aGetLoadPriority = nullptr);
} // namespace detail

} // namespace gridgoblin
}
