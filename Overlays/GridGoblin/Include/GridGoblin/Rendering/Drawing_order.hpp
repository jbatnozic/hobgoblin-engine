// Copyright 2024 Jovan Batnozic. Released under MS-PL licence in Serbia.
// See https://github.com/jbatnozic/Hobgoblin?tab=readme-ov-file#licence

#pragma once

#include <GridGoblin/Spatial/Spatial_info.hpp>

namespace jbatnozic {
namespace gridgoblin {
namespace dimetric {

// clang-format off
enum CheckDrawingOrderResult {
    DRAW_LHS_FIRST  = -1,
    DOES_NOT_MATTER =  0,
    DRAW_RHS_FIRST  = +1
};
// clang-format on

//! Check whether `aLhs` should be drawn before `aRhs` as per the dimetric drawing order.
//! Return value reference:
//! - DRAW_LHS_FIRST  : `aLhs` should be drawn before `aRhs`.
//! - DOES_NOT_MATTER : it doesn't matter which one is drawn first.
//! - DRAW_RHS_FIRST  : `aRhs` should be drawn before `aLhs`.
//!
//! Note: if the bounding boxes overlap, there is technically no correct answer, so a simple
//!       check is done to see which centre is lower on the screen.
int CheckDrawingOrder(const SpatialInfo& aLhs, const SpatialInfo& aRhs);

} // namespace dimetric
} // namespace gridgoblin
} // namespace jbatnozic
