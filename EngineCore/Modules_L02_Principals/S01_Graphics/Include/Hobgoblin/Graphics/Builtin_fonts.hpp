#ifndef UHOBGOBLIN_GR_BUILTIN_FONTS_HPP
#define UHOBGOBLIN_GR_BUILTIN_FONTS_HPP

#include <Hobgoblin/Graphics/Font.hpp>

#include <Hobgoblin/Private/Pmacro_define.hpp>

HOBGOBLIN_NAMESPACE_BEGIN
namespace gr {

class BuiltInFonts {
public:
    enum FontChoice {
        /// A nice serif roman font.
        /// SOURCE: https://www.1001fonts.com/cmu-font.html#styles
        COMPUTER_MODERN_SERIF_ROMAN,

        /// A monospaced typewriter-style font.
        /// SOURCE: https://www.1001fonts.com/cmu-font.html#styles
        COMPUTER_MODERN_TYPEWRITER_TEXT,

        /// Old classic serif font.
        /// SOURCE: https://github.com/georgd/EB-Garamond
        EB_GARAMOND_12_REGULAR,

        /// Clean, modern monospace font.
        /// SOURCE: https://fonts.google.com/specimen/Inconsolata
        INCONSOLATA_REGULAR,

        /// A modern looking sans-serif font.
        /// SOURCE: https://fonts.google.com/specimen/Titillium+Web
        TITILLIUM_REGULAR
    };

    static const Font& getFont(FontChoice aFontChoice);

private:
    static const Font& getCmuSerifRoman();
    static const Font& getCmuTypewriterText();
    static const Font& getEbGaramond12Regular();
    static const Font& getInconsolataRegular();
    static const Font& getTitilliumRegular();
};

} // namespace gr
HOBGOBLIN_NAMESPACE_END

#include <Hobgoblin/Private/Pmacro_undef.hpp>
#include <Hobgoblin/Private/Short_namespace.hpp>

#endif // !UHOBGOBLIN_GR_BUILTIN_FONTS_HPP