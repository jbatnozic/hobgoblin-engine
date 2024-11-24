// Copyright 2024 Jovan Batnozic. Released under MS-PL licence in Serbia.
// See https://github.com/jbatnozic/Hobgoblin?tab=readme-ov-file#licence

#ifndef UHOBGOBLIN_INPUT_VIRTUAL_KEYBOARD_KEY_HPP
#define UHOBGOBLIN_INPUT_VIRTUAL_KEYBOARD_KEY_HPP

#include <Hobgoblin/HGExcept.hpp>
#include <Hobgoblin/Input/Universal_input_enumeration.hpp>

#include <optional>

#include <Hobgoblin/Private/Pmacro_define.hpp>

HOBGOBLIN_NAMESPACE_BEGIN
namespace in {

//! Class used to represent a virtual keyboard key in a type-safe way.
class VirtualKeyboardKey {
public:
    VirtualKeyboardKey() = default;

    constexpr VirtualKeyboardKey(UniversalInputEnum aValue)
        : _value{aValue} {
        HG_VALIDATE_ARGUMENT(IsVirtualKeyboardKey(aValue) || aValue == VK_UNKNOWN,
                             "{} is not a valid VirtualKeyboardKey value.",
                             (int)aValue);
    }

    constexpr UniversalInputEnum val() const {
        return _value;
    }

    constexpr operator UniversalInputEnum() const {
        return _value;
    }

private:
    UniversalInputEnum _value = VK_UNKNOWN;
};

//! Returns `true` if the given virtual key is currently pressed,
//! and `false` otherwise. (checks the state of the keyboard directly)
bool CheckPressedVK(VirtualKeyboardKey aKey);

std::optional<VirtualKeyboardKey> DetectPressedVK();

} // namespace in
HOBGOBLIN_NAMESPACE_END

#include <Hobgoblin/Private/Pmacro_undef.hpp>
#include <Hobgoblin/Private/Short_namespace.hpp>

#endif // !UHOBGOBLIN_INPUT_VIRTUAL_KEYBOARD_KEY_HPP
