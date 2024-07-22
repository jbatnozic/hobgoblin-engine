#pragma once

#include <GridWorld/Model/Chunk_id.hpp>
#include <GridWorld/Model/Chunk_extension.hpp>

namespace gridworld {

class ChunkStateListenerInterface {
public:
    virtual ~ChunkStateListenerInterface() = default;

    virtual void onChunkLoaded(detail::ChunkId aChunkId, ChunkExtensionInterface* aExtension) = 0;
    virtual void onChunkUnloaded(detail::ChunkId aChunkId, ChunkExtensionInterface* aExtension) = 0;
};

} // namespace gridworld
