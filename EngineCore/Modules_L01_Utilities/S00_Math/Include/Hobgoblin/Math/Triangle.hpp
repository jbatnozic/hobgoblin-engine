// Copyright 2024 Jovan Batnozic. Released under MS-PL licence in Serbia.
// See https://github.com/jbatnozic/Hobgoblin?tab=readme-ov-file#licence

#ifndef UHOBGOBLIN_MATH_TRIANGLE_HPP
#define UHOBGOBLIN_MATH_TRIANGLE_HPP

#include <Hobgoblin/Math/Vector.hpp>

#include <Hobgoblin/Private/Pmacro_define.hpp>

HOBGOBLIN_NAMESPACE_BEGIN
namespace math {

template <class T>
struct Triangle {
    Vector2<T> a;
    Vector2<T> b;
    Vector2<T> c;
};

using TriangleF = Triangle<float>;
using TriangleD = Triangle<double>;

template<class T>
bool IsPointInsideTriangle(const Vector2<T>& aPoint, const Triangle<T>& aTriangle) {
    class CrossProduct {
    public:
        // Note to self: I think this function calculates the magnitude of the resulting
        // vector of of the cross product of vectors (p1->p2) and (p1->p3).
        static T calculate(const Vector2<T>& p1, const Vector2<T>& p2, const Vector2<T>& p3) {
            return (p2.x - p1.x) * (p3.y - p1.y) - (p2.y - p1.y) * (p3.x - p1.x);
        }
    };

    // Compute the cross products for the point with respect to the triangle's edges
    const float cross1 = CrossProduct::calculate(aTriangle.a, aTriangle.b, aPoint);
    const float cross2 = CrossProduct::calculate(aTriangle.b, aTriangle.c, aPoint);
    const float cross3 = CrossProduct::calculate(aTriangle.c, aTriangle.a, aPoint);

    // Check if all the cross products have the same sign (either all positive or all negative)
    const bool hasSameSign =
        (cross1 > 0 && cross2 > 0 && cross3 > 0) || (cross1 < 0 && cross2 < 0 && cross3 < 0);

    return hasSameSign;
}

} // namespace math
HOBGOBLIN_NAMESPACE_END

#include <Hobgoblin/Private/Pmacro_undef.hpp>
#include <Hobgoblin/Private/Short_namespace.hpp>

#endif // !UHOBGOBLIN_MATH_TRIANGLE_HPP
