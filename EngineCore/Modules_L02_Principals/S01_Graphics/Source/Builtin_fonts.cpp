
#include <Hobgoblin/Graphics/Builtin_fonts.hpp>

#include <Hobgoblin/HGExcept.hpp>

#include <Hobgoblin/Private/Pmacro_define.hpp>

namespace {
#include "BuiltinFonts/Font_cmu_serif_roman.hpp"
#include "BuiltinFonts/Font_cmu_typewriter_text.hpp"
#include "BuiltinFonts/Font_ebgaramond12_regular.hpp"
#include "BuiltinFonts/Font_inconsolata_regular.hpp"
#include "BuiltinFonts/Font_titillium_webregular.hpp"
} // namespace

HOBGOBLIN_NAMESPACE_BEGIN
namespace gr {

const Font& BuiltInFonts::getFont(FontChoice aFontChoice) {
    switch (aFontChoice) {
    case COMPUTER_MODERN_SERIF_ROMAN:
        return getCmuSerifRoman();

    case COMPUTER_MODERN_TYPEWRITER_TEXT:
        return getCmuTypewriterText();

    case EB_GARAMOND_12_REGULAR:
        return getEbGaramond12Regular();

    case INCONSOLATA_REGULAR:
        return getInconsolataRegular();

    case TITILLIUM_REGULAR:
        return getTitilliumRegular();

    default:
        HG_UNREACHABLE("Invalid value for BuiltInFonts::FontChoice ({}).", (int)aFontChoice);
    }
}

const Font& BuiltInFonts::getCmuSerifRoman() {
    static bool fontInitialized{false};
    static Font font;
    if (!fontInitialized) {
        font.loadFromMemory(cmu_serif_roman_ttf, cmu_serif_roman_ttf_len);
        fontInitialized = true;
    }
    return font;
}

const Font& BuiltInFonts::getCmuTypewriterText() {
    static bool fontInitialized{false};
    static Font font;
    if (!fontInitialized) {
        font.loadFromMemory(cmuntt_ttf, cmuntt_ttf_len);
        fontInitialized = true;
    }
    return font;
}

const Font& BuiltInFonts::getEbGaramond12Regular() {
    static bool fontInitialized{false};
    static Font font;
    if (!fontInitialized) {
        font.loadFromMemory(EBGaramond12_Regular_ttf, EBGaramond12_Regular_ttf_len);
        fontInitialized = true;
    }
    return font;
}

const Font& BuiltInFonts::getInconsolataRegular() {
    static bool fontInitialized{false};
    static Font font;
    if (!fontInitialized) {
        font.loadFromMemory(Inconsolata_Regular_ttf, Inconsolata_Regular_ttf_len);
        fontInitialized = true;
    }
    return font;
}

const Font& BuiltInFonts::getTitilliumRegular() {
    static bool fontInitialized{false};
    static Font font;
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