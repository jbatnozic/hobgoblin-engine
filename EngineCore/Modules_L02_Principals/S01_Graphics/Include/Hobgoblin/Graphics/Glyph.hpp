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

#ifndef UHOBGOBLIN_GRAPHICS_GLYPH_HPP
#define UHOBGOBLIN_GRAPHICS_GLYPH_HPP

#include <Hobgoblin/Graphics/Texture_rect.hpp>
#include <Hobgoblin/Math/Rectangle.hpp>

#include <Hobgoblin/Private/Pmacro_define.hpp>

HOBGOBLIN_NAMESPACE_BEGIN
namespace gr {

/// \brief Structure describing a glyph (a visual representation of a character).
struct Glyph {
    //! Offset to move horizontally to the next character.
    float advance = 0.f;

    //! Left offset after forced autohint. Internally used by getKerning().
    int lsbDelta = 0;

    //! Right offset after forced autohint. Internally used by getKerning().
    int rsbDelta = 0;

    //! Bounding rectangle of the glyph, in coordinates relative to the baseline.
    math::Rectangle<float> bounds;     

    //! Texture coordinates of the glyph inside the font's texture.
    TextureRect textureRect;  
};

} // namespace gr
HOBGOBLIN_NAMESPACE_END

#include <Hobgoblin/Private/Pmacro_undef.hpp>
#include <Hobgoblin/Private/Short_namespace.hpp>

#endif // !UHOBGOBLIN_GRAPHICS_GLYPH_HPP

////////////////////////////////////////////////////////////
/// \class sf::Glyph
/// \ingroup graphics
///
/// A glyph is the visual representation of a character.
///
/// The sf::Glyph structure provides the information needed
/// to handle the glyph:
/// \li its coordinates in the font's texture
/// \li its bounding rectangle
/// \li the offset to apply to get the starting position of the next glyph
///
/// \see sf::Font
///
////////////////////////////////////////////////////////////
