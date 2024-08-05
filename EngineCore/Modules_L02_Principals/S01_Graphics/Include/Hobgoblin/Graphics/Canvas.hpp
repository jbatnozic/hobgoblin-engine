// Copyright 2024 Jovan Batnozic. Released under MS-PL licence in Serbia.
// See https://github.com/jbatnozic/Hobgoblin?tab=readme-ov-file#licence

#ifndef UHOBGOBLIN_GRAPHICS_CANVAS_HPP
#define UHOBGOBLIN_GRAPHICS_CANVAS_HPP

#include <Hobgoblin/Common.hpp>
#include <Hobgoblin/Graphics/Drawable.hpp>
#include <Hobgoblin/Graphics/Primitive_type.hpp>
#include <Hobgoblin/Graphics/Render_states.hpp>
#include <Hobgoblin/Graphics/Vertex.hpp>
#include <Hobgoblin/Graphics/Vertex_buffer.hpp>

#include <Hobgoblin/Private/Pmacro_define.hpp>

HOBGOBLIN_NAMESPACE_BEGIN
namespace gr {

enum class RenderingBackendType {
    SFML //!< Rendering backend is sf::RenderTarget.
};

struct RenderingBackendRef {
    void*                backendPtr;
    RenderingBackendType backendType;
};

//! Abstracts away an object (such as a window or a texture) onto which 2D entities can be drawn.
//! It makes it possible to draw 2D entities like sprites, shapes, text without using any OpenGL
//! command directly.
//!
//! On top of that, canvases are still able to render direct OpenGL stuff. It is even possible to
//! mix together OpenGL calls and regular Hobgoblin drawing commands. When doing so, make sure
//! that OpenGL states are not messed up by calling the pushGLStates/popGLStates functions.
//!
//! \see hg::gr::RenderWindow, hg::gr::RenderTexture
class Canvas {
public:
    //! Virtual destructor.
    virtual ~Canvas() = default;

    //! \brief Return the size of the rendering region of the canvas.
    //!
    //! \return Size in pixels.
    virtual math::Vector2pz getSize() const = 0;

    //! Returns information about the underlying implementation of the Canvas
    //! (the result of this is mostly only useful to the engine itself).
    virtual RenderingBackendRef getRenderingBackend() = 0;

    //! \brief Tell if the render target will use sRGB encoding when drawing on it
    //!
    //! \return True if the render target use sRGB encoding, false otherwise
    virtual bool isSrgb() const = 0;

    ///////////////////////////////////////////////////////////////////////////
    // DRAWING                                                               //
    ///////////////////////////////////////////////////////////////////////////

    //! \brief Clear the canvas with a single color.
    //!
    //! This function is usually called once every frame,
    //! to clear the previous contents of the canvas.
    //!
    //! \param aColor Fill color to use to clear the canvas.
    virtual void clear(const Color& aColor = COLOR_BLACK) = 0;

    virtual void draw(const Drawable&     aDrawable,
                      const RenderStates& aStates = RenderStates::DEFAULT) = 0;

    virtual void draw(const Vertex*       aVertices,
                      PZInteger           aVertexCount,
                      PrimitiveType       aPrimitiveType,
                      const RenderStates& aStates = RenderStates::DEFAULT) = 0;

    virtual void draw(const VertexBuffer& aVertexBuffer,
                      const RenderStates& aStates = RenderStates::DEFAULT) = 0;

    virtual void draw(const VertexBuffer& aVertexBuffer,
                      PZInteger           aFirstVertex,
                      PZInteger           aVertexCount,
                      const RenderStates& aStates = RenderStates::DEFAULT) = 0;

    //! If the underlying object supports draw batching, various draw() calls may
    //! be deferred until later. Calling flush() will make sure that every draw()
    //! call thus far is resolved.
    virtual void flush() = 0;

    ///////////////////////////////////////////////////////////////////////////
    // OPEN GL                                                               //
    ///////////////////////////////////////////////////////////////////////////

    //! \brief Activate or deactivate the canvas for rendering.
    //!
    //! This function makes the canvas' context current for
    //! future OpenGL rendering operations (so you shouldn't care
    //! about it if you're not doing direct OpenGL stuff).
    //! A canvas' context is active only on the current thread,
    //! if you want to make it active on another thread you have
    //! to deactivate it on the previous thread first if it was active.
    //! Only one context can be current in a thread, so if you
    //! want to draw OpenGL geometry to another canvas
    //! don't forget to activate it again. Activating a canvas
    //! will automatically deactivate the previously active
    //! context (if any).
    //!
    //! \param aActive True to activate, false to deactivate.
    //!
    //! \return True if operation was successful, false otherwise.
    [[nodiscard]] virtual bool setActive(bool aActive = true) = 0; // TODO(bool->exc)

    //! \brief Save the current OpenGL render states and matrices.
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

    //! \brief Restore the previously saved OpenGL render states and matrices.
    //!
    //! See the description of pushGLStates to get a detailed
    //! description of these functions.
    //!
    //! \see pushGLStates
    virtual void popGLStates() = 0;

    //! \brief Reset the internal OpenGL states so that the canvas is ready for drawing.
    //!
    //! This function can be used when you mix Hobgoblin drawing
    //! and direct OpenGL rendering, if you choose not to use
    //! pushGLStates/popGLStates. It makes sure that all OpenGL
    //! states needed by Hobgoblin are set, so that subsequent draw()
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
};

} // namespace gr
HOBGOBLIN_NAMESPACE_END

#include <Hobgoblin/Private/Pmacro_undef.hpp>
#include <Hobgoblin/Private/Short_namespace.hpp>

#endif // !UHOBGOBLIN_GRAPHICS_CANVAS_HPP
