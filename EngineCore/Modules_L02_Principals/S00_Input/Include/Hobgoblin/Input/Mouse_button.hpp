#ifndef UHOBGOBLIN_INPUT_MOUSE_BUTTON_HPP
#define UHOBGOBLIN_INPUT_MOUSE_BUTTON_HPP

#include <Hobgoblin/Input/Universal_input_enumeration.hpp>

#include <optional>

#include <Hobgoblin/Private/Pmacro_define.hpp>

HOBGOBLIN_NAMESPACE_BEGIN
namespace in {

//! Class used to represent a mouse button in a type-safe way.
class MouseButton {
public:
    MouseButton() = default;

    constexpr MouseButton(UniversalInputEnum aValue)
        : _value{aValue}
    {
        HARD_ASSERT(IsMouseButton(aValue));
    }

    constexpr UniversalInputEnum val() const {
        return _value;
    }

    constexpr operator UniversalInputEnum() const {
        return _value;
    }

private:
    UniversalInputEnum _value = MB_UNKNOWN;
};

bool CheckPressedMB(MouseButton aKey);

std::optional<MouseButton> DetectPressedMB();

} // namespace in
HOBGOBLIN_NAMESPACE_END

#include <Hobgoblin/Private/Pmacro_undef.hpp>
#include <Hobgoblin/Private/Short_namespace.hpp>

#endif // !UHOBGOBLIN_INPUT_MOUSE_BUTTON_HPP
