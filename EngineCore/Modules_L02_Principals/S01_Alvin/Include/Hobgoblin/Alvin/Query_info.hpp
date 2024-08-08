// Copyright 2024 Jovan Batnozic. Released under MS-PL licence in Serbia.
// See https://github.com/jbatnozic/Hobgoblin?tab=readme-ov-file#licence

#ifndef UHOBGOBLIN_ALVIN_QUERY_INFO_HPP
#define UHOBGOBLIN_ALVIN_QUERY_INFO_HPP

#include <Hobgoblin/ChipmunkPhysics.hpp>
#include <Hobgoblin/Alvin/Collision_delegate.hpp>

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
    cpVect closestPoint = cpv(0.0, 0.0);

    //! The distance from the query point and the closest point on the found shape.
    //! This value will be negative if the point is inside the shape.
    //! \note undefined value if shape is NULL.
    cpFloat distance = 0.0;
};

} // namespace alvin
HOBGOBLIN_NAMESPACE_END

#include <Hobgoblin/Private/Pmacro_undef.hpp>
#include <Hobgoblin/Private/Short_namespace.hpp>

#endif // !UHOBGOBLIN_ALVIN_QUERY_INFO_HPP
