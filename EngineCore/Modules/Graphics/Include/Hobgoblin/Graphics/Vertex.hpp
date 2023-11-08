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

#ifndef UHOBGOBLIN_GRAPHICS_VERTEX_HPP
#define UHOBGOBLIN_GRAPHICS_VERTEX_HPP

#include <Hobgoblin/Graphics/Color.hpp>
#include <Hobgoblin/Math/Vector.hpp>

#include <Hobgoblin/Private/Pmacro_define.hpp>

HOBGOBLIN_NAMESPACE_BEGIN
namespace gr {

namespace detail {
class GraphicsImplAccessor;
} // namespace detail

//! A vertex defines a point with color and texture coordinates.
//! TODO: add constexpr to stuff when math::Vector2 supports it
class Vertex {
public:

    ////////////////////////////////////////////////////////////
    /// \brief Default constructor
    ///
    ////////////////////////////////////////////////////////////
    /*constexpr*/ Vertex() = default;

    ////////////////////////////////////////////////////////////
    /// \brief Construct the vertex from its position
    ///
    /// The vertex color is white and texture coordinates are (0, 0).
    ///
    /// \param aPosition Vertex position.
    ///
    ////////////////////////////////////////////////////////////
    /*constexpr*/ Vertex(const math::Vector2f& aPosition) : position{aPosition} {}

    ////////////////////////////////////////////////////////////
    /// \brief Construct the vertex from its position and color
    ///
    /// The texture coordinates are (0, 0).
    ///
    /// \param aPosition Vertex position.
    /// \param aColor    Vertex color.
    ///
    ////////////////////////////////////////////////////////////
    /*constexpr*/ Vertex(const math::Vector2f& aPosition, const Color& aColor)
        : position{aPosition}, color{aColor} {}

    ////////////////////////////////////////////////////////////
    /// \brief Construct the vertex from its position and texture coordinates
    ///
    /// The vertex color is white.
    ///
    /// \param aPosition  Vertex position.
    /// \param aTexCoords Vertex texture coordinates.
    ///
    ////////////////////////////////////////////////////////////
    /*constexpr*/ Vertex(const math::Vector2f& aPosition, const math::Vector2f& aTexCoords)
        : position{aPosition}, texCoords{aTexCoords} {}

    ////////////////////////////////////////////////////////////
    /// \brief Construct the vertex from its position, color and texture coordinates
    ///
    /// \param aPosition  Vertex position.
    /// \param aColor     Vertex color.
    /// \param aTexCoords Vertex texture coordinates.
    ///
    ////////////////////////////////////////////////////////////
    /*constexpr*/ Vertex(const math::Vector2f& aPosition, const Color& aColor, const math::Vector2f& aTexCoords)
        : position{aPosition}, color{aColor}, texCoords{aTexCoords} {}

    ////////////////////////////////////////////////////////////
    // Member data
    ////////////////////////////////////////////////////////////
    math::Vector2f  position{0.f, 0.f};  //!< 2D position of the vertex
    Color           color{COLOR_WHITE};  //!< Color of the vertex
    math::Vector2f  texCoords{0.f, 0.f}; //!< Coordinates of the texture's pixel to map to the vertex
};

} // namespace gr
HOBGOBLIN_NAMESPACE_END

#include <Hobgoblin/Private/Pmacro_undef.hpp>
#include <Hobgoblin/Private/Short_namespace.hpp>

#endif // !HOBGOBLIN_GRAPHICS_VERTEX_HPP

////////////////////////////////////////////////////////////
/// \class sf::Vertex
/// \ingroup graphics
///
/// A vertex is an improved point. It has a position and other
/// extra attributes that will be used for drawing: in SFML,
/// vertices also have a color and a pair of texture coordinates.
///
/// The vertex is the building block of drawing. Everything which
/// is visible on screen is made of vertices. They are grouped
/// as 2D primitives (triangles, quads, ...), and these primitives
/// are grouped to create even more complex 2D entities such as
/// sprites, texts, etc.
///
/// If you use the graphical entities of SFML (sprite, text, shape)
/// you won't have to deal with vertices directly. But if you want
/// to define your own 2D entities, such as tiled maps or particle
/// systems, using vertices will allow you to get maximum performances.
///
/// Example:
/// \code
/// // define a 100x100 square, red, with a 10x10 texture mapped on it
/// sf::Vertex vertices[] =
/// {
///     sf::Vertex(sf::Vector2f(  0,   0), sf::Color::Red, sf::Vector2f( 0,  0)),
///     sf::Vertex(sf::Vector2f(  0, 100), sf::Color::Red, sf::Vector2f( 0, 10)),
///     sf::Vertex(sf::Vector2f(100, 100), sf::Color::Red, sf::Vector2f(10, 10)),
///     sf::Vertex(sf::Vector2f(100,   0), sf::Color::Red, sf::Vector2f(10,  0))
/// };
///
/// // draw it
/// window.draw(vertices, 4, sf::Quads);
/// \endcode
///
/// Note: although texture coordinates are supposed to be an integer
/// amount of pixels, their type is float because of some buggy graphics
/// drivers that are not able to process integer coordinates correctly.
///
/// \see sf::VertexArray
///
////////////////////////////////////////////////////////////
