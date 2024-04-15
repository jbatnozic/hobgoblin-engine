// Copyright 2024 Jovan Batnozic. Released under MS-PL licence in Serbia.
// See https://github.com/jbatnozic/Hobgoblin?tab=readme-ov-file#licence

// clang-format off


#include <Hobgoblin/Input/Private/Conversions.hpp>

#include "SFML_conversions.hpp"

#include <Hobgoblin/Private/Pmacro_define.hpp>

HOBGOBLIN_NAMESPACE_BEGIN
namespace in {
namespace detail {

std::int32_t SfKeyboardKeyToInputEnum(std::int32_t aValue) {
    return (std::int32_t)ToHgVirtualKeyboardKey(static_cast<sf::Keyboard::Key>(aValue));
}

std::int32_t SfKeyboardScancodeToInputEnum(std::int32_t aValue) {
    return (std::int32_t)ToHgPhysicalKeyboardKey(static_cast<sf::Keyboard::Scan::Scancode>(aValue));
}

std::int32_t SfMouseButtonToInputEnum(std::int32_t aValue) {
    return (std::int32_t)ToHgMouseButton(static_cast<sf::Mouse::Button>(aValue));
}

std::int32_t SfMouseWheelToInputEnum(std::int32_t aValue) {
    return (std::int32_t)ToHgMouseWheel(static_cast<sf::Mouse::Wheel>(aValue));
}

} // namespace detail
} // namespace in
HOBGOBLIN_NAMESPACE_END

#include <Hobgoblin/Private/Pmacro_undef.hpp>

// clang-format on
