// Copyright 2024 Jovan Batnozic. Released under MS-PL licence in Serbia.
// See https://github.com/jbatnozic/Hobgoblin?tab=readme-ov-file#licence

#pragma once

#include <GridGoblin/Spatial/Position_in_world.hpp>

#include <optional>

namespace jbatnozic {
namespace gridgoblin {

//! Virtual interface for objects which can be used to calculate which points in a GridGoblin World
//! are visible from some arbitrary point of view, and which are blocked by walls of cells.
//! This information can then be used for purposes like determining which walls should be lowered
//! during rendering, rendering of shadows for dynamic lighting, and others.
class VisibilityProvider {
public:
    virtual ~VisibilityProvider() = default;

    virtual std::optional<bool> testVisibilityAt(PositionInWorld aPos) const = 0;
};

} // namespace gridgoblin
} // namespace jbatnozic
