// Copyright 2024 Jovan Batnozic. Released under MS-PL licence in Serbia.
// See https://github.com/jbatnozic/Hobgoblin?tab=readme-ov-file#licence

#pragma once

#include <GridGoblin/Spatial/View_position.hpp>
#include <GridGoblin/Spatial/World_position.hpp>

namespace jbatnozic {
namespace gridgoblin {
namespace dimetric {

ViewPosition  ToViewPosition(WorldPosition aWorldPosition);
WorldPosition ToWorldPosition(ViewPosition aViewPosition);

} // namespace dimetric
} // namespace gridgoblin
} // namespace jbatnozic
