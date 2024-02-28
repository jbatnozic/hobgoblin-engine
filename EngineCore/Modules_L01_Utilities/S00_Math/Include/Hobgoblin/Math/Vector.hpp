#ifndef UHOBGOBLIN_MATH_VECTOR_HPP
#define UHOBGOBLIN_MATH_VECTOR_HPP

#include <Hobgoblin/Common.hpp>
#include <SFML/System/Vector2.hpp>
#include <SFML/System/Vector3.hpp>

#include <Hobgoblin/Private/Pmacro_define.hpp>

HOBGOBLIN_NAMESPACE_BEGIN
namespace math {

// TODO: Use own vector class

///////////////////////////////////////////////////////////////////////////
// VECTOR 2                                                              //
///////////////////////////////////////////////////////////////////////////

using sf::Vector2;
using sf::Vector2i;
using sf::Vector2u;
using sf::Vector2f;
using Vector2pz = Vector2<PZInteger>;
using Vector2d  = Vector2<double>;

#ifndef NOT_USING_SFML_VECTORS
template <class T>
Vector2<T> operator+(const Vector2<T>& aLhs, const Vector2<T>& aRhs) {
    return {aLhs.x + aRhs.x, aLhs.y + aRhs.y};
}
#endif

///////////////////////////////////////////////////////////////////////////
// VECTOR 3                                                              //
///////////////////////////////////////////////////////////////////////////

using sf::Vector3;
using sf::Vector3i;
using Vector3u  = Vector3<unsigned>;
using sf::Vector3f;
using Vector3pz = Vector3<PZInteger>;
using Vector3d  = Vector3<double>;

///////////////////////////////////////////////////////////////////////////
// VECTOR 4                                                              //
///////////////////////////////////////////////////////////////////////////

//! A 4-dimensional vector.
template <typename taArithmetic>
struct Vector4 {
    //! \brief Default constructor, creates a zero vector.
    Vector4()
        : x{static_cast<taArithmetic>(0)}
        , y{static_cast<taArithmetic>(0)}
        , z{static_cast<taArithmetic>(0)}
        , w{static_cast<taArithmetic>(0)}
    {
    }

    //! \brief Construct from 4 vector components.
    //!
    //! \param aX Component of the 4D vector.
    //! \param aY Component of the 4D vector.
    //! \param aZ Component of the 4D vector.
    //! \param aW Component of the 4D vector.
    Vector4(taArithmetic aX, taArithmetic aY, taArithmetic aZ, taArithmetic aW)
        : x{aX}
        , y{aY}
        , z{aZ}
        , w{aW}
    {
    }

    //! \brief Conversion constructor.
    //!
    //! \param other 4D vector of different type.
    template <typename taArithmetic2>
    explicit Vector4(const Vector4<taArithmetic2>& aOther)
        : x(static_cast<taArithmetic>(aOther.x))
        , y(static_cast<taArithmetic>(aOther.y))
        , z(static_cast<taArithmetic>(aOther.z))
        , w(static_cast<taArithmetic>(aOther.w))
    {
    }

    taArithmetic x; //!< 1st component (X) of the 4D vector.
    taArithmetic y; //!< 2nd component (Y) of the 4D vector.
    taArithmetic z; //!< 3rd component (Z) of the 4D vector.
    taArithmetic w; //!< 4th component (W) of the 4D vector.
};

using Vector4i  = Vector4<int>;
using Vector4u  = Vector4<unsigned>;
using Vector4f  = Vector4<float>;
using Vector4pz = Vector4<PZInteger>;
using Vector4d  = Vector4<double>;

} // namespace math
HOBGOBLIN_NAMESPACE_END

#include <Hobgoblin/Private/Pmacro_undef.hpp>
#include <Hobgoblin/Private/Short_namespace.hpp>

#endif // !UHOBGOBLIN_MATH_VECTOR_HPP