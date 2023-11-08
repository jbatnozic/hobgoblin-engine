#ifndef UHOBGOBLIN_GRAPHICS_TEXTURE_RECT_HPP
#define UHOBGOBLIN_GRAPHICS_TEXTURE_RECT_HPP

#include <Hobgoblin/Math/Rectangle.hpp>

#include <cstdint>

#include <Hobgoblin/Private/Pmacro_define.hpp>

HOBGOBLIN_NAMESPACE_BEGIN
namespace gr {

//! An instance of TextureRect marks an area withing a Texture.
using TextureRect = math::Rectangle<std::uint16_t>;

static_assert(sizeof(TextureRect) <= sizeof(std::intptr_t), "TextureRect object too big.");

} // namespace gr
HOBGOBLIN_NAMESPACE_END

#include <Hobgoblin/Private/Pmacro_undef.hpp>
#include <Hobgoblin/Private/Short_namespace.hpp>

#endif // !UHOBGOBLIN_GRAPHICS_TEXTURE_RECT_HPP
