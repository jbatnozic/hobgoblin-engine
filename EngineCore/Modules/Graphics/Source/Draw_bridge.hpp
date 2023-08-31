#ifndef UHOBGOBLIN_GRAPHICS_DRAW_BRIDGE_HPP
#define UHOBGOBLIN_GRAPHICS_DRAW_BRIDGE_HPP

#include <Hobgoblin/Graphics/Canvas.hpp>
#include <Hobgoblin/Graphics/Drawable.hpp>
#include <Hobgoblin/Graphics/View.hpp>

#include "Multiview_rendertarget_adapter.hpp"

#include <typeinfo>

#include <Hobgoblin/Private/Pmacro_define.hpp>

HOBGOBLIN_NAMESPACE_BEGIN
namespace gr {

template <class taCallable>
bool Draw(Canvas& aCanvas, const taCallable&& aCallable) {
    if (typeid(aCanvas) == typeid(MultiViewRenderTargetAdapter)) {
        aCallable(static_cast<MultiViewRenderTargetAdapter&>(aCanvas).getSFMLRenderTarget());
        return true;
    }
    return false;
}


} // namespace gr
HOBGOBLIN_NAMESPACE_END

#include <Hobgoblin/Private/Pmacro_undef.hpp>
#include <Hobgoblin/Private/Short_namespace.hpp>

#endif // !UHOBGOBLIN_GRAPHICS_DRAW_BRIDGE_HPP
