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

#ifndef UHOBGOBLIN_GRAPHICS_TRANSFORM_HPP
#define UHOBGOBLIN_GRAPHICS_TRANSFORM_HPP

#include <Hobgoblin/Math/Rectangle.hpp>
#include <Hobgoblin/Math/Angle.hpp>
#include <Hobgoblin/Math/Vector.hpp>

#include <type_traits>

#include <Hobgoblin/Private/Pmacro_define.hpp>

HOBGOBLIN_NAMESPACE_BEGIN
namespace gr {

namespace detail {
class GraphicsImplAccessor;
} // namespace detail

//! \brief Define a 3x3 transform matrix.
class Transform {
public:
    //! \brief Default constructor
    //!
    //! Creates an identity transform (a transform that does nothing).
    Transform();

    //! \brief Construct a transform from a 3x3 matrix
    //!
    //! \param a00 Element (0, 0) of the matrix
    //! \param a01 Element (0, 1) of the matrix
    //! \param a02 Element (0, 2) of the matrix
    //! \param a10 Element (1, 0) of the matrix
    //! \param a11 Element (1, 1) of the matrix
    //! \param a12 Element (1, 2) of the matrix
    //! \param a20 Element (2, 0) of the matrix
    //! \param a21 Element (2, 1) of the matrix
    //! \param a22 Element (2, 2) of the matrix
    Transform(float a00, float a01, float a02,
              float a10, float a11, float a12,
              float a20, float a21, float a22);

    Transform(const Transform& aOther);
    Transform& operator=(const Transform& aOther);

    Transform(Transform&& aOther) noexcept;
    Transform& operator=(Transform&& aOther) noexcept;

    //! \brief Destructor
    ~Transform();

    //! \brief Return the transform as a 4x4 matrix
    //!
    //! This function returns a pointer to an array of 16 floats
    //! containing the transform elements as a 4x4 matrix, which
    //! is directly compatible with OpenGL functions.
    //!
    //! \code
    //! sf::Transform transform = ...;
    //! glLoadMatrixf(transform.getMatrix());
    //! \endcode
    //!
    //! \return Pointer to a 4x4 matrix
    const float* getMatrix() const;

    //! \brief Return the inverse of the transform
    //!
    //! If the inverse cannot be computed, an identity transform
    //! is returned.
    //!
    //! \return A new transform which is the inverse of self
    Transform getInverse() const;

    //! \brief Transform a 2D point
    //!
    //! These two statements are equivalent:
    //! \code
    //! sf::Vector2f transformedPoint = matrix.transformPoint(x, y);
    //! sf::Vector2f transformedPoint = matrix * sf::Vector2f(x, y);
    //! \endcode
    //!
    //! \param aX X coordinate of the point to transform
    //! \param aY Y coordinate of the point to transform
    //!
    //! \return Transformed point
    math::Vector2f transformPoint(float aX, float aY) const;

    //! \brief Transform a 2D point
    //!
    //! These two statements are equivalent:
    //! \code
    //! sf::Vector2f transformedPoint = matrix.transformPoint(point);
    //! sf::Vector2f transformedPoint = matrix * point;
    //! \endcode
    //!
    //! \param aPoint Point to transform
    //!
    //! \return Transformed point
    math::Vector2f transformPoint(const math::Vector2f& aPoint) const;

    //! \brief Transform a rectangle
    //!
    //! Since SFML doesn't provide support for oriented rectangles,
    //! the result of this function is always an axis-aligned
    //! rectangle. Which means that if the transform contains a
    //! rotation, the bounding rectangle of the transformed rectangle
    //! is returned.
    //!
    //! \param aRectangle Rectangle to transform
    //!
    //! \return Transformed rectangle
    math::Rectangle<float> transformRect(const math::Rectangle<float>& aRectangle) const;

    //! \brief Combine the current transform with another one
    //!
    //! The result is a transform that is equivalent to applying
    //! \a transform followed by *this. Mathematically, it is
    //! equivalent to a matrix multiplication (*this) * transform.
    //!
    //! These two statements are equivalent:
    //! \code
    //! left.combine(right);
    //! left *= right;
    //! \endcode
    //!
    //! \param aTransform Transform to combine with this transform
    //!
    //! \return Reference to *this
    Transform& combine(const Transform& aTransform);

    //! \brief Combine the current transform with a translation
    //!
    //! This function returns a reference to *this, so that calls
    //! can be chained.
    //! \code
    //! sf::Transform transform;
    //! transform.translate(100, 200).rotate(45);
    //! \endcode
    //!
    //! \param aX Offset to apply on X axis
    //! \param aY Offset to apply on Y axis
    //!
    //! \return Reference to *this
    //!
    //! \see rotate, scale
    Transform& translate(float aX, float aY);

