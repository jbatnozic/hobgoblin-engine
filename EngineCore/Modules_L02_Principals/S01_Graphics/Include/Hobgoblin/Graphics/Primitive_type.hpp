// Code in this file is adapted from SFML code and retains its original
// open source licence (provided below).
// See https://github.com/SFML/SFML

////////////////////////////////////////////////////////////
//
// SFML - Simple and Fast Multimedia Library
// Copyright (C) 2007-2023 Laurent Gomila (laurent@sfml-dev.org)
//
// This software is provided 'as-is', without any express or implied warranty.
// In no event will the authors be held liable for any damages arising from the use of this software.
//
// Permission is granted to anyone to use this software for any purpose,
// including commercial applications, and to alter it and redistribute it freely,
// subject to the following restrictions:
//
// 1. The origin of this software must not be misrepresented;
//    you must not claim that you wrote the original software.
//    If you use this software in a product, an acknowledgment
//    in the product documentation would be appreciated but is not required.
//
// 2. Altered source versions must be plainly marked as such,
//    and must not be misrepresented as being the original software.
//
// 3. This notice may not be removed or altered from any source distribution.
//
////////////////////////////////////////////////////////////

// clang-format off

#ifndef UHOBGOBLIN_GRAPHICS_PRIMITIVETYPE_HPP
#define UHOBGOBLIN_GRAPHICS_PRIMITIVETYPE_HPP

#include <Hobgoblin/Private/Pmacro_define.hpp>

HOBGOBLIN_NAMESPACE_BEGIN
namespace gr {

//! \brief Types of primitives that a sf::VertexArray can render
//!
//! Points and lines have no area, therefore their thickness
//! will always be 1 pixel, regardless the current transform
//! and view.
enum class PrimitiveType {
    POINTS,         //!< List of individual points
    LINES,          //!< List of individual lines
    LINE_STRIP,     //!< List of connected lines, a point uses the previous point to form a line
    TRIANGLES,      //!< List of individual triangles
    TRIANGLE_STRIP, //!< List of connected triangles, a point uses the two previous points to form a triangle
    TRIANGLE_FAN,   //!< List of connected triangles, a point uses the common center and the previous point to form a triangle
    COUNT,          //!< Number of different primitive types. Always keep last in the enum.
};

} // namespace gr
HOBGOBLIN_NAMESPACE_END

#include <Hobgoblin/Private/Pmacro_undef.hpp>
#include <Hobgoblin/Private/Short_namespace.hpp>

#endif // !UHOBGOBLIN_GRAPHICS_PRIMITIVETYPE_HPP

// clang-format on
