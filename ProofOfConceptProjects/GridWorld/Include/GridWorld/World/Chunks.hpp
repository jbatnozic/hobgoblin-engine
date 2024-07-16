#pragma once

#include <cstdint>

namespace gridworld {

struct ChunkId {
    std::uint16_t x;
    std::uint16_t y;
};

struct Chunk {}; // TODO(temp)

class ChunkDiskIo {
public:
    void  write();
    Chunk read();
};

} // namespace gridworld
