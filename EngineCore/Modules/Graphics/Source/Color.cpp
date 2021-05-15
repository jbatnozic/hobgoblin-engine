
#include <Hobgoblin/Graphics/Color.hpp>

#include <algorithm>

#include <Hobgoblin/Private/Pmacro_define.hpp>

HOBGOBLIN_NAMESPACE_BEGIN
namespace gr {

const Color Color::Transparent{0x00000000};

#define XMACRO(_name_, _hexcode_) const Color Color:: _name_ {_hexcode_};
#include <Hobgoblin/Graphics/Private/Xmacro_html_colors.hpp>
#undef  XMACRO


Color MultiplyColors(Color c1, Color c2) {
    Color rv;
    rv.r = std::min(255, int{c1.r} * int{c2.r} / 255);
    rv.g = std::min(255, int{c1.g} * int{c2.g} / 255);
    rv.b = std::min(255, int{c1.b} * int{c2.b} / 255);
    rv.a = std::min(255, int{c1.a} * int{c2.a} / 255);
    return rv;
}

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