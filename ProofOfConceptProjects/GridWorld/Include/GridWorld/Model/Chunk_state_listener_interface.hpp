#pragma once

#include <GridWorld/Model/Chunk_extension.hpp>
#include <GridWorld/World/Chunk_id.hpp>

namespace gridworld {

class ChunkStateListenerInterface {
public:
    virtual ~ChunkStateListenerInterface() = default;

    virtual void onChunkLoaded(ChunkId aChunkId, ChunkExtensionInterface* aExtension)   = 0;
    virtual void onChunkUnloaded(ChunkId aChunkId, ChunkExtensionInterface* aExtension) = 0;
};

} // namespace gridworld
