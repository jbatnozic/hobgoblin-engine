// Copyright 2024 Jovan Batnozic. Released under MS-PL licence in Serbia.
// See https://github.com/jbatnozic/Hobgoblin?tab=readme-ov-file#licence

#ifndef UHOBGOBLIN_ALVIN_COLLISION_DATA_HPP
#define UHOBGOBLIN_ALVIN_COLLISION_DATA_HPP

#include <Hobgoblin/ChipmunkPhysics.hpp>
#include <Hobgoblin/Common.hpp>

#include <Hobgoblin/Private/Pmacro_define.hpp>

HOBGOBLIN_NAMESPACE_BEGIN
namespace alvin {

struct CollisionData {
    //! Arbiter governing the current collision.
    NeverNull<cpArbiter*> arbiter;

    //! Space in which the collision happened.
    NeverNull<cpSpace*> space;

    //! This will always be either 1 or 2:
    //! - 1 if this is the 1st shape processing the collision
    //! - 2 if this is the 2nd shape processing the collision
    //!
    //! (remember that, when a collision happens, both shapes
    //! are notified about it - at least when the collision
    //! is not rejected.)
    PZInteger order;
};

} // namespace alvin
HOBGOBLIN_NAMESPACE_END

#include <Hobgoblin/Private/Pmacro_undef.hpp>
#include <Hobgoblin/Private/Short_namespace.hpp>

#endif // !UHOBGOBLIN_ALVIN_COLLISION_DATA_HPP
