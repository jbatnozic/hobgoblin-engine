#ifndef UHOBGOBLIN_INPUT_PRIVATE_CONVERSIONS_HPP
#define UHOBGOBLIN_INPUT_PRIVATE_CONVERSIONS_HPP

#include <cstdint>

#include <Hobgoblin/Private/Pmacro_define.hpp>

HOBGOBLIN_NAMESPACE_BEGIN
namespace in {
namespace detail {

// NOTE: These function are needed by the Window module to convert SFML events.

//! Converts a value of `sf::Keyboard::Key` into a value that can be cast
//! into `UniversalInputEnum` (will have one of the VK_* values).
//! For internal engine use only.
std::int32_t SfKeyboardKeyToInputEnum(std::int32_t aValue);

//! Converts a value of `sf::Keyboard::Scancode` into a value that can be cast
//! into `UniversalInputEnum` (will have one of the PK_* values).
//! For internal engine use only.
std::int32_t SfKeyboardScancodeToInputEnum(std::int32_t aValue);

//! Converts a value of `sf::Mouse::Button` into a value that can be cast
//! into `UniversalInputEnum` (will have one of the MB_* values).
//! For internal engine use only.
std::int32_t SfMouseButtonToInputEnum(std::int32_t aValue);

//! Converts a value of `sf::Mouse::Wheel` into a value that can be cast
//! into `UniversalInputEnum` (will have one of the MW_* values).
//! For internal engine use only.
std::int32_t SfMouseWheelToInputEnum(std::int32_t aValue);

} // namespace detail
} // namespace in
HOBGOBLIN_NAMESPACE_END

#include <Hobgoblin/Private/Pmacro_undef.hpp>
#include <Hobgoblin/Private/Short_namespace.hpp>

#endif // !UHOBGOBLIN_INPUT_PRIVATE_CONVERSIONS_HPP
