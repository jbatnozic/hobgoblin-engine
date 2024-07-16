#pragma once

#include <cstdint>
#include <tuple>

namespace gridworld {

namespace detail {

struct ChunkId {
    std::uint16_t x;
    std::uint16_t y;

    bool operator==(const ChunkId& aOther) const {
        return std::tie(x, y) == std::tie(aOther.x, aOther.y);
    }
};

} // namespace detail

} // namespace gridworld
