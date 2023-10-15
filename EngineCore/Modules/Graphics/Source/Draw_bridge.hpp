#ifndef UHOBGOBLIN_GRAPHICS_DRAW_BRIDGE_HPP
#define UHOBGOBLIN_GRAPHICS_DRAW_BRIDGE_HPP

#include <Hobgoblin/Graphics/Canvas.hpp>
#include <Hobgoblin/Graphics/Drawable.hpp>
#include <Hobgoblin/Graphics/View.hpp>

#include "Multiview_rendertarget_adapter.hpp"

#include <cassert>
#include <typeinfo>

#include <Hobgoblin/Private/Pmacro_define.hpp>

HOBGOBLIN_NAMESPACE_BEGIN
namespace gr {

namespace detail {
class CanvasAccessor {
public:
    static CanvasType getCanvasDetails(Canvas& aCanvas, void*& aRenderingBackend) {
        CanvasType type;
        aCanvas.getCanvasDetails(type, aRenderingBackend);
        return type;
    }
};
} // namespace detail

//! It BRIDGES the DRAW calls :D
template <class taCallable>
bool Draw(Canvas& aCanvas, taCallable&& aCallable) {
    void* renderingBackend;
    const auto canvasType = detail::CanvasAccessor::getCanvasDetails(aCanvas, renderingBackend);
    assert(renderingBackend != nullptr);

    switch (canvasType) {
    case CanvasType::Proxy:
        return Draw(*static_cast<Canvas*>(renderingBackend), std::forward<taCallable>(aCallable));

    case CanvasType::SFML:
        aCallable(*static_cast<sf::RenderTarget*>(renderingBackend));
        return true;

    default:
        HARD_ASSERT(false && "Invalid CanvasType value.");
    }
}

} // namespace gr
HOBGOBLIN_NAMESPACE_END

#include <Hobgoblin/Private/Pmacro_undef.hpp>
#include <Hobgoblin/Private/Short_namespace.hpp>

#endif // !UHOBGOBLIN_GRAPHICS_DRAW_BRIDGE_HPP
