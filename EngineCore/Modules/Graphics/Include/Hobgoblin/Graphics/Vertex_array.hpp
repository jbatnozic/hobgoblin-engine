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

#ifndef UHOBGOBLIN_GRAPHICS_VERTEX_ARRAY_HPP
#define UHOBGOBLIN_GRAPHICS_VERTEX_ARRAY_HPP

#include <Hobgoblin/Common.hpp>
#include <Hobgoblin/Graphics/Vertex.hpp>
#include <Hobgoblin/Graphics/Primitive_type.hpp>
#include <Hobgoblin/Graphics/Drawable.hpp>
#include <Hobgoblin/Math/Rectangle.hpp>

#include <type_traits>
#include <vector>

#include <Hobgoblin/Private/Pmacro_define.hpp>

HOBGOBLIN_NAMESPACE_BEGIN
namespace gr {

//! TODO(description)
class VertexArray : public Drawable {
public:
    //! Vector of individual vertices.
    std::vector<Vertex> vertices;

    //! The type of primitives to draw. This field defines how the 
    //! vertices must be interpreted when it's time to draw them.
    PrimitiveType primitiveType;

    //! \brief Compute the bounding rectangle of the vertex array
    //!
    //! This function returns the minimal axis-aligned rectangle
    //! that contains all the vertices of the array.
    //!
    //! \return Bounding rectangle of the vertex array
    math::Rectangle<float> calculateBounds() const;

    BatchingType getBatchingType() const override;

private:
    void _draw(Canvas& aCanvas, const RenderStates& aStates) const override;
};

} // namespace gr
HOBGOBLIN_NAMESPACE_END

#include <Hobgoblin/Private/Pmacro_undef.hpp>
#include <Hobgoblin/Private/Short_namespace.hpp>

#endif // !UHOBGOBLIN_GRAPHICS_VERTEX_ARRAY_HPP

////////////////////////////////////////////////////////////
/// \class sf::VertexArray
/// \ingroup graphics
///
/// sf::VertexArray is a very simple wrapper around a dynamic
/// array of vertices and a primitives type.
///
/// It inherits sf::Drawable, but unlike other drawables it
/// is not transformable.
///
/// Example:
/// \code
/// sf::VertexArray lines(sf::LineStrip, 4);
/// lines[0].position = sf::Vector2f(10, 0);
/// lines[1].position = sf::Vector2f(20, 0);
/// lines[2].position = sf::Vector2f(30, 5);
/// lines[3].position = sf::Vector2f(40, 2);
///
/// window.draw(lines);
/// \endcode
///
/// \see sf::Vertex
///
////////////////////////////////////////////////////////////
