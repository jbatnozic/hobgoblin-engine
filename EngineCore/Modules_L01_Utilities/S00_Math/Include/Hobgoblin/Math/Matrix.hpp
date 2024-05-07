// Copyright 2024 Jovan Batnozic. Released under MS-PL licence in Serbia.
// See https://github.com/jbatnozic/Hobgoblin?tab=readme-ov-file#licence

#ifndef UHOBGOBLIN_MATH_MATRIX_HPP
#define UHOBGOBLIN_MATH_MATRIX_HPP

#include <Hobgoblin/Common.hpp>
#include <array>

#include <Hobgoblin/Private/Pmacro_define.hpp>

HOBGOBLIN_NAMESPACE_BEGIN
namespace math {

template <class taArithmetic, PZInteger taWidth, PZInteger taHeight>
class Matrix {
public:
    static constexpr PZInteger WIDTH  = taWidth;
    static constexpr PZInteger HEIGHT = taHeight;

    std::array<taArithmetic, static_cast<std::size_t>(WIDTH * HEIGHT)> data;
};

} // namespace math
HOBGOBLIN_NAMESPACE_END

#include <Hobgoblin/Private/Pmacro_undef.hpp>
#include <Hobgoblin/Private/Short_namespace.hpp>

#endif // !UHOBGOBLIN_MATH_MATRIX_HPP