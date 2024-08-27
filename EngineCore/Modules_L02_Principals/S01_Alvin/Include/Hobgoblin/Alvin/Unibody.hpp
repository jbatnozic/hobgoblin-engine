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

#include <cassert>
#include <new>
#include <optional>

#include <Hobgoblin/Private/Pmacro_define.hpp>

HOBGOBLIN_NAMESPACE_BEGIN
namespace alvin {

//! For the very common case where you want to have a physics body consisting of a singular
//! shape, paired together with a collision delegate, this struct provides a convenient way
//! to bundle all three as a single object.
struct Unibody {
    //! Storage for the collision delegate of the Unibody.
    //!
    //! \warning the field is public to enable more ergonomic use (you can access it simply with
    //!          `unibody.delegate`). DO NOT reassign the field! Also, DO NOT access it if the
    //!          Unibody is not initialized (see `isInitialized()`).
    union {
        CollisionDelegate delegate; //!< Collision delegate for the unibody's body and shape.

        void* __alvinimpl_delegatePlaceholder;
    };

    //! Storage for the body of the Unibody.
    //!
    //! \warning the field is public to enable more ergonomic use (you can access it simply with
    //!          `unibody.body`). DO NOT reassign the field! Also, DO NOT access it if the
    //!          Unibody is not initialized (see `isInitialized()`). Exception: you can access it
    //!          during initialization (either constructor or `init()` method) from the shape
    //!          factory, because the body is guaranteed to already exist before the shape is created.
    union {
        Body body;

        void* __alvinimpl_bodyPlaceholder;
    };

    //! Storage for the shape of the Unibody.
    //!
    //! \warning the field is public to enable more ergonomic use (you can access it simply with
    //!          `unibody.shape`). DO NOT reassign the field! Also, DO NOT access it if the
    //!          Unibody is not initialized (see `isInitialized()`).
    union {
        Shape shape;

        void* __alvinimpl_shapePlaceholder;
    };

    //! Default constructor. Creates the Unibody in an uninitialized state.
    Unibody();

    //! Constructor. Fully initializes the Unibody.
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
            taShapeFactory&&             aShapeFactory) {
        init(std::forward<taCollisionDelegateFactory>(aCollisionDelegateFactory),
             std::forward<taBodyFactory>(aBodyFactory),
             std::forward<taShapeFactory>(aShapeFactory));
    }

    //! If the Unibody was default-constructed, use this method to fully initialize it.
    //! The parameters are the same as for the non-default constructor.
    template <class taCollisionDelegateFactory, class taBodyFactory, class taShapeFactory>
    void init(taCollisionDelegateFactory&& aCollisionDelegateFactory,
              taBodyFactory&&              aBodyFactory,
              taShapeFactory&&             aShapeFactory);

    //! Destructor.
    ~Unibody();

    //! Prevent copying.
    Unibody(const Unibody&)            = delete;
    Unibody& operator=(const Unibody&) = delete;

    //! Allow cheap moving.
    //!
    //! \note leaves `aOther` in uninitialized state.
    //!
    //! \throws TracedLogicError if `aOther` is already bound to one or more shapes.
    //! \see bindDelegate
    Unibody(Unibody&& aOther);

    //! Allow cheap moving.
    //!
    //! \note leaves `aOther` in uninitialized state.
    //!
    //! \throws TracedLogicError if `aOther` is already bound to one or more shapes.
    //! \see bindDelegate
    Unibody& operator=(Unibody&& aOther);

    //! Checks whether the Unibody is initialized (true) or not (false).
    bool isInitialized() const noexcept {
        return _isInitialized;
    }

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
        assert(_isInitialized);
        SELF.delegate.bind(aEntity, shape, aGroup, aCategory, aCollidesWith);
    }

    //! Adds the body and shape to the space at an undefined position.
    //! \note there is no need to remove them manually later, it will happen
    //!       automatically in the destructor.
    void addToSpace(NeverNull<cpSpace*> aSpace);

    //! Adds the body and shape to the space at the initial position provided by `aInitialPosition`.
    //! \note there is no need to remove them manually later, it will happen
    //!       automatically in the destructor.
    void addToSpace(NeverNull<cpSpace*> aSpace, const math::Vector2<double>& aInitialPosition);

    //! Automatic conversion to `cpBody*` for compatibility with regular Chipmunk functions.
    //!
    //! \note if the Unibody is initialized (see `isInitialized()`), this method never returns NULL.
    //!       Otherwise, the behaviour is undefined. Exception: during initialization (either
    //!       constructor or `init()` method) you can use this conversion from the shape factory,
    //!       because the body is guaranteed to already exist before the shape is created.
    operator cpBody*();

    //! Automatic conversion to `const cpBody*` for compatibility with regular Chipmunk functions.
    //!
    //! \note if the Unibody is initialized (see `isInitialized()`), this method never returns NULL.
    //!       Otherwise, the behaviour is undefined. Exception: during initialization (either
    //!       constructor or `init()` method) you can use this conversion from the shape factory,
    //!       because the body is guaranteed to already exist before the shape is created.
    operator const cpBody*() const;

    //! Automatic conversion to `cpShape*` for compatibility with regular Chipmunk functions.
    //!
    //! \note if the Unibody is initialized (see `isInitialized()`), this method never returns NULL.
    //!       Otherwise, the behaviour is undefined.
    operator cpShape*();

    //! Automatic conversion to `const cpShape*` for compatibility with regular Chipmunk functions.
    //!
    //! \note if the Unibody is initialized (see `isInitialized()`), this method never returns NULL.
    //!       Otherwise, the behaviour is undefined.
    operator const cpShape*() const;

private:
    bool _isInitialized = false;
};

///////////////////////////////////////////////////////////////////////////
// IMPLEMENTATIONS                                                       //
///////////////////////////////////////////////////////////////////////////

template <class taCollisionDelegateFactory, class taBodyFactory, class taShapeFactory>
void Unibody::init(taCollisionDelegateFactory&& aCollisionDelegateFactory,
                   taBodyFactory&&              aBodyFactory,
                   taShapeFactory&&             aShapeFactory) {
    assert(!_isInitialized);

    // Delegate
    try {
        new (&SELF.delegate) CollisionDelegate(aCollisionDelegateFactory());
    } catch (...) {
        throw;
    }

    // Body
    try {
        new (&SELF.body) Body(aBodyFactory());
    } catch (...) {
        SELF.delegate.~CollisionDelegate();
        throw;
    }

    // Shape
    try {
        new (&SELF.shape) Shape(aShapeFactory());
    } catch (...) {
        SELF.body.~Body();
        SELF.delegate.~CollisionDelegate();
        throw;
    }

    _isInitialized = true;
}

inline Unibody::operator cpBody*() {
    return SELF.body;
}

inline Unibody::operator const cpBody*() const {
    return SELF.body;
}

inline Unibody::operator cpShape*() {
    return SELF.shape;
}

inline Unibody::operator const cpShape*() const {
    return SELF.shape;
}

} // namespace alvin
HOBGOBLIN_NAMESPACE_END

#include <Hobgoblin/Private/Pmacro_undef.hpp>
#include <Hobgoblin/Private/Short_namespace.hpp>

#endif // !UHOBGOBLIN_ALVIN_UNIBODY_HPP
