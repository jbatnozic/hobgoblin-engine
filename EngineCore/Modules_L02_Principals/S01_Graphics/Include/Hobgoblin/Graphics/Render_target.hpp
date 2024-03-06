////////////////////////////////////////////////////////////
//
// SFML - Simple and Fast Multimedia Library
// Copyright (C) 2007-2018 Laurent Gomila (laurent@sfml-dev.org)
//
// This software is provided 'as-is', without any express or implied warranty.
// In no event will the authors be held liable for any damages arising from the use of this software.
//
// Permission is granted to anyone to use this software for any purpose,
// including commercial applications, and to alter it and redistribute it freely,
// subject to the following restrictions:
//
// 1. The origin of this software must not be misrepresented;
//    you must not claim that you wrote the original software.
//    If you use this software in a product, an acknowledgment
//    in the product documentation would be appreciated but is not required.
//
// 2. Altered source versions must be plainly marked as such,
//    and must not be misrepresented as being the original software.
//
// 3. This notice may not be removed or altered from any source distribution.
//
////////////////////////////////////////////////////////////

#ifndef UHOBGOBLIN_GRAPHICS_RENDER_TARGET_HPP
#define UHOBGOBLIN_GRAPHICS_RENDER_TARGET_HPP

#include <Hobgoblin/Graphics/Canvas.hpp>
#include <Hobgoblin/Graphics/Color.hpp>
#include <Hobgoblin/Graphics/View.hpp>
#include <Hobgoblin/Graphics/Transform.hpp>
#include <Hobgoblin/Graphics/Blend_mode.hpp>
#include <Hobgoblin/Graphics/Render_states.hpp>
#include <Hobgoblin/Graphics/Primitive_type.hpp>
#include <Hobgoblin/Graphics/Vertex.hpp>
#include <Hobgoblin/Math/Rectangle.hpp>
#include <Hobgoblin/Math/Vector.hpp>
#include <Hobgoblin/Utility/No_copy_no_move.hpp>

#include <type_traits>

#include <Hobgoblin/Private/Pmacro_define.hpp>

HOBGOBLIN_NAMESPACE_BEGIN
namespace gr {

class Drawable;
class VertexBuffer;

class RenderTarget : public Canvas {
public:
    virtual ~RenderTarget() = default;

    //! \brief Clear the entire target with a single color
    //!
    //! This function is usually called once every frame,
    //! to clear the previous contents of the target.
    //!
    //! \param color Fill color to use to clear the render target
    virtual void clear(const Color& aColor = COLOR_BLACK) = 0;

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

    //! TODO
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
    virtual math::Vector2f mapPixelToCoords(const math::Vector2i& aPoint, PZInteger aViewIdx = 0) const = 0;

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
    virtual math::Vector2i mapCoordsToPixel(const math::Vector2f& point, PZInteger aViewIdx = 0) const = 0;

    //! \brief Return the size of the rendering region of the target
    //!
    //! \return Size in pixels
    virtual math::Vector2pz getSize() const = 0;

    //! \brief Activate or deactivate the render target for rendering
    //!
    //! This function makes the render target's context current for
    //! future OpenGL rendering operations (so you shouldn't care
    //! about it if you're not doing direct OpenGL stuff).
    //! A render target's context is active only on the current thread,
    //! if you want to make it active on another thread you have
    //! to deactivate it on the previous thread first if it was active.
    //! Only one context can be current in a thread, so if you
    //! want to draw OpenGL geometry to another render target
    //! don't forget to activate it again. Activating a render
    //! target will automatically deactivate the previously active
    //! context (if any).
    //!
    //! \param active True to activate, false to deactivate
    //!
    //! \return True if operation was successful, false otherwise
    [[nodiscard]] virtual bool setActive(bool aActive = true) = 0;  // TODO(bool->exc)

    //! \brief Save the current OpenGL render states and matrices
    //!
    //! This function can be used when you mix SFML drawing
    //! and direct OpenGL rendering. Combined with popGLStates,
    //! it ensures that:
    //! \li SFML's internal states are not messed up by your OpenGL code
    //! \li your OpenGL states are not modified by a call to a SFML function
    //!
    //! More specifically, it must be used around code that
    //! calls Draw functions. Example:
    //! \code
    //! // OpenGL code here...
    //! window.pushGLStates();
    //! window.draw(...);
    //! window.draw(...);
    //! window.popGLStates();
    //! // OpenGL code here...
    //! \endcode
    //!
    //! Note that this function is quite expensive: it saves all the
    //! possible OpenGL states and matrices, even the ones you
    //! don't care about. Therefore it should be used wisely.
    //! It is provided for convenience, but the best results will
    //! be achieved if you handle OpenGL states yourself (because
    //! you know which states have really changed, and need to be
    //! saved and restored). Take a look at the resetGLStates
    //! function if you do so.
    //!
    //! \see popGLStates
    virtual void pushGLStates() = 0;

    //! \brief Restore the previously saved OpenGL render states and matrices
    //!
    //! See the description of pushGLStates to get a detailed
    //! description of these functions.
    //!
    //! \see pushGLStates
    virtual void popGLStates() = 0;

    //! \brief Reset the internal OpenGL states so that the target is ready for drawing
    //!
    //! This function can be used when you mix SFML drawing
    //! and direct OpenGL rendering, if you choose not to use
    //! pushGLStates/popGLStates. It makes sure that all OpenGL
    //! states needed by SFML are set, so that subsequent draw()
    //! calls will work as expected.
    //!
    //! Example:
    //! \code
    //! // OpenGL code here...
    //! glPushAttrib(...);
    //! window.resetGLStates();
    //! window.draw(...);
    //! window.draw(...);
    //! glPopAttrib(...);
    //! // OpenGL code here...
    //! \endcode
    virtual void resetGLStates() = 0;

    //! \brief Tell if the render target will use sRGB encoding when drawing on it
    //!
    //! \return True if the render target use sRGB encoding, false otherwise
    virtual bool isSrgb() const = 0;
};

} // namespace gr
HOBGOBLIN_NAMESPACE_END

#include <Hobgoblin/Private/Pmacro_undef.hpp>
#include <Hobgoblin/Private/Short_namespace.hpp>

#endif // !UHOBGOBLIN_GRAPHICS_RENDER_TARGET_HPP

////////////////////////////////////////////////////////////
/// \class sf::RenderTarget
/// \ingroup graphics
///
/// sf::RenderTarget defines the common behavior of all the
/// 2D render targets usable in the graphics module. It makes
/// it possible to draw 2D entities like sprites, shapes, text
/// without using any OpenGL command directly.
///
/// A sf::RenderTarget is also able to use views (sf::View),
/// which are a kind of 2D cameras. With views you can globally
/// scroll, rotate or zoom everything that is drawn,
/// without having to transform every single entity. See the
/// documentation of sf::View for more details and sample pieces of
/// code about this class.
///
/// On top of that, render targets are still able to render direct
/// OpenGL stuff. It is even possible to mix together OpenGL calls
/// and regular SFML drawing commands. When doing so, make sure that
/// OpenGL states are not messed up by calling the
/// pushGLStates/popGLStates functions.
///
/// \see sf::RenderWindow, sf::RenderTexture, sf::View
///
////////////////////////////////////////////////////////////
