
#include <Hobgoblin/Graphics/Brush.hpp>

#include <cassert>

#include <Hobgoblin/Private/Pmacro_define.hpp>

HOBGOBLIN_NAMESPACE_START
namespace gr {

Brush::Brush(Canvas* canvas, sf::RenderStates renderStates)
    : _canvas{canvas}
    , _renderStates{renderStates}
{
}

void Brush::setCanvas(Canvas* canvas) {
    _canvas = canvas;
}

void Brush::setRenderStates(sf::RenderStates renderStates) {
    _renderStates = renderStates;
}

const sf::RenderStates& Brush::getRenderStates() const noexcept {
    return _renderStates;
}

void Brush::drawCircle(float x, float y, float radius) const {
    assert(_canvas && "Cannot draw with canvas set to null");
    sf::CircleShape circle{radius};
    circle.setPosition({x, y});
    _canvas->draw(circle, _renderStates);
}


} // namespace gr
HOBGOBLIN_NAMESPACE_END

#include <Hobgoblin/Private/Pmacro_undef.hpp>
#include <Hobgoblin/Private/Short_namespace.hpp>