#pragma once

#include <cstdint>
#include <memory> // for std::hash
#include <tuple>

namespace gridworld {

namespace detail {

// TODO: shouldn't be in detail::
struct ChunkId {
    std::uint16_t x;
    std::uint16_t y;

    bool operator==(const ChunkId& aOther) const {
        return std::tie(x, y) == std::tie(aOther.x, aOther.y);
    }
};

} // namespace detail

} // namespace gridworld

#define CHUNK_ID gridworld::detail::ChunkId

template <>
struct std::hash<CHUNK_ID> {
    std::size_t operator()(const CHUNK_ID& aChunkId) const noexcept {
        const std::int32_t temp = (aChunkId.y << 16) | aChunkId.x;
        return std::hash<std::int32_t>{}(temp);
    }
};

#undef CHUNK_ID
