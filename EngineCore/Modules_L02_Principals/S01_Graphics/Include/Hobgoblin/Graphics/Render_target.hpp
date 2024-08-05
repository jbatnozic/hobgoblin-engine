// Copyright 2024 Jovan Batnozic. Released under MS-PL licence in Serbia.
// See https://github.com/jbatnozic/Hobgoblin?tab=readme-ov-file#licence

#ifndef UHOBGOBLIN_GRAPHICS_RENDER_TARGET_HPP
#define UHOBGOBLIN_GRAPHICS_RENDER_TARGET_HPP

#include <Hobgoblin/Graphics/Canvas.hpp>
#include <Hobgoblin/Graphics/View_controller.hpp>

#include <Hobgoblin/Private/Pmacro_define.hpp>

HOBGOBLIN_NAMESPACE_BEGIN
namespace gr {

class RenderTarget
    : public Canvas
    , public ViewController {};

} // namespace gr
HOBGOBLIN_NAMESPACE_END

#include <Hobgoblin/Private/Pmacro_undef.hpp>
#include <Hobgoblin/Private/Short_namespace.hpp>

#endif // !UHOBGOBLIN_GRAPHICS_RENDER_TARGET_HPP
