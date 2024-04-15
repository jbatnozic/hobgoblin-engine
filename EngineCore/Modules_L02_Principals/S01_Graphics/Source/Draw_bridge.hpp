// Copyright 2024 Jovan Batnozic. Released under MS-PL licence in Serbia.
// See https://github.com/jbatnozic/Hobgoblin?tab=readme-ov-file#licence

// clang-format off

#ifndef UHOBGOBLIN_GRAPHICS_DRAW_BRIDGE_HPP
#define UHOBGOBLIN_GRAPHICS_DRAW_BRIDGE_HPP

#include <Hobgoblin/HGExcept.hpp>
#include <Hobgoblin/Graphics/Canvas.hpp>
#include <Hobgoblin/Graphics/Drawable.hpp>
#include <Hobgoblin/Graphics/View.hpp>

#include "Multiview_rendertarget_adapter.hpp"

#include <cassert>
#include <typeinfo>

#include <Hobgoblin/Private/Pmacro_define.hpp>

HOBGOBLIN_NAMESPACE_BEGIN
namespace gr {

//! It BRIDGES the DRAW calls :D
template <class taCallable>
bool Draw(Canvas& aCanvas, taCallable&& aCallable) {
    CanvasType canvasType;
    void* renderingBackend;
    aCanvas.getCanvasDetails(canvasType, renderingBackend);
    assert(renderingBackend != nullptr);

    switch (canvasType) {
    case CanvasType::SFML:
        aCallable(*static_cast<sf::RenderTarget*>(renderingBackend));
        return true;

    default:
        HG_THROW_TRACED(InvalidArgumentError, 0, "Invalid CanvasType value ({}).", (int)canvasType);
    }
}

} // namespace gr
HOBGOBLIN_NAMESPACE_END

#include <Hobgoblin/Private/Pmacro_undef.hpp>
#include <Hobgoblin/Private/Short_namespace.hpp>

#endif // !UHOBGOBLIN_GRAPHICS_DRAW_BRIDGE_HPP

// clang-format on