    //! \brief Combine the current transform with a translation
    //!
    //! This function returns a reference to *this, so that calls
    //! can be chained.
    //! \code
    //! sf::Transform transform;
    //! transform.translate(sf::Vector2f(100, 200)).rotate(45);
    //! \endcode
    //!
    //! \param aOffset Translation offset to apply
    //!
    //! \return Reference to *this
    //!
    //! \see rotate, scale
    Transform& translate(const math::Vector2f& aOffset);

    //! \brief Combine the current transform with a rotation
    //!
    //! This function returns a reference to *this, so that calls
    //! can be chained.
    //! \code
    //! sf::Transform transform;
    //! transform.rotate(90).translate(50, 20);
    //! \endcode
    //!
    //! \param aAngle Rotation angle, in degrees
    //!
    //! \return Reference to *this
    //!
    //! \see translate, scale
    Transform& rotate(math::AngleF aAngle);

    //! \brief Combine the current transform with a rotation
    //!
    //! The center of rotation is provided for convenience as a second
    //! argument, so that you can build rotations around arbitrary points
    //! more easily (and efficiently) than the usual
    //! translate(-center).rotate(angle).translate(center).
    //!
    //! This function returns a reference to *this, so that calls
    //! can be chained.
    //! \code
    //! sf::Transform transform;
    //! transform.rotate(90, 8, 3).translate(50, 20);
    //! \endcode
    //!
    //! \param aAngle Rotation angle, in degrees
    //! \param aCenterX X coordinate of the center of rotation
    //! \param aCenterY Y coordinate of the center of rotation
    //!
    //! \return Reference to *this
    //!
    //! \see translate, scale
    Transform& rotate(math::AngleF aAngle, float aCenterX, float aCenterY);

    //! \brief Combine the current transform with a rotation
    //!
    //! The center of rotation is provided for convenience as a second
    //! argument, so that you can build rotations around arbitrary points
    //! more easily (and efficiently) than the usual
    //! translate(-center).rotate(angle).translate(center).
    //!
    //! This function returns a reference to *this, so that calls
    //! can be chained.
    //! \code
    //! sf::Transform transform;
    //! transform.rotate(90, sf::Vector2f(8, 3)).translate(sf::Vector2f(50, 20));
    //! \endcode
    //!
    //! \param aAngle Rotation angle, in degrees
    //! \param aCenter Center of rotation
    //!
    //! \return Reference to *this
    //!
    //! \see translate, scale
    Transform& rotate(math::AngleF aAngle, const math::Vector2f& aCenter);

    //! \brief Combine the current transform with a scaling
    //!
    //! This function returns a reference to *this, so that calls
    //! can be chained.
    //! \code
    //! sf::Transform transform;
    //! transform.scale(2, 1).rotate(45);
    //! \endcode
    //!
    //! \param scaleX Scaling factor on the X axis
    //! \param scaleY Scaling factor on the Y axis
    //!
    //! \return Reference to *this
    //!
    //! \see translate, rotate
    Transform& scale(float scaleX, float scaleY);

    //! \brief Combine the current transform with a scaling
    //!
    //! The center of scaling is provided for convenience as a second
    //! argument, so that you can build scaling around arbitrary points
    //! more easily (and efficiently) than the usual
    //! translate(-center).scale(factors).translate(center).
    //!
    //! This function returns a reference to *this, so that calls
    //! can be chained.
    //! \code
    //! sf::Transform transform;
    //! transform.scale(2, 1, 8, 3).rotate(45);
    //! \endcode
    //!
    //! \param aScaleX Scaling factor on X axis
    //! \param aScaleY Scaling factor on Y axis
    //! \param aCenterX X coordinate of the center of scaling
    //! \param aCenterY Y coordinate of the center of scaling
    //!
    //! \return Reference to *this
    //!
    //! \see translate, rotate
    Transform& scale(float aScaleX, float aScaleY, float aCenterX, float aCenterY);

    //! \brief Combine the current transform with a scaling
    //!
    //! This function returns a reference to *this, so that calls
    //! can be chained.
    //! \code
    //! sf::Transform transform;
    //! transform.scale(sf::Vector2f(2, 1)).rotate(45);
    //! \endcode
    //!
    //! \param aFactors Scaling factors
    //!
    //! \return Reference to *this
    //!
    //! \see translate, rotate
    Transform& scale(const math::Vector2f& aFactors);

