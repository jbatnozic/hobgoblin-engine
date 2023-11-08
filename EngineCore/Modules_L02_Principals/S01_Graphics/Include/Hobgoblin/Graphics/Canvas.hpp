#ifndef UHOBGOBLIN_GR_CANVAS_HPP
#define UHOBGOBLIN_GR_CANVAS_HPP

#include <Hobgoblin/Graphics/Drawable.hpp>
#include <Hobgoblin/Graphics/Primitive_type.hpp>
#include <Hobgoblin/Graphics/Render_states.hpp>
#include <Hobgoblin/Graphics/Vertex.hpp>
#include <Hobgoblin/Graphics/Vertex_buffer.hpp>

#include <Hobgoblin/Private/Pmacro_define.hpp>

HOBGOBLIN_NAMESPACE_BEGIN
namespace gr {

enum class CanvasType {
    SFML //!< Rendering backend is sf::RenderTarget.
};

//! Abstracts an object (such as a window or a texture) that can be drawn onto.
class Canvas {
public:
    virtual ~Canvas() = default;

    virtual void draw(const Drawable& aDrawable,
                      const RenderStates& aStates = RenderStates::DEFAULT) = 0;

    virtual void draw(const Vertex* aVertices,
                      PZInteger aVertexCount,
                      PrimitiveType aPrimitiveType,
                      const RenderStates& aStates = RenderStates::DEFAULT) = 0;

    virtual void draw(const VertexBuffer& aVertexBuffer,
                      const RenderStates& aStates = RenderStates::DEFAULT) = 0;

    virtual void draw(const VertexBuffer& aVertexBuffer,
                      PZInteger aFirstVertex,
                      PZInteger aVertexCount,
                      const RenderStates& aStates = RenderStates::DEFAULT) = 0;

    //! If the underlying object supports draw batching, various draw() calls may
    //! be deferred until later. Calling flush() will make sure that every draw()
    //! call thus far is resolved.
    virtual void flush() = 0;

    //! Returns information about the type of the underlying implementation of the Canvas.
    //! 
    //! \param aType reference to a variable of type `CanvasType` where the type of the
    //!              canvas shall be written.
    //! \param aRenderingBackend reference to a void pointer where the address of the
    //!                          rendering backend (mathing the type of the canvas)
    //!                          shall be written.
    virtual void getCanvasDetails(CanvasType& aType, void*& aRenderingBackend) = 0;
};

} // namespace gr
HOBGOBLIN_NAMESPACE_END

#include <Hobgoblin/Private/Pmacro_undef.hpp>
#include <Hobgoblin/Private/Short_namespace.hpp>

#endif // !UHOBGOBLIN_GR_CANVAS_HPP