// Copyright 2024 Jovan Batnozic. Released under MS-PL licence in Serbia.
// See https://github.com/jbatnozic/Hobgoblin?tab=readme-ov-file#licence

#pragma once

#include <Griddy/Model/Chunk_id.hpp>

#include <Hobgoblin/HGExcept.hpp>
#include <Hobgoblin/Utility/Stream.hpp>

#include <cstdint>
#include <typeinfo>

namespace griddy {

namespace hg = ::jbatnozic::hobgoblin;

class Chunk;

class ChunkExtensionInterface {
public:
    virtual ~ChunkExtensionInterface() = default;

    // called after filling brand new chunk
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

    virtual std::int64_t getUniqueIdentifier() const = 0;

    virtual const std::type_info& getTypeInfo() const = 0;
};

} // namespace griddy