    //! \brief Combine the current transform with a scaling
    //!
    //! The center of scaling is provided for convenience as a second
    //! argument, so that you can build scaling around arbitrary points
    //! more easily (and efficiently) than the usual
    //! translate(-center).scale(factors).translate(center).
    //!
    //! This function returns a reference to *this, so that calls
    //! can be chained.
    //! \code
    //! sf::Transform transform;
    //! transform.scale(sf::Vector2f(2, 1), sf::Vector2f(8, 3)).rotate(45);
    //! \endcode
    //!
    //! \param aFactors Scaling factors
    //! \param aCenter Center of scaling
    //!
    //! \return Reference to *this
    //!
    //! \see translate, rotate
    Transform& scale(const math::Vector2f& aFactors, const math::Vector2f& aCenter);
    
    static const Transform IDENTITY; //!< The identity transform (does nothing)

private:
    friend class detail::GraphicsImplAccessor;

    void* _getSFMLImpl();
    const void* _getSFMLImpl() const;

    static constexpr ::std::size_t STORAGE_SIZE  = 64;
    static constexpr ::std::size_t STORAGE_ALIGN = 4;
    ::std::aligned_storage<STORAGE_SIZE, STORAGE_ALIGN>::type _storage;
};

//! \relates sf::Transform
//! \brief Overload of binary operator * to combine two transforms
//!
//! This call is equivalent to calling Transform(left).combine(right).
//!
//! \param left Left operand (the first transform)
//! \param right Right operand (the second transform)
//!
//! \return New combined transform
Transform operator*(const Transform& left, const Transform& right);

//! \relates sf::Transform
//! \brief Overload of binary operator *= to combine two transforms
//!
//! This call is equivalent to calling left.combine(right).
//!
//! \param left Left operand (the first transform)
//! \param right Right operand (the second transform)
//!
//! \return The combined transform
Transform& operator*=(Transform& left, const Transform& right);

//! \relates sf::Transform
//! \brief Overload of binary operator * to transform a point
//!
//! This call is equivalent to calling left.transformPoint(right).
//!
//! \param left Left operand (the transform)
//! \param right Right operand (the point to transform)
//!
//! \return New transformed point
math::Vector2f operator*(const Transform& left, const math::Vector2f& right);

//! \relates sf::Transform
//! \brief Overload of binary operator == to compare two transforms
//!
//! Performs an element-wise comparison of the elements of the
//! left transform with the elements of the right transform.
//!
//! \param left Left operand (the first transform)
//! \param right Right operand (the second transform)
//!
//! \return true if the transforms are equal, false otherwise
bool operator==(const Transform& left, const Transform& right);

//! \relates sf::Transform
//! \brief Overload of binary operator != to compare two transforms
//!
//! This call is equivalent to !(left == right).
//!
//! \param left Left operand (the first transform)
//! \param right Right operand (the second transform)
//!
//! \return true if the transforms are not equal, false otherwise
bool operator!=(const Transform& left, const Transform& right);

} // namespace gr
HOBGOBLIN_NAMESPACE_END

#include <Hobgoblin/Private/Pmacro_undef.hpp>
#include <Hobgoblin/Private/Short_namespace.hpp>

#endif // !UHOBGOBLIN_GRAPHICS_TRANSFORM_HPP

////////////////////////////////////////////////////////////
/// \class sf::Transform
/// \ingroup graphics
///
/// A sf::Transform specifies how to translate, rotate, scale,
/// shear, project, whatever things. In mathematical terms, it defines
/// how to transform a coordinate system into another.
///
/// For example, if you apply a rotation transform to a sprite, the
/// result will be a rotated sprite. And anything that is transformed
/// by this rotation transform will be rotated the same way, according
/// to its initial position.
///
/// Transforms are typically used for drawing. But they can also be
/// used for any computation that requires to transform points between
/// the local and global coordinate systems of an entity (like collision
/// detection).
///
/// Example:
/// \code
/// // define a translation transform
/// sf::Transform translation;
/// translation.translate(20, 50);
///
/// // define a rotation transform
/// sf::Transform rotation;
/// rotation.rotate(45);
///
/// // combine them
/// sf::Transform transform = translation * rotation;
///
/// // use the result to transform stuff...
/// sf::Vector2f point = transform.transformPoint(10, 20);
/// sf::FloatRect rect = transform.transformRect(sf::FloatRect(0, 0, 10, 100));
/// \endcode
///
/// \see sf::Transformable, sf::RenderStates
///
////////////////////////////////////////////////////////////
