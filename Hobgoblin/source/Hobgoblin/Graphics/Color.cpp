
#include <Hobgoblin/Graphics/Color.hpp>

#include <Hobgoblin/Private/Pmacro_define.hpp>

HOBGOBLIN_NAMESPACE_START
namespace gr {

const Color Color::Transparent{0x00000000};

#define XMACRO(_name_, _hexcode_) const Color Color:: _name_ {_hexcode_};
#include <Hobgoblin/Private/Xmacro_html_colors.hpp>
#undef  XMACRO

} // namespace gr
HOBGOBLIN_NAMESPACE_END

#include <Hobgoblin/Private/Pmacro_undef.hpp>