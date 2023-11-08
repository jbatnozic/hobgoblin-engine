#include <Hobgoblin/Input/Physical_keyboard_key.hpp>

#include <SFML/Window/Keyboard.hpp>

#include "SFML_conversions.hpp"

#include <Hobgoblin/Private/Pmacro_define.hpp>

HOBGOBLIN_NAMESPACE_BEGIN
namespace in {

bool CheckPressedPK(PhysicalKeyboardKey aKey) {
    return sf::Keyboard::isKeyPressed((sf::Keyboard::Scancode)ToSfScancode(aKey));
}

std::optional<PhysicalKeyboardKey> DetectPressedPK() {
    for (int i = HG_INPUT_PHYSICALKEY_FIRST; i <= HG_INPUT_PHYSICALKEY_LAST; i += 1) {
        const auto key = PhysicalKeyboardKey{static_cast<UniversalInputEnum>(i)};
        if (CheckPressedPK(key)) {
            return key;
        }
    }
    return {};
}

} // namespace in
HOBGOBLIN_NAMESPACE_END

#include <Hobgoblin/Private/Pmacro_undef.hpp>
