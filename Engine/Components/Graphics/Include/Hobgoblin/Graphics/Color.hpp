#ifndef UHOBGOBLIN_GR_COLOR_HPP
#define UHOBGOBLIN_GR_COLOR_HPP

#include <SFML/Graphics/Color.hpp>

#include <Hobgoblin/Private/Pmacro_define.hpp>

HOBGOBLIN_NAMESPACE_START
namespace gr {

struct Color : public sf::Color {
public:
    using sf::Color::Color;

    Color(sf::Color other) {
        SELF.r = other.r;
        SELF.g = other.g;
        SELF.b = other.b;
        SELF.a = other.a;
    }

    static const Color Transparent;

#define XMACRO(_name_, _hexcode_) static const Color _name_ ;
#include <Hobgoblin/Private/Xmacro_html_colors.hpp>
#undef  XMACRO
};

// TODO These functions aren't exactly correct
Color AddColors(Color c1, Color c2);
Color MultiplyColors(Color c1, Color c2);

inline
Color BlendColors(Color src, Color dst, int blendMode) {
    // TODO
}

} // namespace gr
HOBGOBLIN_NAMESPACE_END

#include <Hobgoblin/Private/Pmacro_undef.hpp>
#include <Hobgoblin/Private/Short_namespace.hpp>

#endif // !UHOBGOBLIN_GR_COLOR_HPP