#pragma once

#include <Hobgoblin/Common.hpp>

#include <cstdint>
#include <memory> // for std::hash
#include <ostream>
#include <tuple>

#include <fmt/format.h>
#include <fmt/ostream.h>

namespace gridworld {

namespace hg = ::jbatnozic::hobgoblin;

struct ChunkId {
    std::uint16_t x;
    std::uint16_t y;

    ChunkId(std::uint16_t aX = 0, std::uint16_t aY = 0)
        : x{aX}
        , y{aY} {}

    ChunkId(hg::PZInteger aX, hg::PZInteger aY)
        : x{static_cast<std::uint16_t>(aX)}
        , y{static_cast<std::uint16_t>(aY)} {}

    bool operator==(const ChunkId& aOther) const {
        return std::tie(x, y) == std::tie(aOther.x, aOther.y);
    }

    bool operator<(const ChunkId& aOther) const {
        return (y < aOther.y) || ((y == aOther.y) && (x < aOther.x));
    }

    friend std::ostream& operator<<(std::ostream& aOs, ChunkId aChunkId) {
        return aOs << '[' << aChunkId.x << ',' << aChunkId.y << ']';
    }
};

} // namespace gridworld

template <>
struct fmt::formatter<gridworld::ChunkId> : fmt::ostream_formatter {};

template <>
struct std::hash<gridworld::ChunkId> {
    std::size_t operator()(const gridworld::ChunkId& aChunkId) const noexcept {
        const std::int32_t temp = (aChunkId.y << 16) | aChunkId.x;
        return std::hash<std::int32_t>{}(temp);
    }
};
