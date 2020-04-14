
#include <Hobgoblin/Graphics/Builtin_fonts.hpp>

#include <cassert>

#include <Hobgoblin/Private/Pmacro_define.hpp>

namespace {
#include <Hobgoblin/Private/Font_cmu_serif_roman.hpp>
#include <Hobgoblin/Private/Font_cmu_typewriter_text.hpp>
#include <Hobgoblin/Private/Font_ebgaramond12_regular.hpp>
#include <Hobgoblin/Private/Font_inconsolata_regular.hpp>
#include <Hobgoblin/Private/Font_titillium_webregular.hpp>
} // namespace

HOBGOBLIN_NAMESPACE_START
namespace gr {

const sf::Font& BuiltInFonts::getFont(FontChoice fontChoice) {
    switch (fontChoice) {
    case ComputerModernSerifRoman:
        return getCmuSerifRoman();

    case ComputerModernTypewriterText:
        return getCmuTypewriterText();

    case EbGaramond12Regular:
        return getEbGaramond12Regular();

    case InconsolataRegular:
        return getInconsolataRegular();

    case TitilliumRegular:
        return getTitilliumRegular();

    default:
        assert(0 && "Unreachable");
    }
}

const sf::Font& BuiltInFonts::getCmuSerifRoman() {
    static bool fontInitialized{false};
    static sf::Font font;
    if (!fontInitialized) {
        assert(font.loadFromMemory(cmu_serif_roman_ttf, cmu_serif_roman_ttf_len));
        fontInitialized = true;
    }
    return font;
}

const sf::Font& BuiltInFonts::getCmuTypewriterText() {
    static bool fontInitialized{false};
    static sf::Font font;
    if (!fontInitialized) {
        assert(font.loadFromMemory(cmuntt_ttf, cmuntt_ttf_len));
        fontInitialized = true;
    }
    return font;
}

const sf::Font& BuiltInFonts::getEbGaramond12Regular() {
    static bool fontInitialized{false};
    static sf::Font font;
    if (!fontInitialized) {
        assert(font.loadFromMemory(EBGaramond12_Regular_ttf, EBGaramond12_Regular_ttf_len));
        fontInitialized = true;
    }
    return font;
}

const sf::Font& BuiltInFonts::getInconsolataRegular() {
    static bool fontInitialized{false};
    static sf::Font font;
    if (!fontInitialized) {
        assert(font.loadFromMemory(Inconsolata_Regular_ttf, Inconsolata_Regular_ttf_len));
        fontInitialized = true;
    }
    return font;
}

const sf::Font& BuiltInFonts::getTitilliumRegular() {
    static bool fontInitialized{false};
    static sf::Font font;
    if (!fontInitialized) {
        font.loadFromMemory(TitilliumWeb_Regular_ttf, TitilliumWeb_Regular_ttf_len);
        fontInitialized = true;
    }
    return font;
}

} // namespace gr
HOBGOBLIN_NAMESPACE_END

#include <Hobgoblin/Private/Pmacro_undef.hpp>
#include <Hobgoblin/Private/Short_namespace.hpp>