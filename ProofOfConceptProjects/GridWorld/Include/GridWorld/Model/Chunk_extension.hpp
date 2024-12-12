#pragma once

#include <GridWorld/Model/Chunk_id.hpp>

#include <cstdint>
#include <typeinfo>
#include <vector>

namespace gridworld {

class Chunk;

class ChunkExtensionInterface {
public:
    virtual ~ChunkExtensionInterface() = default;

    virtual const std::type_info& getTypeInfo() const = 0;

    virtual std::int64_t getUniqueIdentifier() const = 0;

    // called after filling brand new chunk
    virtual void init(ChunkId aChunkId, const Chunk& aChunk);

    enum class SerializationMethod {
        BINARY
        // More possibly to be added in the future
    };

    virtual SerializationMethod getPreferredSerializationMethod() const = 0;

    virtual void serializeToBinary(std::vector<std::uint8_t>& aBuffer) const = 0;

    virtual void deserializeFromBinary(const std::vector<std::uint8_t>& aBuffer) = 0;
};

} // namespace gridworld
