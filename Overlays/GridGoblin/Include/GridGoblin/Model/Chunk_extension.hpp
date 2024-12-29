// Copyright 2024 Jovan Batnozic. Released under MS-PL licence in Serbia.
// See https://github.com/jbatnozic/Hobgoblin?tab=readme-ov-file#licence

#pragma once

#include <GridGoblin/Model/Chunk_id.hpp>

#include <Hobgoblin/HGExcept.hpp>
#include <Hobgoblin/Utility/Stream.hpp>

#include <cstdint>
#include <typeinfo>

namespace jbatnozic {
namespace gridgoblin {

namespace hg = ::jbatnozic::hobgoblin;

class Chunk;

//! Interface for a chunk extension.
//! A chunk extension is used to extend a chunk with arbitrary user data.
class ChunkExtensionInterface {
public:
    virtual ~ChunkExtensionInterface() = default;

    //! This method is called after the extension is created for a loaded or a newly-created chunk.
    //!
    //! \param aChunkId ID of the chunk for which the extension has been created.
    //! \param aChunk   chunk for which the extension has been created.
    //!
    //! \warning be very careful what you do in this callback! At this point, the chunk hasn't been
    //!          integrated into the World and thus isn't accessible through the World instance.
    //!          Consequently, values of its cells other than Floor and Wall (and flags) - for example,
    //!          cell openness - aren't yet calculated when `init` is called. Finally, at this point
    //!          the extension hasn't even yet been attached to its chunk, so `aChunk->getExtension()`
    //!          will return `nullptr`.
    //!
    //! \note this call will immediately be followed by a call to `deserialize()`, if the preferred
    //!       serialization method is not NONE, and if the chunk extension has been serialized prior.
    virtual void init(ChunkId aChunkId, const Chunk& aChunk) {}

    ///////////////////////////////////////////////////////////////////////////
    // MARK: SERIALIZATION                                                   //
    ///////////////////////////////////////////////////////////////////////////

    enum class SerializationMethod {
        NONE,
        BINARY_STREAM
        // More possibly to be added in the future
    };

    virtual SerializationMethod getPreferredSerializationMethod() const {
        return SerializationMethod::NONE;
    }

    //! Serialize the extension.
    //! Corresponds to `SerializationMethod::BINARY_STREAM`.
    virtual void serialize(hg::util::OutputStream& aOStream) const {
        HG_NOT_IMPLEMENTED();
    }

    //! Deserialize the extension.
    //! Corresponds to `SerializationMethod::BINARY_STREAM`.
    virtual void deserialize(hg::util::InputStream& aIStream) {
        HG_NOT_IMPLEMENTED();
    }

    ///////////////////////////////////////////////////////////////////////////
    // MARK: IDENTIFICATION                                                  //
    ///////////////////////////////////////////////////////////////////////////

    //! Returns the unique identifier of the specific extension instance.
    //! \note this method is provided purely for user convenience and will never be called by the
    //!       library, so you can have it return whatever you want (for example, if you're not going
    //!       to use this you can have them all return 0).
    virtual std::int64_t getUniqueIdentifier() const = 0;

    //! Returns the type info of the actual derived type of the extension instance.
    //! \note this method is provided purely for user convenience and will never be
    //!       called by the library.
    virtual const std::type_info& getTypeInfo() const = 0;
};

} // namespace gridgoblin
} // namespace jbatnozic
