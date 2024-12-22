// Copyright 2024 Jovan Batnozic. Released under MS-PL licence in Serbia.
// See https://github.com/jbatnozic/Hobgoblin?tab=readme-ov-file#licence

#pragma once

#include <Griddy/Model/Cell.hpp>
#include <Griddy/Model/Chunk.hpp>
#include <Griddy/Model/Chunk_id.hpp>
#include <Griddy/Model/Chunk_extension.hpp>

#include <Griddy/Private/Chunk_storage_handler.hpp>

#include <memory>

namespace jbatnozic {
namespace griddy {

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

} // namespace griddy
}
