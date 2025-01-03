// Copyright 2024 Jovan Batnozic. Released under MS-PL licence in Serbia.
// See https://github.com/jbatnozic/Hobgoblin?tab=readme-ov-file#licence

#include <GridGoblin/Spatial/Position_conversions.hpp>

namespace jbatnozic {
namespace gridgoblin {
namespace dimetric {

ViewPosition ToViewPosition(WorldPosition aWorldPosition) {
    ViewPosition result;
    // clang-format off
    result->x =  aWorldPosition->y + aWorldPosition->x;
    result->y = (aWorldPosition->y - aWorldPosition->x) / 2.f;
    // clang-format on
    return result;
}

WorldPosition ToWorldPosition(ViewPosition aViewPosition) {
    WorldPosition result;
    result->x = aViewPosition->x / 2.f - aViewPosition->y;
    result->y = aViewPosition->x / 2.f + aViewPosition->y;
    return result;
}

} // namespace dimetric
} // namespace gridgoblin
} // namespace jbatnozic
