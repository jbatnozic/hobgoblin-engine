// Copyright 2024 Jovan Batnozic. Released under MS-PL licence in Serbia.
// See https://github.com/jbatnozic/Hobgoblin?tab=readme-ov-file#licence

#ifndef UHOBGOBLIN_ALVIN_UNIBODY_HPP
#define UHOBGOBLIN_ALVIN_UNIBODY_HPP

#include <Hobgoblin/ChipmunkPhysics.hpp>
#include <Hobgoblin/Common.hpp>
#include <Hobgoblin/Math/Vector.hpp>

#include <Hobgoblin/Alvin/Body.hpp>
#include <Hobgoblin/Alvin/Collision_delegate.hpp>
#include <Hobgoblin/Alvin/Shape.hpp>
#include <Hobgoblin/Alvin/Space.hpp>

#include <optional>

#include <Hobgoblin/Private/Pmacro_define.hpp>

HOBGOBLIN_NAMESPACE_BEGIN
namespace alvin {

//! For the very common case where you want to have a physics body consisting of a singular
//! shape, paired together with a collision delegate, this struct provides a convenient way
//! to bundle all three as a single object.
struct Unibody {
    //! The body, its singular shape and their collision delegate.
    //! They have to be kept in this order because the destruction order must be
    //! shape -> body -> delegate.
    //! \warning they are public elements to enable more ergonomic use. Unfortunately, this also
    //!          means that it's possible to reassign them, but you really shouldn't do that!
    CollisionDelegate delegate; //!< Collision delegate for the unibody's body and shape.
    Body              body;     //!< Unibody's body.
    Shape             shape;    //!< Unibody's shape.

    //! Constructor.
    //!
    //! \param aCollisionDelegateFactory Functor accepting no arguments that will return an instance
    //!                                  of `alvin::CollisionDelegate` to store in the unibody.
    //! \param aBodyFactory Functor accepting no arguments that will return an instance
    //!                     of `alvin::Body` to store in the unibody.
    //! \param aShapeFactory Functor accepting no arguments that will return an instance
    //!                      of `alvin::Shape` to store in the unibody.
    //!
    //! \note the functors will be called in order of listing, meaning that in the shape functor
    //!       you can already refer to the body.
    //! \note none of the functors will be stored.
    template <class taCollisionDelegateFactory, class taBodyFactory, class taShapeFactory>
    Unibody(taCollisionDelegateFactory&& aCollisionDelegateFactory,
            taBodyFactory&&              aBodyFactory,
            taShapeFactory&&             aShapeFactory)
        : delegate{aCollisionDelegateFactory()}
        , body{aBodyFactory()}
        , shape{aShapeFactory()} {}

    ~Unibody() = default;

    //! Prevent copying.
    Unibody(const Unibody&)            = delete;
    Unibody& operator=(const Unibody&) = delete;

    //! Allow cheap moving.
    Unibody(Unibody&&)            = default;
    Unibody& operator=(Unibody&&) = default;

    //! Binds the collision delegate to the shape owned by the same unibody,
    //! and to the provided entity.
    //!
    //! \note Once a delegate is bound, it (and consequently the owning unibody)
    //!       must not be moved anymore!
    template <class taEntity>
    void bindDelegate(taEntity&                aEntity,
                      std::optional<cpGroup>   aGroup        = std::nullopt,
                      std::optional<cpBitmask> aCategory     = std::nullopt,
                      std::optional<cpBitmask> aCollidesWith = std::nullopt) {
        delegate.bind(aEntity, shape, aGroup, aCategory, aCollidesWith);
    }

    //! Adds the body and shape to the space at an undefined position.
    //! \note there is no need to remove them manually later, it will happen
    //!       automatically in the destructor.
    void addToSpace(NeverNull<cpSpace*> aSpace) {
        cpSpaceAddBody(aSpace, body);
        cpSpaceAddShape(aSpace, shape);
    }

    //! Adds the body and shape to the space at the initial position provided by `aInitialPosition`.
    //! \note there is no need to remove them manually later, it will happen
    //!       automatically in the destructor.
    void addToSpace(NeverNull<cpSpace*> aSpace, const math::Vector2<double>& aInitialPosition) {
        cpSpaceAddBody(aSpace, body);
        cpBodySetPosition(body, cpv(aInitialPosition.x, aInitialPosition.y));
        cpSpaceAddShape(aSpace, shape);
    }

    //! Automatic conversion to `cpBody*` for
    //! compatibility with regular Chipmunk functions.
    //! \note the returned pointer will never be NULL
    //!       unless the object was moved from.
    operator cpBody*() {
        return body;
    }

    //! Automatic conversion to `const cpBody*` for
    //! compatibility with regular Chipmunk functions.
    //! \note the returned pointer will never be NULL
    //!       unless the object was moved from.
    operator const cpBody*() const {
        return body;
    }

    //! Automatic conversion to `cpShape*` for
    //! compatibility with regular Chipmunk functions.
    //! \note the returned pointer will never be NULL
    //!       unless the object was moved from.
    operator cpShape*() {
        return shape;
    }

    //! Automatic conversion to `const cpShape*` for
    //! compatibility with regular Chipmunk functions.
    //! \note the returned pointer will never be NULL
    //!       unless the object was moved from.
    operator const cpShape*() const {
        return shape;
    }
};

} // namespace alvin
HOBGOBLIN_NAMESPACE_END

#include <Hobgoblin/Private/Pmacro_undef.hpp>
#include <Hobgoblin/Private/Short_namespace.hpp>

#endif // !UHOBGOBLIN_ALVIN_UNIBODY_HPP
