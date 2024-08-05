// Copyright 2024 Jovan Batnozic. Released under MS-PL licence in Serbia.
// See https://github.com/jbatnozic/Hobgoblin?tab=readme-ov-file#licence

#ifndef UHOBGOBLIN_GRAPHICS_VIEW_CONTROLLER_HPP
#define UHOBGOBLIN_GRAPHICS_VIEW_CONTROLLER_HPP

#include <Hobgoblin/Common.hpp>
#include <Hobgoblin/Graphics/View.hpp>
#include <Hobgoblin/Math/Vector.hpp>

#include <Hobgoblin/Private/Pmacro_define.hpp>

HOBGOBLIN_NAMESPACE_BEGIN
namespace gr {

//! ViewController abstracts away an object (such as a window or render texture) which is able
//! to use views (hg::gr::View), which are a kind of 2D cameras. With views you can globally
//! scroll, rotate or zoom everything that is drawn,  without having to transform every single
//! entity. See the documentation of hg::gr::View for more details and sample pieces of code
//! about this class.
class ViewController {
public:
    virtual ~ViewController() = default;

    //! \brief Sets the number of views that can be used by this render target.
    //!
    //! All the views start as active and with the default dimensions.
    //! Keep in mind that every draw() call done with this render target will be
    //! automatically repeated for every active view, so always deactive the views
    //! that you're not using.
    //!
    //! \param aViewCount Number of views.
    //!
    //! \see setView, getView
    virtual void setViewCount(PZInteger aViewCount) = 0;

    //! \brief Change the current active view
    //!
    //! The view is like a 2D camera, it controls which part of
    //! the 2D scene is visible, and how it is viewed in the
    //! render target.
    //! The new view will affect everything that is drawn, until
    //! another view is set.
    //! The render target keeps its own copy of the view object,
    //! so it is not necessary to keep the original one alive
    //! after calling this function.
    //! To restore the original view of the target, you can pass
    //! the result of getDefaultView() to this function.
    //!
    //! \param aView New view to use
    //! \param aViewIdx TODO
    //!
    //! \see getView, getDefaultView
    virtual void setView(const View& aView) = 0;

    //! TODO(add description)
    virtual void setView(PZInteger aViewIdx, const View& aView) = 0;

    //! \return The number of views used by this render target.
    virtual PZInteger getViewCount() const = 0;

    //! \brief Get the view currently in use in the render target
    //!
    //! \param aViewIdx TODO
    //!
    //! \return The view object that is currently used
    //!
    //! \see setView, getDefaultView
    virtual const View& getView(PZInteger aViewIdx = 0) const = 0;

    //! TODO(add description)
    virtual View& getView(PZInteger aViewIdx = 0) = 0;

    //! \brief Get the default view of the render target
    //!
    //! The default view has the initial size of the render target,
    //! and never changes after the target has been created.
    //!
    //! \return The default view of the render target
    //!
    //! \see setView, getView
    virtual View getDefaultView() const = 0;

    //! \brief Get the viewport of a view, applied to this render target
    //!
    //! The viewport is defined in the view as a ratio, this function
    //! simply applies this ratio to the current dimensions of the
    //! render target to calculate the pixels rectangle that the viewport
    //! actually covers in the target.
    //!
    //! \param view The view for which we want to compute the viewport
    //!
    //! \return Viewport rectangle, expressed in pixels
    virtual math::Rectangle<PZInteger> getViewport(const View& aView) const = 0;

    //! TODO
    virtual math::Rectangle<PZInteger> getViewport(PZInteger aViewIdx) const = 0;

    //! \brief Convert a point from target coordinates to world
    //!        coordinates, using the current view
    //!
    //! This function is an overload of the mapPixelToCoords
    //! function that implicitly uses the current view.
    //! It is equivalent to:
    //! \code
    //! target.mapPixelToCoords(point, target.getView());
    //! \endcode
    //!
    //! \param point Pixel to convert
    //! \param aViewIdx TODO
    //!
    //! \return The converted point, in "world" coordinates
    //!
    //! \see mapCoordsToPixel
    virtual math::Vector2f mapPixelToCoords(const math::Vector2i& aPoint,
                                            PZInteger             aViewIdx = 0) const = 0;

    //! \brief Convert a point from target coordinates to world coordinates
    //!
    //! This function finds the 2D position that matches the
    //! given pixel of the render target. In other words, it does
    //! the inverse of what the graphics card does, to find the
    //! initial position of a rendered pixel.
    //!
    //! Initially, both coordinate systems (world units and target pixels)
    //! match perfectly. But if you define a custom view or resize your
    //! render target, this assertion is not true anymore, i.e. a point
    //! located at (10, 50) in your render target may map to the point
    //! (150, 75) in your 2D world -- if the view is translated by (140, 25).
    //!
    //! For render-windows, this function is typically used to find
    //! which point (or object) is located below the mouse cursor.
    //!
    //! This version uses a custom view for calculations, see the other
    //! overload of the function if you want to use the current view of the
    //! render target.
    //!
    //! \param point Pixel to convert
    //! \param view The view to use for converting the point
    //!
    //! \return The converted point, in "world" units
    //!
    //! \see mapCoordsToPixel
    virtual math::Vector2f mapPixelToCoords(const math::Vector2i& aPoint, const View& aView) const = 0;

    //! \brief Convert a point from world coordinates to target coordinates
    //!
    //! This function finds the pixel of the render target that matches
    //! the given 2D point. In other words, it goes through the same process
    //! as the graphics card, to compute the final position of a rendered point.
    //!
    //! Initially, both coordinate systems (world units and target pixels)
    //! match perfectly. But if you define a custom view or resize your
    //! render target, this assertion is not true anymore, i.e. a point
    //! located at (150, 75) in your 2D world may map to the pixel
    //! (10, 50) of your render target -- if the view is translated by (140, 25).
    //!
    //! This version uses a custom view for calculations, see the other
    //! overload of the function if you want to use the current view of the
    //! render target.
    //!
    //! \param point Point to convert
    //! \param view The view to use for converting the point
    //!
    //! \return The converted point, in target coordinates (pixels)
    //!
    //! \see mapPixelToCoords
    virtual math::Vector2i mapCoordsToPixel(const math::Vector2f& point, const View& view) const = 0;

    //! \brief Convert a point from world coordinates to target
    //!        coordinates, using the current view
    //!
    //! This function is an overload of the mapCoordsToPixel
    //! function that implicitly uses the current view.
    //! It is equivalent to:
    //! \code
    //! target.mapCoordsToPixel(point, target.getView());
    //! \endcode
    //!
    //! \param point Point to convert
    //! \param aViewIdx TODO
    //!
    //! \return The converted point, in target coordinates (pixels)
    //!
    //! \see mapPixelToCoords
    virtual math::Vector2i mapCoordsToPixel(const math::Vector2f& point,
                                            PZInteger             aViewIdx = 0) const = 0;
};

} // namespace gr
HOBGOBLIN_NAMESPACE_END

#include <Hobgoblin/Private/Pmacro_undef.hpp>
#include <Hobgoblin/Private/Short_namespace.hpp>

#endif // !UHOBGOBLIN_GRAPHICS_VIEW_CONTROLLER_HPP
