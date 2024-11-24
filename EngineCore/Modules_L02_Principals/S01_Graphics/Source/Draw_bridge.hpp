// Copyright 2024 Jovan Batnozic. Released under MS-PL licence in Serbia.
// See https://github.com/jbatnozic/Hobgoblin?tab=readme-ov-file#licence

#ifndef UHOBGOBLIN_GRAPHICS_DRAW_BRIDGE_HPP
#define UHOBGOBLIN_GRAPHICS_DRAW_BRIDGE_HPP

#include <Hobgoblin/Graphics/Canvas.hpp>
#include <Hobgoblin/Graphics/Drawable.hpp>
#include <Hobgoblin/Graphics/View.hpp>
#include <Hobgoblin/HGExcept.hpp>

#include <cassert>
#include <typeinfo>

#include <Hobgoblin/Private/Pmacro_define.hpp>

HOBGOBLIN_NAMESPACE_BEGIN
namespace gr {

//! It BRIDGES the DRAW calls :D
template <class taCallable>
bool Draw(Canvas& aCanvas, taCallable&& aCallable) {
    const auto backendRef = aCanvas.getRenderingBackend();
    assert(backendRef.backendPtr != nullptr);

    switch (backendRef.backendType) {
    case RenderingBackendType::SFML:
        aCallable(*static_cast<sf::RenderTarget*>(backendRef.backendPtr));
        return true;

    default:
        HG_THROW_TRACED(InvalidArgumentError,
                        0,
                        "Invalid RenderingBackendType value ({}).",
                        (int)backendRef.backendType);
    }
}

} // namespace gr
HOBGOBLIN_NAMESPACE_END

#include <Hobgoblin/Private/Pmacro_undef.hpp>
#include <Hobgoblin/Private/Short_namespace.hpp>

#endif // !UHOBGOBLIN_GRAPHICS_DRAW_BRIDGE_HPP
