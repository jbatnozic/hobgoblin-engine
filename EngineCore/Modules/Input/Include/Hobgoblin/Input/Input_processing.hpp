#ifndef UHOBGOBLIN_INPUT_INPUT_PROCESSING_HPP
#define UHOBGOBLIN_INPUT_INPUT_PROCESSING_HPP

#include <Hobgoblin/Input/Universal_input_enumeration.hpp>
#include <Hobgoblin/Input/Physical_keyboard_key.hpp>
#include <Hobgoblin/Input/Virtual_keyboard_key.hpp>

#include <Hobgoblin/Private/Pmacro_define.hpp>

HOBGOBLIN_NAMESPACE_BEGIN
namespace in {

//! TODO
VirtualKeyboardKey Localize(PhysicalKeyboardKey aKey);

//! TODO
PhysicalKeyboardKey Delocalize(VirtualKeyboardKey aKey);

} // namespace in
HOBGOBLIN_NAMESPACE_END

#include <Hobgoblin/Private/Pmacro_undef.hpp>
#include <Hobgoblin/Private/Short_namespace.hpp>

#endif // !UHOBGOBLIN_INPUT_INPUT_PROCESSING_HPP
