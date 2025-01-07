// Copyright 2024 Jovan Batnozic. Released under MS-PL licence in Serbia.
// See https://github.com/jbatnozic/Hobgoblin?tab=readme-ov-file#licence

#pragma once

#include <GridGoblin/Spatial/Position_in_view.hpp>
#include <GridGoblin/Spatial/Position_in_world.hpp>

namespace jbatnozic {
namespace gridgoblin {
namespace dimetric {

PositionInView  ToPositionInView(PositionInWorld aPosInWorld);
PositionInWorld ToPositionInWorld(PositionInView aPosInView);

} // namespace dimetric
} // namespace gridgoblin
} // namespace jbatnozic
