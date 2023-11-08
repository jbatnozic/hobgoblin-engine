#ifndef UHOBGOBLIN_INPUT_PHYSICAL_KEYBOARD_KEY_HPP
#define UHOBGOBLIN_INPUT_PHYSICAL_KEYBOARD_KEY_HPP

#include <Hobgoblin/Input/Universal_input_enumeration.hpp>

#include <optional>

#include <Hobgoblin/Private/Pmacro_define.hpp>

HOBGOBLIN_NAMESPACE_BEGIN
namespace in {

//! Class used to represent a physical keyboard key in a type-safe way.
class PhysicalKeyboardKey {
public:
    PhysicalKeyboardKey() = default;

    constexpr PhysicalKeyboardKey(UniversalInputEnum aValue)
        : _value{aValue}
    {
        HARD_ASSERT(IsPhysicalKeyboardKey(aValue));
    }

    constexpr UniversalInputEnum val() const {
        return _value;
    }

    constexpr operator UniversalInputEnum() const {
        return _value;
    }

private:
    UniversalInputEnum _value = PK_UNKNOWN;
};

bool CheckPressedPK(PhysicalKeyboardKey aKey);

std::optional<PhysicalKeyboardKey> DetectPressedPK();

} // namespace in
HOBGOBLIN_NAMESPACE_END

#include <Hobgoblin/Private/Pmacro_undef.hpp>
#include <Hobgoblin/Private/Short_namespace.hpp>

#endif // !UHOBGOBLIN_INPUT_PHYSICAL_KEYBOARD_KEY_HPP
