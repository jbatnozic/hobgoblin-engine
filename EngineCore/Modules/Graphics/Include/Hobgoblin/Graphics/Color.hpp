#ifndef UHOBGOBLIN_GRAPHICS_COLOR_HPP
#define UHOBGOBLIN_GRAPHICS_COLOR_HPP

#include <cstdint>

#include <Hobgoblin/Private/Pmacro_define.hpp>

HOBGOBLIN_NAMESPACE_BEGIN
namespace gr {

class Color {
public:
  constexpr Color() = default;

  //! Bits 31..24 = Red, Bits 23..16 = Green, Bits 15..8 = Blue, 7..0 = Alpha
  constexpr explicit Color(std::uint32_t aColor)
      : r{(aColor >> 24) & 0xFF}
      , g{(aColor >> 16) & 0xFF}
      , b{(aColor >>  8) & 0xFF}
      , a{(aColor >>  0) & 0xFF}
  {}

  constexpr Color(
    std::uint8_t aRed,
    std::uint8_t aGreen,
    std::uint8_t aBlue,
    std::uint8_t aAlpha = 255
  )
      : r{aRed}, g{aGreen}, b{aBlue}, a{aAlpha} {}

  std::uint32_t toInt() const;

  std::uint8_t r = 0;
  std::uint8_t g = 0;
  std::uint8_t b = 0;
  std::uint8_t a = 255;
};

bool operator==(Color aLhs, Color aRhs);

#define XMACRO(_name_, _hexcode_) constexpr Color COLOR_##_name_{_hexcode_};
// TODO: missing transparent colour
#include <Hobgoblin/Graphics/Private/Xmacro_html_colors.hpp>
#undef  XMACRO

} // namespace gr
HOBGOBLIN_NAMESPACE_END

#include <Hobgoblin/Private/Pmacro_undef.hpp>
#include <Hobgoblin/Private/Short_namespace.hpp>

#endif // !UHOBGOBLIN_GRAPHICS_COLOR_HPP
