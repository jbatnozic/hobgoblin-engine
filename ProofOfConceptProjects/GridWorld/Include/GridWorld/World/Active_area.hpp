#pragma once

#include <Hobgoblin/Common.hpp>

#include <GridWorld/Model/Chunk_id.hpp>

#include <functional>
#include <vector>

namespace gridworld {

namespace detail {
class ChunkStorageHandler;
} // namespace detail

namespace hg = jbatnozic::hobgoblin;

//! TODO(add description)
class ActiveArea {
public:
    //! TODO(add description)
    ~ActiveArea();

    //! TODO(add description)
    void setToNone();

    //! TODO(add description)
    //! 2 2 2 2 2
    //! 2 1 1 1 2
    //! 2 1 0 1 2
    //! 2 1 1 1 2
    //! 2 2 2 2 2
    void setToChunkRingSquare(ChunkId                                      aCentralChunk,
                              hg::PZInteger                                aRingCount,
                              const std::function<hg::PZInteger(ChunkId)>& aGetLoadPriority = nullptr);

    //! TODO(add description)
    //!     2    
    //!   2 1 2  
    //! 2 1 0 1 2
    //!   2 1 2  
    //!     2    
    void setToChunkRingDiamond(ChunkId                                      aCentralChunk,
                               hg::PZInteger                                aRingCount,
                               const std::function<hg::PZInteger(ChunkId)>& aGetLoadPriority = nullptr);

    //! Sets the active area to the provided list of chunks. If the function object to determine
    //! priority of chunks is not provided, they will be loaded in order off appearance in the list.
    void setToChunkList(std::vector<ChunkId>                         aChunkList,
                        const std::function<hg::PZInteger(ChunkId)>& aGetLoadPriority = nullptr);

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

} // namespace gridworld
