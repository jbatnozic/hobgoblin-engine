// Copyright 2024 Jovan Batnozic. Released under MS-PL licence in Serbia.
// See https://github.com/jbatnozic/Hobgoblin?tab=readme-ov-file#licence

#include <GridGoblin/Spatial/Spatial_info.hpp>

namespace jbatnozic {
namespace gridgoblin {

namespace {
constexpr float Half(float aF) {
    return aF / 2.f;
}
} // namespace

SpatialInfo::SpatialInfo(Layer aLayer)
    : _layer{aLayer} {}

SpatialInfo SpatialInfo::fromCenterAndSize(WorldPosition      aCenter,
                                           hg::math::Vector2f aSize,
                                           Layer              aLayer) {
    SpatialInfo result{aLayer};
    result._center = aCenter;
    result.setSizeMaintainingCenter(aSize);
    return result;
}

SpatialInfo SpatialInfo::fromTopLeftAndSize(WorldPosition      aTopLeft,
                                            hg::math::Vector2f aSize,
                                            Layer              aLayer) {
    SpatialInfo result{aLayer};
    result._bbox.x = aTopLeft->x;
    result._bbox.y = aTopLeft->y;
    result.setSize(aSize);
    return result;
}

void SpatialInfo::setCenter(WorldPosition aPoint) {
    _center->x = aPoint->x;
    _center->y = aPoint->y;

    _bbox.x = aPoint->x - Half(_bbox.w);
    _bbox.y = aPoint->y - Half(_bbox.h);
}

void SpatialInfo::setTopLeft(WorldPosition aPoint) {
    _bbox.x = aPoint->x;
    _bbox.y = aPoint->y;

    _center->x = aPoint->x + Half(_bbox.w);
    _center->y = aPoint->y + Half(_bbox.h);
}

void SpatialInfo::setSize(hg::math::Vector2f aSize) {
    _bbox.w = aSize.x;
    _bbox.h = aSize.y;

    _center->x = _bbox.x + Half(aSize.x);
    _center->y = _bbox.y + Half(aSize.y);
}

void SpatialInfo::setSizeMaintainingCenter(hg::math::Vector2f aSize) {
    _bbox.w = aSize.x;
    _bbox.h = aSize.y;

    _bbox.x = _center->x - Half(aSize.x);
    _bbox.y = _center->y - Half(aSize.y);
}

} // namespace gridgoblin
} // namespace jbatnozic
