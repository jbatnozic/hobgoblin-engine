#pragma once

#include <Hobgoblin/Math.hpp>

#include <functional>

namespace gridworld {

namespace hg = jbatnozic::hobgoblin;

enum class Layer {
    Floor,
    FloorDecoration,
    Entity,
    Ceiling // TODO: Do I need this?
};

enum class Shape {
    Empty,
    Square,
    Circle
};

class SpatialInfo {
public:
    // TODO(constructor)

    void setCentre(hg::math::Vector2<float> aPoint);

    void setTopLeft(hg::math::Vector2<float> aPoint);

    // Maintains the top-left corner, not the centre!
    void setSize(hg::math::Vector2<float> aSize);

    void setSizeMaintainingCentre(hg::math::Vector2<float> aSize);

    // TODO(getters)

    enum CheckDrawingOrderResult {
        DRAW_OTHER_FIRST = -1,
        DOES_NOT_MATTER  =  0,
        DRAW_THIS_FIRST  =  1
    };

    //! Checks whether `this` should be drawn before `aOther` as per the isometric rendering order.
    //! Return value reference:
    //! - DRAW_OTHER_FIRST: `aOther` should be drawn before `this`.
    //! - DOES_NOT_MATTER : it doesn't matter which one is drawn first.
    //! - DRAW_THIS_FIRST : `this` should be drawn before `aOther`.
    //! 
    //! Note: if the bounding boxes overlap, there is technically no correct answer, so a simple
    //!       check is done to see which centre is lower on the screen.
    CheckDrawingOrderResult checkIsometricDrawingOrder(const SpatialInfo& aOther) const;

private:
    hg::math::Rectangle<float> bbox;
    hg::math::Vector2<float>   centre;
    Shape                      shape;

    bool overlaps(const SpatialInfo& aOther) const;
};

} // namespace gridw
