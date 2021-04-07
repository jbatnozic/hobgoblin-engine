#ifndef UHOBGOBLIN_GR_BUILTIN_FONTS_HPP
#define UHOBGOBLIN_GR_BUILTIN_FONTS_HPP

#include <SFML/Graphics/Font.hpp>

#include <Hobgoblin/Private/Pmacro_define.hpp>

HOBGOBLIN_NAMESPACE_START
namespace gr {

class BuiltInFonts {
public:
    enum FontChoice {
        /// A nice serif roman font.
        /// SOURCE: https://www.1001fonts.com/cmu-font.html#styles
        ComputerModernSerifRoman,

        /// A monospaced typewriter-style font.
        /// SOURCE: https://www.1001fonts.com/cmu-font.html#styles
        ComputerModernTypewriterText,

        /// Old classic serif font.
        /// SOURCE: https://github.com/georgd/EB-Garamond
        EbGaramond12Regular,

        /// Clean, modern monospace font.
        /// SOURCE: https://fonts.google.com/specimen/Inconsolata
        InconsolataRegular,

        /// A modern looking sans-serif font.
        /// SOURCE: https://fonts.google.com/specimen/Titillium+Web
        TitilliumRegular
    };

    static const sf::Font& getFont(FontChoice fontChoice);

private:
    static const sf::Font& getCmuSerifRoman();
    static const sf::Font& getCmuTypewriterText();
    static const sf::Font& getEbGaramond12Regular();
    static const sf::Font& getInconsolataRegular();
    static const sf::Font& getTitilliumRegular();
};

} // namespace gr
HOBGOBLIN_NAMESPACE_END

#include <Hobgoblin/Private/Pmacro_undef.hpp>
#include <Hobgoblin/Private/Short_namespace.hpp>

#endif // !UHOBGOBLIN_GR_BUILTIN_FONTS_HPP