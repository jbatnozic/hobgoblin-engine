
#include <GridWorld/Spatial_info.hpp>

#include <array>
#include <cstdint>

namespace gridworld {

SpatialInfo SpatialInfo::fromCentreAndSize(WorldPosition      aCentre,
                                           hg::math::Vector2f aSize,
                                           Shape              aShape) {
    SpatialInfo result;
    result.centre = aCentre;
    result.setSizeMaintainingCentre(aSize);
    return result;
}

SpatialInfo SpatialInfo::fromTopLeftAndSize(WorldPosition      aTopLeft,
                                            hg::math::Vector2f aSize,
                                            Shape              aShape) {
    SpatialInfo result;
    result.bbox.x = aTopLeft->x;
    result.bbox.y = aTopLeft->y;
    result.setSize(aSize);
    return result;
}

#define IS_BEHIND_USE_HOBGOBLIN_COORDINATE_SYSTEM 1
#define IS_BEHIND_USE_LOOKUP_TABLE                1

namespace {
#if IS_BEHIND_USE_LOOKUP_TABLE
std::array<int, 16> IS_BEHIND_RESULT_LOOKUP_TABLE =
    {1, 1, -1, -1, 1, 1, 0, 0, -1, 0, -1, 0, -1, 0, 0, 0};
#endif

constexpr float Half(float aF) {
    return aF / 2.f;
}
} // namespace

void SpatialInfo::setCentre(WorldPosition aPoint) {
    centre->x = aPoint->x;
    centre->y = aPoint->y;

    bbox.x = aPoint->x - Half(bbox.w);
    bbox.y = aPoint->y - Half(bbox.h);
}

void SpatialInfo::setTopLeft(WorldPosition aPoint) {
    bbox.x = aPoint->x;
    bbox.y = aPoint->y;

    centre->x = aPoint->x + Half(bbox.w);
    centre->y = aPoint->y + Half(bbox.h);
}

void SpatialInfo::setSize(hg::math::Vector2f aSize) {
    bbox.w = aSize.x;
    bbox.h = aSize.y;

    centre->x = bbox.x + Half(aSize.x);
    centre->y = bbox.y + Half(aSize.y);
}

void SpatialInfo::setSizeMaintainingCentre(hg::math::Vector2f aSize) {
    bbox.w = aSize.x;
    bbox.h = aSize.y;

    bbox.x = centre->x - Half(aSize.x);
    bbox.y = centre->y - Half(aSize.y);
}

WorldPosition SpatialInfo::getCentre() const {
    return centre;
}

WorldPosition SpatialInfo::getTopLeft() const {
    return WorldPosition{
        {bbox.getLeft(), bbox.getTop()}
    };
}

hg::math::Vector2f SpatialInfo::getSize() const {
    return {bbox.w, bbox.h};
}

// clang-format off
SpatialInfo::CheckDrawingOrderResult SpatialInfo::checkDimetricDrawingOrder(
    const SpatialInfo& aOther) const {
    if (overlaps(aOther)) {
        return ((centre->x - centre->y) > (aOther.centre->x - aOther.centre->y)) ? DRAW_THIS_FIRST
                                                                                 : DRAW_OTHER_FIRST;
    }

    // Algorithm reference: https://stackoverflow.com/a/69088148

#if !IS_BEHIND_USE_HOBGOBLIN_COORDINATE_SYSTEM
    // Use the coordinates as-is

    const bool cmpX1 = ((bbox.x        +        bbox.w) <= aOther.bbox.x);
    const bool cmpX2 = ((aOther.bbox.x + aOther.bbox.w) <= bbox.x);
    const bool cmpY1 = ((bbox.y        +        bbox.h) <= aOther.bbox.y);
    const bool cmpY2 = ((aOther.bbox.y + aOther.bbox.h) <= bbox.y);
#else
    // Flip the Y axix

    const bool cmpX1 = ((bbox.x        +        bbox.w) <= aOther.bbox.x);
    const bool cmpX2 = ((aOther.bbox.x + aOther.bbox.w) <= bbox.x);
    const bool cmpY1 = ((-bbox.y                      ) <= (-aOther.bbox.y - aOther.bbox.h));
    const bool cmpY2 = ((-aOther.bbox.y               ) <= (-bbox.y - aOther.bbox.h));
#endif

#if !IS_BEHIND_USE_LOOKUP_TABLE
    if ((cmpX1 && cmpY2) || (cmpX2 && cmpY1)) return (CheckDrawingOrderResult) 0;
    if (cmpX1 || cmpY1)                       return (CheckDrawingOrderResult)-1;
    /*if (cmpX2 || cmpY2)*/                   return (CheckDrawingOrderResult) 1;
#else
    const auto cmpX1u = static_cast<unsigned>(cmpX1) << 3;
    const auto cmpX2u = static_cast<unsigned>(cmpX2) << 2;
    const auto cmpY1u = static_cast<unsigned>(cmpY1) << 1;
    const auto cmpY2u = static_cast<unsigned>(cmpY2) << 0;

    return (CheckDrawingOrderResult)IS_BEHIND_RESULT_LOOKUP_TABLE[cmpX1u | cmpX2u | cmpY1u | cmpY2u];
#endif
}
// clang-format on

bool SpatialInfo::overlaps(const SpatialInfo& aOther) const {
    return bbox.overlaps(aOther.bbox);
}

} // namespace gridworld
