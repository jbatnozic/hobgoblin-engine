#include <Hobgoblin/Input/Universal_input_enumeration.hpp>

#include <SFML/Window/Keyboard.hpp>

#include <algorithm>
#include <optional>
#include <string>
#include <unordered_map>

#include "SFML_conversions.hpp"

#include <Hobgoblin/Private/Pmacro_define.hpp>

HOBGOBLIN_NAMESPACE_BEGIN
namespace in {

namespace {

bool StringReplace(std::string& aSource, const std::string& aTarget, const std::string& aReplacement) {
    const size_t start_pos = aSource.find(aTarget);
    if (start_pos == std::string::npos) {
        return false;
    }
    aSource.replace(start_pos, aTarget.length(), aReplacement);
    return true;
}

class ValueMapping {
public:
    ValueMapping() {
        #define XMACRO(_val_) _addMapping(_val_, #_val_);
        #include "Xmacro_universal_input_enum_values.inl"
        #undef XMCARO
    }

    std::optional<UniversalInputEnum> stringToValue(std::string aString) const {
        for (char& c : aString) {
            c = toupper(c);
        }

        const auto iter = _stringToValue.find(aString);
        if (iter == _stringToValue.end()) {
            return {};
        }
        return iter->second;
    }

    std::optional<std::string> valueToString(UniversalInputEnum aValue) const {
        const auto iter = _valueToString.find(aValue);
        if (iter == _valueToString.end()) {
            return {};
        }
        return iter->second;
    }

private:
    std::unordered_map<std::string, UniversalInputEnum> _stringToValue;
    std::unordered_map<UniversalInputEnum, std::string> _valueToString;

    void _addMapping(UniversalInputEnum aValue, std::string aName) {
        StringReplace(aName, "VK_", "VIRTUALKEY:");
        StringReplace(aName, "PK_", "PHYSICALKEY:");

        _stringToValue[aName] = aValue;
        _valueToString[aValue] = aName;
    }
};

const ValueMapping g_valueMap{};

} // namespace

std::optional<UniversalInputEnum> StringToInput(const std::string& aString) {
    return g_valueMap.stringToValue(aString);
}

std::optional<std::string> InputToString(UniversalInputEnum aValue) {
    return g_valueMap.valueToString(aValue);
}

bool CheckPressedVK(VirtualKeyboardKey aKey) {
    return sf::Keyboard::isKeyPressed((sf::Keyboard::Key)ToSfKeyboardKey(aKey));
}

std::optional<VirtualKeyboardKey> DetectPressedVK() {
    for (int i = HG_VIRTUALKEY_FIRST; i <= HG_VIRTUALKEY_LAST; i += 1) {
        const auto key = VirtualKeyboardKey{static_cast<UniversalInputEnum>(i)};
        if (CheckPressedVK(key)) {
            return key;
        }
    }
    return {};
}

bool CheckPressedPK(PhysicalKeyboardKey aKey) {
    return sf::Keyboard::isKeyPressed((sf::Keyboard::Scancode)ToSfScancode(aKey));
}

std::optional<PhysicalKeyboardKey> DetectPressedPK() {
    for (int i = HG_PHYSICALKEY_FIRST; i <= HG_PHYSICALKEY_LAST; i += 1) {
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
