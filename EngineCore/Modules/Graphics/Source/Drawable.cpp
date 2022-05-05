
#include <Hobgoblin/Graphics/Drawable.hpp>
#include <Hobgoblin/Graphics/Canvas.hpp>

#include <Hobgoblin/Private/Pmacro_define.hpp>

HOBGOBLIN_NAMESPACE_BEGIN
namespace gr {
namespace detail {

void Drawable_DrawOntoCanvas(const Drawable& aDrawable, 
                             Canvas& aCanvas, 
                             const sf::RenderStates& aStates) {
    aDrawable._draw(aCanvas, aStates);
}

} // namespace detail
} // namespace gr
HOBGOBLIN_NAMESPACE_END

#include <Hobgoblin/Private/Pmacro_undef.hpp>