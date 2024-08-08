// Copyright 2024 Jovan Batnozic. Released under MS-PL licence in Serbia.
// See https://github.com/jbatnozic/Hobgoblin?tab=readme-ov-file#licence

#ifndef UHOBGOBLIN_ALVIN_QUERY_INFO_HPP
#define UHOBGOBLIN_ALVIN_QUERY_INFO_HPP

#include <Hobgoblin/Alvin/Collision_delegate.hpp>
#include <Hobgoblin/ChipmunkPhysics.hpp>

#include <Hobgoblin/Private/Pmacro_define.hpp>

HOBGOBLIN_NAMESPACE_BEGIN
namespace alvin {

// MUST NOT REMOVE SHAPE DURING QUERY CALLBACK

struct PointQueryInfo {
    //! The nearest shape, NULL if no shape was within range.
    cpShape* shape = nullptr;

    //! Collision delegate of the nearest shape (or NULL if shape is NULL).
    CollisionDelegate* delegate = nullptr;

    //! The closest point on the shape's surface (in world space coordinates).
    //! \note undefined value if shape is NULL.
    cpVect closestPoint = cpvzero;

    //! The distance from the query point and the closest point on the found shape.
    //! This value will be negative if the point is inside the shape.
    //! \note undefined value if shape is NULL.
    cpFloat distance = 0.0;
};

struct RaycastQueryInfo {
    //! TODO
    cpShape* shape = nullptr;

    //! TODO
    CollisionDelegate* delegate = nullptr;

    //! Point (in world coordinates) where the ray hit the shape.
    //!
    //! \note if the `aRadius` parameter of the raycast query used to obtain this object was
    //!       greater than zero, then `point` will mark a point of the shape that intersects
    //!       with this 'fat' ray that is closest to the origin of the ray.
    //!
    //! \note undefined value if shape is NULL.
    cpVect closestPoint = cpvzero;

    //! Normal vector of the shape's surface at the point where the ray hit the shape.
    //!
    //! \note undefined value if shape is NULL.
    cpVect surfaceNormal = cpvzero;

    //! Represents the distance between the origin of the ray and the point where it hit the
    //! shape, normalized to the range [0. 1] (0 = closest to the ray origin, 1 = furthest away).
    //!
    //! \warning it's not recommended to use `normalizedDistance` if the `aRadius` parameter of
    //!          the raycast query used to obtain this object was greater than zero, because the
    //!          value won't be totally accurate, and the greater the radius the greater the
    //!          inaccurracy will be.
    //!
    //! \note undefined value if shape is NULL.
    cpFloat normalizedDistance = 0.0;
};

struct BboxQueryInfo {
    //! TODO
    cpShape* shape = nullptr;

    //! TODO
    CollisionDelegate* delegate = nullptr;
};

} // namespace alvin
HOBGOBLIN_NAMESPACE_END

#include <Hobgoblin/Private/Pmacro_undef.hpp>
#include <Hobgoblin/Private/Short_namespace.hpp>

#endif // !UHOBGOBLIN_ALVIN_QUERY_INFO_HPP
