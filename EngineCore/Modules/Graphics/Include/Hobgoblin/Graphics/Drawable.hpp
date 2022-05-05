#ifndef UHOBGOBLIN_GR_DRAWABLE_HPP
#define UHOBGOBLIN_GR_DRAWABLE_HPP

#include <SFML/Graphics.hpp>

#include <Hobgoblin/Private/Pmacro_define.hpp>

namespace sf {
class RenderStates;
} // namespace sf

HOBGOBLIN_NAMESPACE_BEGIN
namespace gr {

class Canvas;
class Drawable;

namespace detail {
void Drawable_DrawOntoCanvas(const Drawable&, Canvas&, const sf::RenderStates&);
} // namespace detail

class Drawable {
public:
    virtual ~Drawable() = default;

protected:
    virtual void _draw(Canvas& aCanvas, const sf::RenderStates& aStates) const = 0;

private:
    friend void detail::Drawable_DrawOntoCanvas(const Drawable&, Canvas&, const sf::RenderStates&);
};

} // namespace gr
HOBGOBLIN_NAMESPACE_END

#include <Hobgoblin/Private/Pmacro_undef.hpp>
#include <Hobgoblin/Private/Short_namespace.hpp>

#endif // !UHOBGOBLIN_GR_DRAWABLE_HPP