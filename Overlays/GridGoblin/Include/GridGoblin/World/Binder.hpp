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

    //! Called when a previously requested chunk (either requested explicitly via an ActiveArea, or
    //! for on-demand loading via a getter) is ready to be integrated into the world. This means that
    //! on the next `World::update()` call, this chunk will become available.
    //!
    //! \warning this method can (and usually will be) called from a background thread, AT ANY TIME! Be
    //!          very careful what you put in it. Definitely don't do anything in it that could try to
    //!          edit the world.
    virtual void onChunkReady(ChunkId aChunkId) {}

    //! Called when a Chunk is created for the first time, meaning access to this chunk was requested
    //! but it could not be found in the cache, so a default chunk had to be created.
    //!
    //! \note this method can only be called in response to 3 different situations:
    //!       - the World is being updated (`World::update()` called), and the chunk was requested by a
    //!         change in one of the active areas prior;
    //!       - one of the active areas is changed (`ActiveArea::set*()` called) - sometimes, rarely, the
    //!         chunk is immediately available;
    //!       - the chunk was created on-demand, triggered by one of the World's getters.
    //!       In all three cases, `onChunkCreated` is called from the same thread which triggered any of
    //!       the above conditions.
    virtual void onChunkCreated(ChunkId aChunkId, const Chunk& aChunk) {}

    //! Called when a Chunk is loaded from the cache after it was previously unloaded.
    //!
    //! \note this method can only be called in response to 3 different situations:
    //!       - the World is being updated (`World::update()` called), and the chunk was requested by a
    //!         change in one of the active areas prior;
    //!       - one of the active areas is changed (`ActiveArea::set*()` called) - sometimes, rarely, the
    //!         chunk is immediately available;
    //!       - the chunk was loaded on-demand, triggered by one of the World's getters.
    //!       In all three cases, `onChunkLoaded` is called from the same thread which triggered any of
    //!       the above conditions.
    virtual void onChunkLoaded(ChunkId aChunkId, const Chunk& aChunk) {}

    //! TODO(description)
    //! [called when World::prune]
    virtual void onChunkUnloaded(ChunkId aChunkId) {}

    //! TODO(description)
    virtual std::unique_ptr<ChunkExtensionInterface> createChunkExtension() {
        return nullptr;
    }
};

// TODO: implement proper chunk unload callback structure

} // namespace gridgoblin
} // namespace jbatnozic
