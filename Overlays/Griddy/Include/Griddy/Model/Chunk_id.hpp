// Copyright 2024 Jovan Batnozic. Released under MS-PL licence in Serbia.
// See https://github.com/jbatnozic/Hobgoblin?tab=readme-ov-file#licence

#pragma once

#include <Hobgoblin/Common.hpp>

#include <cstdint>
#include <memory> // for std::hash
#include <ostream>
#include <tuple>

#include <fmt/format.h>
#include <fmt/ostream.h>

namespace jbatnozic {
namespace griddy {

namespace hg = ::jbatnozic::hobgoblin;

//! Identifies a single chunk by its X and Y position in the world grid.
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

} // namespace griddy
} // namespace jbatnozic

template <>
struct fmt::formatter<jbatnozic::griddy::ChunkId> : fmt::ostream_formatter {};

template <>
struct std::hash<jbatnozic::griddy::ChunkId> {
    std::size_t operator()(const jbatnozic::griddy::ChunkId& aChunkId) const noexcept {
        const std::int32_t temp = (aChunkId.y << 16) | aChunkId.x;
        return std::hash<std::int32_t>{}(temp);
    }
};
