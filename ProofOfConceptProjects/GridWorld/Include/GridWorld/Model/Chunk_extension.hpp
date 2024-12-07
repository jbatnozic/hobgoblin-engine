#pragma once

namespace gridworld {

class ChunkExtensionInterface {
public:
    virtual ~ChunkExtensionInterface() = default;

    // init(ChunkId, const Chunk&)

    // get type ID, get int64 ID

    // methods to serialize/deserialize ; to string/binary_buffer/json

    // method to get preferred way to serialize
};

} // namespace gridworld
