#pragma once

#include <GridWorld/Model/Shape.hpp>
#include <GridWorld/Positions.hpp>

#include <Hobgoblin/Math.hpp>

#include <functional>

namespace gridworld {

namespace hg = jbatnozic::hobgoblin;

enum class Layer {
    Floor,
    FloorDecoration,
    Entity,
    Ceiling // TODO: Do I need this?
    // TODO(change capitalization)
};

class SpatialInfo {
public:
    SpatialInfo() = default;

    static SpatialInfo fromCentreAndSize(WorldPosition      aCentre,
                                         hg::math::Vector2f aSize,
                                         Shape              aShape = Shape::FULL_SQUARE);

    static SpatialInfo fromTopLeftAndSize(WorldPosition      aTopLeft,
                                          hg::math::Vector2f aSize,
                                          Shape              aShape = Shape::FULL_SQUARE);

    void setCentre(WorldPosition aPoint);

    void setTopLeft(WorldPosition aPoint);

    // Maintains the top-left corner, not the centre!
    void setSize(hg::math::Vector2f aSize);

    void setSizeMaintainingCentre(hg::math::Vector2f aSize);

    // TODO(getters)

    WorldPosition getCentre() const;

    WorldPosition getTopLeft() const;

    hg::math::Vector2f getSize() const;

    enum CheckDrawingOrderResult {
        DRAW_OTHER_FIRST = -1,
        DOES_NOT_MATTER  = 0,
        DRAW_THIS_FIRST  = 1
    };

    //! Checks whether `this` should be drawn before `aOther` as per the dimetric rendering order.
    //! Return value reference:
    //! - DRAW_OTHER_FIRST: `aOther` should be drawn before `this`.
    //! - DOES_NOT_MATTER : it doesn't matter which one is drawn first.
    //! - DRAW_THIS_FIRST : `this` should be drawn before `aOther`.
    //!
    //! Note: if the bounding boxes overlap, there is technically no correct answer, so a simple
    //!       check is done to see which centre is lower on the screen.
    CheckDrawingOrderResult checkDimetricDrawingOrder(const SpatialInfo& aOther) const;

private:
    hg::math::Rectangle<float> bbox;
    WorldPosition              centre;
    Shape                      shape = Shape::EMPTY;

    bool overlaps(const SpatialInfo& aOther) const;
};

} // namespace gridworld
