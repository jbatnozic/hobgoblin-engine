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

inline
Color MultiplyColors(Color c1, Color c2) {
    Color rv;
    rv.r = std::min(255, int{c1.r} * int{c2.r} / 255);
    rv.g = std::min(255, int{c1.g} * int{c2.g} / 255);
    rv.b = std::min(255, int{c1.b} * int{c2.b} / 255);
    rv.a = std::min(255, int{c1.a} * int{c2.a} / 255);
    return rv;
}

inline
Color AddColors(Color c1, Color c2) {
    Color rv;
    rv.r = std::min(255, int{c1.r} + int{c2.r});
    rv.g = std::min(255, int{c1.g} + int{c2.g});
    rv.b = std::min(255, int{c1.b} + int{c2.b});
    rv.a = std::min(255, int{c1.a} + int{c2.a});
    return rv;
}

} // namespace gr
HOBGOBLIN_NAMESPACE_END

#include <Hobgoblin/Private/Pmacro_undef.hpp>
#include <Hobgoblin/Private/Short_namespace.hpp>

#endif // !UHOBGOBLIN_GR_COLOR_HPP