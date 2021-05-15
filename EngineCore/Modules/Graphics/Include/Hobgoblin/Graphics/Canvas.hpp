#ifndef UHOBGOBLIN_GR_CANVAS_HPP
#define UHOBGOBLIN_GR_CANVAS_HPP

#include <SFML/Graphics.hpp>

#include <Hobgoblin/Private/Pmacro_define.hpp>

HOBGOBLIN_NAMESPACE_BEGIN
namespace gr {

class Canvas {
public:
    virtual void draw(const sf::Drawable& drawable,
                      const sf::RenderStates& states = sf::RenderStates::Default) = 0;
    
    virtual void draw(const sf::Vertex* vertices, std::size_t vertexCount, sf::PrimitiveType type,
                      const sf::RenderStates& states = sf::RenderStates::Default) = 0;

    virtual void draw(const sf::VertexBuffer& vertexBuffer,
                      const sf::RenderStates& states = sf::RenderStates::Default) = 0;

    virtual void draw(const sf::VertexBuffer& vertexBuffer, std::size_t firstVertex, std::size_t vertexCount,
                      const sf::RenderStates& states = sf::RenderStates::Default) = 0;
};

class CanvasAdapter : public Canvas {
public:
     CanvasAdapter(sf::RenderTarget& renderTarget)
        : _renderTarget{renderTarget}
    {
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

private:
    sf::RenderTarget& _renderTarget;
};

} // namespace gr
HOBGOBLIN_NAMESPACE_END

#include <Hobgoblin/Private/Pmacro_undef.hpp>
#include <Hobgoblin/Private/Short_namespace.hpp>

#endif // !UHOBGOBLIN_GR_CANVAS_HPP