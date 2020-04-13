#ifndef UHOBGOBLIN_GR_BRUSH_HPP
#define UHOBGOBLIN_GR_BRUSH_HPP

#include <Hobgoblin/Graphics/Canvas.hpp>
#include <Hobgoblin/Graphics/Multisprite.hpp>
#include <Hobgoblin/Graphics/Views.hpp>
#include <SFML/Graphics.hpp>

#include <cassert>

#include <Hobgoblin/Private/Pmacro_define.hpp>

HOBGOBLIN_NAMESPACE_START
namespace gr {

class Brush {
public:
    // TODO: Implementation to .cpp file

    Brush(Canvas* canvas, sf::RenderStates renderStates = sf::RenderStates::Default);

    void setCanvas(Canvas* canvas);

    void setRenderStates(sf::RenderStates renderStates);

    const sf::RenderStates& getRenderStates() const noexcept;

    void drawCircle(float x, float y, float radius) const;

    // drawRectangle, drawSprite, drawMultisprite, drawLine, drawRectangle, drawTexture, ...

    //void draw(const sf::Drawable& drawable,
    //          const sf::RenderStates& states = sf::RenderStates::Default);

    //void draw(const sf::Vertex* vertices, std::size_t vertexCount, sf::PrimitiveType type,
    //          const sf::RenderStates& states = sf::RenderStates::Default);

    //void draw(const sf::VertexBuffer& vertexBuffer,
    //          const sf::RenderStates& states = sf::RenderStates::Default);

    //void draw(const sf::VertexBuffer& vertexBuffer, std::size_t firstVertex, std::size_t vertexCount,
    //          const sf::RenderStates& states = sf::RenderStates::Default);

private:
    Canvas* _canvas;
    sf::RenderStates _renderStates;
};

} // namespace gr
HOBGOBLIN_NAMESPACE_END

#include <Hobgoblin/Private/Pmacro_undef.hpp>
#include <Hobgoblin/Private/Short_namespace.hpp>

#endif // !UHOBGOBLIN_GR_BRUSH_HPP