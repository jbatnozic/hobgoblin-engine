#ifndef UHOBGOBLIN_GR_CANVAS_HPP
#define UHOBGOBLIN_GR_CANVAS_HPP

#include <Hobgoblin/Graphics/Drawable.hpp>
#include <SFML/Graphics.hpp>

#include <Hobgoblin/Private/Pmacro_define.hpp>

HOBGOBLIN_NAMESPACE_BEGIN
namespace gr {

//! Abstracts an object (such as a window or a texture) that can be drawn onto.
class Canvas {
public:
    virtual ~Canvas() = default;

    virtual void draw(const Drawable& aDrawable,
                      const sf::RenderStates& aStates = sf::RenderStates::Default) = 0;

    virtual void draw(const sf::Drawable& drawable,
                      const sf::RenderStates& states = sf::RenderStates::Default) = 0;

    virtual void draw(const sf::Vertex* vertices,
                      std::size_t vertexCount,
                      sf::PrimitiveType type,
                      const sf::RenderStates& states = sf::RenderStates::Default) = 0;

    virtual void draw(const sf::VertexBuffer& vertexBuffer,
                      const sf::RenderStates& states = sf::RenderStates::Default) = 0;

    virtual void draw(const sf::VertexBuffer& vertexBuffer,
                      std::size_t firstVertex,
                      std::size_t vertexCount,
                      const sf::RenderStates& states = sf::RenderStates::Default) = 0;

    //! If the underlying object supports draw batching, various draw() calls may
    //! be deferred until later. Calling flush() will make sure that every draw()
    //! call thus far is resolved.
    virtual void flush() = 0;
};

//! Adapts a sf::RenderTarget to the Canvas interface.
class CanvasAdapter : public Canvas {
public:
    // TODO: Impl to .cpp file!

    CanvasAdapter(sf::RenderTarget& renderTarget)
        : _renderTarget{renderTarget}
    {
    }

    void draw(const Drawable& aDrawable,
              const sf::RenderStates& aStates = sf::RenderStates::Default) override {
        detail::Drawable_DrawOntoCanvas(aDrawable, SELF, aStates);
    }

    void draw(const sf::Drawable& drawable,
              const sf::RenderStates& states = sf::RenderStates::Default) override {
        _renderTarget.draw(drawable, states);
    }

    void draw(const sf::Vertex* vertices, std::size_t vertexCount, sf::PrimitiveType type,
              const sf::RenderStates& states = sf::RenderStates::Default) override {
        _renderTarget.draw(vertices, vertexCount, type, states);
    }

    void draw(const sf::VertexBuffer& vertexBuffer,
              const sf::RenderStates& states = sf::RenderStates::Default) override {
        _renderTarget.draw(vertexBuffer, states);
    }

    void draw(const sf::VertexBuffer& vertexBuffer, std::size_t firstVertex, std::size_t vertexCount,
              const sf::RenderStates& states = sf::RenderStates::Default) override {
        _renderTarget.draw(vertexBuffer, firstVertex, vertexCount, states);
    }

    void flush() override {
    }

private:
    sf::RenderTarget& _renderTarget;
};

} // namespace gr
HOBGOBLIN_NAMESPACE_END

#include <Hobgoblin/Private/Pmacro_undef.hpp>
#include <Hobgoblin/Private/Short_namespace.hpp>

#endif // !UHOBGOBLIN_GR_CANVAS_HPP