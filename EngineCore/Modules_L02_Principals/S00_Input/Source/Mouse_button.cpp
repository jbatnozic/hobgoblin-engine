// Copyright 2024 Jovan Batnozic. Released under MS-PL licence in Serbia.
// See https://github.com/jbatnozic/Hobgoblin?tab=readme-ov-file#licence

#include <Hobgoblin/Input/Mouse_button.hpp>

#include <SFML/Window/Mouse.hpp>

#include "SFML_conversions.hpp"

#include <Hobgoblin/Private/Pmacro_define.hpp>

HOBGOBLIN_NAMESPACE_BEGIN
namespace in {

bool CheckPressedMB(MouseButton aKey) {
    if (aKey.val() == MB_UNKNOWN) {
        return false;
    }
    return sf::Mouse::isButtonPressed(ToSfMouseButton(aKey));
}

std::optional<MouseButton> DetectPressedMB() {
    for (int i = HG_INPUT_MOUSEBUTTON_FIRST; i <= HG_INPUT_MOUSEBUTTON_LAST; i += 1) {
        const auto key = MouseButton{static_cast<UniversalInputEnum>(i)};
        if (CheckPressedMB(key)) {
            return key;
        }
    }
    return {};
}

} // namespace in
HOBGOBLIN_NAMESPACE_END

#include <Hobgoblin/Private/Pmacro_undef.hpp>
