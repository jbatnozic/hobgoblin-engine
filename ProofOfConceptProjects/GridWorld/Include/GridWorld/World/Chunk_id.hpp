#pragma once

#include <cstdint>
#include <memory> // for std::hash
#include <tuple>

namespace gridworld {

struct ChunkId {
    std::uint16_t x;
    std::uint16_t y;

    bool operator==(const ChunkId& aOther) const {
        return std::tie(x, y) == std::tie(aOther.x, aOther.y);
    }
};

} // namespace gridworld

template <>
struct std::hash<gridworld::ChunkId> {
    std::size_t operator()(const gridworld::ChunkId& aChunkId) const noexcept {
        const std::int32_t temp = (aChunkId.y << 16) | aChunkId.x;
        return std::hash<std::int32_t>{}(temp);
    }
};
