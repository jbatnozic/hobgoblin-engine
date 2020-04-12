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

    Brush(Canvas* canvas, sf::RenderStates renderStates = sf::RenderStates::Default)
        : _canvas{canvas}
        , _renderStates{renderStates}
    {
    }

    void setCanvas(Canvas* canvas) {
        _canvas = canvas;
    }

    void setRenderStates(sf::RenderStates renderStates);
    const sf::RenderStates& getRenderStates() const noexcept;

    void drawCircle(float x, float y, float radius) const {
        assert(_canvas && "Cannot draw with canvas set to null");
        sf::CircleShape circle{radius};
        circle.setPosition({x, y});
        _canvas->draw(circle, _renderStates);
    }

    // drawRectangle, drawSprite, drawMultisprite, drawLine, drawRectangle, drawTexture, ...

private:
    Canvas* _canvas;
    sf::RenderStates _renderStates;
};

} // namespace gr
HOBGOBLIN_NAMESPACE_END

#include <Hobgoblin/Private/Pmacro_undef.hpp>
#include <Hobgoblin/Private/Short_namespace.hpp>

#endif // !UHOBGOBLIN_GR_BRUSH_HPP