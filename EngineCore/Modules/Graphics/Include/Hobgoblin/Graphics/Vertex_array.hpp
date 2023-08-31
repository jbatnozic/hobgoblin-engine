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

#include <Hobgoblin/Private/Pmacro_define.hpp>

HOBGOBLIN_NAMESPACE_BEGIN
namespace gr {

namespace detail {
class GraphicsImplAccessor;
} // namespace detail

//! \brief Define a set of one or more 2D primitives.
class VertexArray : public Drawable {
public:
    //! \brief Default constructor
    //!
    //! Creates an empty vertex array.
    VertexArray();

    //! \brief Construct the vertex array with a type and an initial number of vertices
    //!
    //! \param type        Type of primitives
    //! \param vertexCount Initial number of vertices in the array
    explicit VertexArray(PrimitiveType aType, PZInteger aVertexCount = 0);

    //! TODO
    ~VertexArray();

    //! \brief Return the vertex count
    //!
    //! \return Number of vertices in the array
    PZInteger getVertexCount() const;

    //! \brief Get a read-write access to a vertex by its index
    //!
    //! This function doesn't check \a index, it must be in range
    //! [0, getVertexCount() - 1]. The behavior is undefined
    //! otherwise.
    //!
    //! \param index Index of the vertex to get
    //!
    //! \return Reference to the index-th vertex
    //!
    //! \see getVertexCount
    // Vertex& operator[](PZInteger aIndex); TODO

    //! \brief Get a read-only access to a vertex by its index
    //!
    //! This function doesn't check \a index, it must be in range
    //! [0, getVertexCount() - 1]. The behavior is undefined
    //! otherwise.
    //!
    //! \param index Index of the vertex to get
    //!
    //! \return Const reference to the index-th vertex
    //!
    //! \see getVertexCount
    // const Vertex& operator[](PZInteger aIndex) const; TODO

    //! \brief Clear the vertex array
    //!
    //! This function removes all the vertices from the array.
    //! It doesn't deallocate the corresponding memory, so that
    //! adding new vertices after clearing doesn't involve
    //! reallocating all the memory.
    void clear();

    //! \brief Resize the vertex array
    //!
    //! If \a vertexCount is greater than the current size, the previous
    //! vertices are kept and new (default-constructed) vertices are
    //! added.
    //! If \a vertexCount is less than the current size, existing vertices
    //! are removed from the array.
    //!
    //! \param vertexCount New size of the array (number of vertices)
    void resize(PZInteger aVertexCount);

    //! \brief Add a vertex to the array
    //!
    //! \param vertex Vertex to add
    void append(const Vertex& aVertex);

    //! \brief Set the type of primitives to draw
    //!
    //! This function defines how the vertices must be interpreted
    //! when it's time to draw them:
    //! \li As points
    //! \li As lines
    //! \li As triangles
    //! \li As quads
    //! The default primitive type is sf::Points.
    //!
    //! \param type Type of primitive
    void setPrimitiveType(PrimitiveType aType);

    //! \brief Get the type of primitives drawn by the vertex array
    //!
    //! \return Primitive type
    PrimitiveType getPrimitiveType() const;

    //! \brief Compute the bounding rectangle of the vertex array
    //!
    //! This function returns the minimal axis-aligned rectangle
    //! that contains all the vertices of the array.
    //!
    //! \return Bounding rectangle of the vertex array
    math::Rectangle<float> getBounds() const;

    BatchingType getBatchingType() const {
        return BatchingType::VertexArray;
    }

private:
    //! \brief Draw the vertex array to a render target
    //!
    //! \param target Render target to draw to
    //! \param states Current render states
    void _draw(Canvas& aCanvas, const RenderStates& aStates) const override;

private:
    friend class detail::GraphicsImplAccessor;

    void* _getSFMLImpl();
    const void* _getSFMLImpl() const;

    static constexpr std::size_t STORAGE_SIZE  = 48;
    static constexpr std::size_t STORAGE_ALIGN =  8;
    std::aligned_storage<STORAGE_SIZE, STORAGE_ALIGN>::type _storage;
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
