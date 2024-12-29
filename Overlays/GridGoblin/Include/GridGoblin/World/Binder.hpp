// Copyright 2024 Jovan Batnozic. Released under MS-PL licence in Serbia.
// See https://github.com/jbatnozic/Hobgoblin?tab=readme-ov-file#licence

#pragma once

#include <GridGoblin/Model/Cell.hpp>
#include <GridGoblin/Model/Chunk.hpp>
#include <GridGoblin/Model/Chunk_extension.hpp>
#include <GridGoblin/Model/Chunk_id.hpp>

#include <GridGoblin/Private/Chunk_storage_handler.hpp>

#include <memory>

namespace jbatnozic {
namespace gridgoblin {

//! This class contains a number of callbacks which can be overriden to 'bind'
//! a GridGoblin World to its enclosing application and help them interoperate better.
class Binder {
public:
    //! Virtual destructor.
    virtual ~Binder() = default;

    //! Called when a Chunk is created for the first time, meaning access to this chunk was requested
    //! but it could not be found in the cache, so a default chunk had to be created.
    virtual void onChunkCreated(ChunkId aChunkId, const Chunk& aChunk) {}

    //! Called when a Chunk is loaded from the cache after it was previously unloaded.
    virtual void onChunkLoaded(ChunkId aChunkId, const Chunk& aChunk) {}

    //! TODO(description)
    virtual void onChunkUnloaded(ChunkId aChunkId) {}

    //! TODO(description)
    virtual std::unique_ptr<ChunkExtensionInterface> createChunkExtension() {
        return nullptr;
    }
};

// TODO: implement proper chunk unload callback structure

} // namespace gridgoblin
} // namespace jbatnozic
