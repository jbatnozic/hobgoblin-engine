// Copyright 2024 Jovan Batnozic. Released under MS-PL licence in Serbia.
// See https://github.com/jbatnozic/Hobgoblin?tab=readme-ov-file#licence

#pragma once

#include <GridGoblin/Model/Layer.hpp>
#include <GridGoblin/Model/Shape.hpp>
#include <GridGoblin/Spatial/World_position.hpp>

#include <Hobgoblin/Math.hpp>

namespace jbatnozic {
namespace gridgoblin {

namespace hg = ::jbatnozic::hobgoblin;

//! Describes the position of an object in a GridGoblin World.
//! Stores information about the bounding rectangle of the object, and the layer in which it exists.
class SpatialInfo {
public:
    SpatialInfo() = default;

    static SpatialInfo fromCenterAndSize(WorldPosition aCenter, hg::math::Vector2f aSize, Layer aLayer);

    static SpatialInfo fromTopLeftAndSize(WorldPosition      aTopLeft,
                                          hg::math::Vector2f aSize,
                                          Layer              aLayer);

    void setCenter(WorldPosition aPoint);

    void setTopLeft(WorldPosition aPoint);

    // Maintains the top-left corner, not the center!
    void setSize(hg::math::Vector2f aSize);

    void setSizeMaintainingCenter(hg::math::Vector2f aSize);

    WorldPosition getCenter() const;

    WorldPosition getTopLeft() const;

    const hg::math::Rectangle<float>& getBoundingBox() const;

    hg::math::Vector2f getSize() const;

    Layer getLayer() const;
    void  setLayer(Layer aLayer);

private:
    SpatialInfo(Layer aLayer);

    hg::math::Rectangle<float> _bbox;
    WorldPosition              _center;
    Layer                      _layer = Layer::FLOOR;
};

inline WorldPosition SpatialInfo::getCenter() const {
    return _center;
}

inline WorldPosition SpatialInfo::getTopLeft() const {
    return WorldPosition{
        {_bbox.getLeft(), _bbox.getTop()}
    };
}

inline const hg::math::Rectangle<float>& SpatialInfo::getBoundingBox() const {
    return _bbox;
}

inline hg::math::Vector2f SpatialInfo::getSize() const {
    return {_bbox.w, _bbox.h};
}

inline Layer SpatialInfo::getLayer() const {
    return _layer;
}
inline void SpatialInfo::setLayer(Layer aLayer) {
    _layer = aLayer;
}

} // namespace gridgoblin
} // namespace jbatnozic
