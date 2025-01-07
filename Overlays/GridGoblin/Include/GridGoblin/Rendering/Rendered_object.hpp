// Copyright 2024 Jovan Batnozic. Released under MS-PL licence in Serbia.
// See https://github.com/jbatnozic/Hobgoblin?tab=readme-ov-file#licence

#pragma once

#include <Hobgoblin/Graphics.hpp>

#include <GridGoblin/Spatial/Position_in_view.hpp>
#include <GridGoblin/Spatial/Spatial_info.hpp>

namespace jbatnozic {
namespace gridgoblin {

//! Abstract base class for objects that exist in and (optionally) move around a GridGoblin World,
//! that also need to be rendered/drawn together with or inside of the World.
class RenderedObject {
public:
    //! Virtual destructor.
    virtual ~RenderedObject() = default;

    //! Default constructor.
    //!
    //! \warning it's recommended to use the other constructor in derived classes, because this one will
    //!          initialize the spatial info object (see below) with default values.
    RenderedObject() = default;

    RenderedObject(const SpatialInfo& aSpatialInfo)
        : _spatialInfo{aSpatialInfo} {}

    const SpatialInfo& getSpatialInfo() const {
        return _spatialInfo;
    }

    //! Called by the renderer to render/draw the object.
    //!
    //! \param aCanvas canvas onto which to draw.
    //! \param aPosInView position where to draw, in the coordinate system of the view (but the value
    //!                   is absolute, not relative to the current position of the view!). This will
    //!                   correspond to the center of the spatial info object (see below). Note: this
    //!                   parameter is needed because some renderers (for example, the dimetric one)
    //!                   transform the positions of objects in ways that don't correspond 1:1 to
    //!                   the coordinate system of the view.
    virtual void render(hg::gr::Canvas& aCanvas, PositionInView aPosInView) const = 0;

protected:
    //! Spatial info describing where in the world the object is placed. Remember to keep his information
    //! up to date in derived classes, so renderers can draw the object properly and in correct relation
    //! with other rendered objects and terrain.
    SpatialInfo _spatialInfo;
};

} // namespace gridgoblin
} // namespace jbatnozic
