#pragma once

#include <GridWorld/Positions.hpp>
#include <GridWorld/Spatial_info.hpp>

#include <Hobgoblin/Graphics.hpp>
#include <Hobgoblin/Math.hpp>

#include <functional>

namespace gridworld {

namespace hg = jbatnozic::hobgoblin;

class DrawingContext {
public:
    //! Returns the world coordinates of the point which corresponds to the centre of
    //! the used view (as in - camera view).
    virtual WorldPosition getViewCentre() const = 0;

    //! Returns the world coordinates of the point from which visibility is measured.
    //! Note: In a game, this would most likely be the origin of the player's character
    //!       or similar.
    virtual WorldPosition getPointOfView() const = 0;

    //! Tests whether a point is visible from the main Point of View.
    //!
    //! If `true` is returned, the point is visible; if `false` is returned, the point
    //! is not visible. If `std::none` is returned, visibility could not be determined
    //! (point likely too far away from PoV).
    //!
    //! \see getPointOfView
    virtual std::optional<bool> testVisibilityAt(WorldPosition aPos) const = 0;

    //! Returns the ambient color (based on lighting rendering) at a certain point.
    //!
    //! If `std::none` is returned, the ambient colour could not be determined (point
    //! likely too far away from the view centre - remember that only the minimum
    //! lighting required to cover the view is rendered).
    virtual std::optional<hg::gr::Color> getColorAt(WorldPosition aPos) const = 0;
};

class RenderedObject {
public:
    RenderedObject() = default;

    RenderedObject(const SpatialInfo& aSpatialInfo)
        : _spatialInfo{aSpatialInfo} {}

    const SpatialInfo& getSpatialInfo() const {
        return _spatialInfo;
    }

    //! position = screen position
    virtual void draw(hg::gr::Canvas& aCanvas, hg::math::Vector2f aScreenPosition) const = 0;

protected:
    SpatialInfo _spatialInfo;
};

struct RendererConfiguration {
    float cellSize = 32.f;
};

// TODO(unused)
class Renderer {
public:
    virtual void startFrame(/*viewport, pov*/) = 0;

    virtual void addObject() = 0;

    virtual void render() = 0;
};

} // namespace gridworld
