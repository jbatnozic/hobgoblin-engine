#ifndef UHOBGOBLIN_GR_COLOR_HPP
#define UHOBGOBLIN_GR_COLOR_HPP

#include <SFML/Graphics/Color.hpp>

#include <Hobgoblin/Private/Pmacro_define.hpp>

HOBGOBLIN_NAMESPACE_START
namespace gr {

struct Color {
public:
    static const sf::Color Transparent;

#define XMACRO(_name_, _hexcode_) static const sf::Color _name_ ;
#include <Hobgoblin/Private/Xmacro_html_colors.hpp>
#undef  XMACRO
};

} // namespace gr
HOBGOBLIN_NAMESPACE_END

#include <Hobgoblin/Private/Pmacro_undef.hpp>
#include <Hobgoblin/Private/Short_namespace.hpp>

#endif // !UHOBGOBLIN_GR_COLOR_HPP