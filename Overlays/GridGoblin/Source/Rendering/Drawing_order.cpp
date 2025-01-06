// Copyright 2024 Jovan Batnozic. Released under MS-PL licence in Serbia.
// See https://github.com/jbatnozic/Hobgoblin?tab=readme-ov-file#licence

#include <GridGoblin/Rendering/Drawing_order.hpp>

#include <array>

namespace jbatnozic {
namespace gridgoblin {
namespace dimetric {

#define USE_HOBGOBLIN_COORDINATE_SYSTEM 1
#define USE_LOOKUP_TABLE                1

#if USE_LOOKUP_TABLE
namespace {
std::array<int, 16> CHECK_DRAWING_ORDER_LOOKUP_TABLE = {DRAW_LHS_FIRST,
                                                        DRAW_LHS_FIRST,
                                                        DRAW_RHS_FIRST,
                                                        DRAW_RHS_FIRST,
                                                        DRAW_LHS_FIRST,
                                                        DRAW_LHS_FIRST,
                                                        DOES_NOT_MATTER,
                                                        DOES_NOT_MATTER,
                                                        DRAW_RHS_FIRST,
                                                        DOES_NOT_MATTER,
                                                        DRAW_RHS_FIRST,
                                                        DOES_NOT_MATTER,
                                                        DRAW_RHS_FIRST,
                                                        DOES_NOT_MATTER,
                                                        DOES_NOT_MATTER,
                                                        DOES_NOT_MATTER};
} // namespace
#endif

// clang-format off
int CheckDrawingOrder(const SpatialInfo& aLhs, const SpatialInfo& aRhs) {
    if (aLhs.getLayer() < aRhs.getLayer()) {
        return DRAW_LHS_FIRST;
    }
    if (aRhs.getLayer() < aLhs.getLayer()) {
        return DRAW_RHS_FIRST;
    }

    if (aLhs.getBoundingBox().overlaps(aRhs.getBoundingBox())) {
        const auto lhsCenter = aLhs.getCenter();
        const auto rhsCenter = aRhs.getCenter();
        return ((lhsCenter->x - lhsCenter->y) > (rhsCenter->x - rhsCenter->y)) ? DRAW_LHS_FIRST
                                                                               : DRAW_RHS_FIRST;
    }

    // Algorithm reference: https://stackoverflow.com/a/69088148

    const auto& lhsBbox = aLhs.getBoundingBox();
    const auto& rhsBbox = aRhs.getBoundingBox();

#if !USE_HOBGOBLIN_COORDINATE_SYSTEM
    // Use the coordinates as-is

    const bool cmpX1 = ((lhsBbox.x + lhsBbox.w) <= rhsBbox.x);
    const bool cmpX2 = ((rhsBbox.x + rhsBbox.w) <= lhsBbox.x);
    const bool cmpY1 = ((lhsBbox.y + lhsBbox.h) <= rhsBbox.y);
    const bool cmpY2 = ((rhsBbox.y + rhsBbox.h) <= lhsBbox.y);
#else
    // Flip the Y axix

    const bool cmpX1 = (( lhsBbox.x + lhsBbox.w) <= rhsBbox.x);
    const bool cmpX2 = (( rhsBbox.x + rhsBbox.w) <= lhsBbox.x);
    const bool cmpY1 = ((-lhsBbox.y            ) <= (-rhsBbox.y - rhsBbox.h));
    const bool cmpY2 = ((-rhsBbox.y            ) <= (-lhsBbox.y - rhsBbox.h));
#endif

#if !USE_LOOKUP_TABLE
    if ((cmpX1 && cmpY2) || (cmpX2 && cmpY1)) return DOES_NOT_MATTER;
    if  (cmpX1 || cmpY1)                      return DRAW_RHS_FIRST;
    /* if (cmpX2 || cmpY2) */                 return DRAW_LHS_FIRST;
#else
    const auto cmpX1u = static_cast<unsigned>(cmpX1) << 3;
    const auto cmpX2u = static_cast<unsigned>(cmpX2) << 2;
    const auto cmpY1u = static_cast<unsigned>(cmpY1) << 1;
    const auto cmpY2u = static_cast<unsigned>(cmpY2) << 0;

    return CHECK_DRAWING_ORDER_LOOKUP_TABLE[cmpX1u | cmpX2u | cmpY1u | cmpY2u];
#endif
}
// clang-format on

} // namespace dimetric
} // namespace gridgoblin
} // namespace jbatnozic
