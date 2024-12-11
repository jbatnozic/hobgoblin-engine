#pragma once

#include <GridWorld/Model/Cell.hpp>
#include <GridWorld/Model/Chunk.hpp>
#include <GridWorld/Model/Chunk_id.hpp>
#include <GridWorld/Model/Chunk_extension.hpp>

#include <GridWorld/Private/Chunk_storage_handler.hpp>

#include <memory>

namespace gridworld {

//! This class contains a number of callbacks which can be overriden to 'bind'
//! a Griddy World to its enclosing application and help them interoperate better.
class Binder {
public:
    //! Virtual destructor.
    virtual ~Binder() = default;

    virtual void onChunkLoaded(ChunkId aChunkId, const Chunk* aChunk) {}

    virtual void onChunkUnloaded(ChunkId aChunkId) {}

    virtual std::unique_ptr<ChunkExtensionInterface> createChunkExtension() {
        return nullptr;
    }
};

} // namespace gridworld
