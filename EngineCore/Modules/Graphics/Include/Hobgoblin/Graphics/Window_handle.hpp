#ifndef HOBGOBLIN_GRAPHICS_WINDOW_HANDLE_HPP
#define HOBGOBLIN_GRAPHICS_WINDOW_HANDLE_HPP

// TODO: This leaks SFML headers!
#include <SFML/Window/WindowHandle.hpp>

#include <Hobgoblin/Private/Pmacro_define.hpp>

HOBGOBLIN_NAMESPACE_BEGIN
namespace gr {

using WindowHandle = sf::WindowHandle;

} // namespace gr
HOBGOBLIN_NAMESPACE_END

#include <Hobgoblin/Private/Pmacro_undef.hpp>
#include <Hobgoblin/Private/Short_namespace.hpp>

#endif // !HOBGOBLIN_GRAPHICS_WINDOW_HANDLE_HPP

