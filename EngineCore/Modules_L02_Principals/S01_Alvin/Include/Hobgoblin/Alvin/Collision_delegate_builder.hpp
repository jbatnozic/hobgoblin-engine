// Copyright 2024 Jovan Batnozic. Released under MS-PL licence in Serbia.
// See https://github.com/jbatnozic/Hobgoblin?tab=readme-ov-file#licence

#ifndef UHOBGOBLIN_ALVIN_COLLISION_DELEGATE_BUILDER_HPP
#define UHOBGOBLIN_ALVIN_COLLISION_DELEGATE_BUILDER_HPP

#include <Hobgoblin/ChipmunkPhysics.hpp>
#include <Hobgoblin/Common.hpp>

#include <Hobgoblin/Alvin/Collision_data.hpp>
#include <Hobgoblin/Alvin/Collision_delegate.hpp>
#include <Hobgoblin/Alvin/Entity_base.hpp>
#include <Hobgoblin/Alvin/Private/Helpers.hpp>

#include <algorithm>
#include <cstdint>
#include <functional>
#include <utility>
#include <vector>

#include <Hobgoblin/Private/Pmacro_define.hpp>

HOBGOBLIN_NAMESPACE_BEGIN
namespace alvin {

// Callback types
HG_DECLARE_TAG_TYPE(COLLISION_CONTACT);
HG_DECLARE_TAG_TYPE(COLLISION_PRE_SOLVE);
HG_DECLARE_TAG_TYPE(COLLISION_POST_SOLVE);
HG_DECLARE_TAG_TYPE(COLLISION_SEPARATE);

class MainCollisionDispatcher;

class CollisionDelegateBuilder {
public:
    //! \brief Defines a CONTACT interaction with an entity type.
    //!
    //! \tparam taOther Type of the entity with which the interaction is being defined.
    //!
    //! \param aFunc The function which will be called once when the shapes first come
    //!              into contact.
    //!              The parameters which will be passed into this function are:
    //!              - a reference to the entity to which the other shape is bound;
    //!              - a reference to an instance of CollisionData which can provide
    //!                more detailed information about the collision;
    //!              The function must return a decision to accept the collision or reject
    //!              it. If the collision is rejected, no other callback will be called
    //!              (including the SEPARATE one) until the shapes separate and come into
    //!              contact again.
    template <class taOther>
    CollisionDelegateBuilder& addInteraction(
        COLLISION_CONTACT_Tag,
        std::function<Decision(taOther&, const CollisionData&)> aFunc);

    //! \brief Defines a PRE_SOLVE interaction with an entity type.
    //!
    //! \tparam taOther Type of the entity with which the interaction is being defined.
    //!
    //! \param aFunc The function which will be called once every step while the shapes
    //!              are touching.
    //!              The parameters which will be passed into this function are:
    //!              - a reference to the entity to which the other shape is bound;
    //!              - a reference to an instance of CollisionData which can provide
    //!                more detailed information about the collision, including a pointer
    //!                to the arbiter governing the collision - the PRE_SOLVE function can
    //!                use this arbiter to override the various collision values (friction,
    //!                elasticity and velocity);
    //!              The function must return a decision to accept the collision or reject
    //!              it. If the collision is rejected a POST_SOLVE callback won't be called
    //!              during the current step.
    template <class taOther>
    CollisionDelegateBuilder& addInteraction(
        COLLISION_PRE_SOLVE_Tag,
        std::function<Decision(taOther&, const CollisionData&)> aFunc);

    //! \brief Defines a POST_SOLVE interaction with an entity type.
    //!
    //! \tparam taOther Type of the entity with which the interaction is being defined.
    //!
    //! \param aFunc The function which will be called once every step after the collision
    //!              has been solved.
    //!              The parameters which will be passed into this function are:
    //!              - a reference to the entity to which the other shape is bound;
    //!              - a reference to an instance of CollisionData which can provide
    //!                more detailed information about the collision, including a pointer
    //!                to the arbiter governing the collision - the POST_SOLVE function can
    //!                use this arbiter to retrieve information about the collision, such as
    //!                the impulse or kinetic energy;
    template <class taOther>
    CollisionDelegateBuilder& addInteraction(COLLISION_POST_SOLVE_Tag,
                                             std::function<void(taOther&, const CollisionData&)> aFunc);

    //! \brief Defines a SEPARATE interaction with an entity type.
    //!
    //! \tparam taOther Type of the entity with which the interaction is being defined.
    //!
    //! \param aFunc The function which will be called once when the shapes are no longer
    //!              touching.
    //!              The parameters which will be passed into this function are:
    //!              - a reference to the entity to which the other shape is bound;
    //!              - a reference to an instance of CollisionData which can provide
    //!                more detailed information about the collision;
    template <class taOther>
    CollisionDelegateBuilder& addInteraction(COLLISION_SEPARATE_Tag,
                                             std::function<void(taOther&, const CollisionData&)> aFunc);

    //! Sets the default decision for CONTACT and PRE_SOLVE interactions for which
    //! there is no explicitly set callback.
    //!
    //! \note if this method is never called, the delegate will be configured to
    //!       accept all collisions.
    CollisionDelegateBuilder& setDefaultDecision(Decision aDecision) {
        _defaultDecision = aDecision;
        return SELF;
    }

    //! TODO(description)
    CollisionDelegate finalize() {
        std::sort(_collisionFunctions.begin(), _collisionFunctions.end());
        return CollisionDelegate{std::move(_collisionFunctions), _defaultDecision, _funcBitmask};
    }

private:
    std::vector<detail::SpecificCollisionFunc> _collisionFunctions;
    Decision                                   _defaultDecision = Decision::ACCEPT_COLLISION;
    std::uint8_t                               _funcBitmask     = 0;
};

///////////////////////////////////////////////////////////////////////////
// IMPLEMENTATIONS                                                       //
///////////////////////////////////////////////////////////////////////////

#ifdef UHOBGOBLIN_DEBUG
#define down_cast dynamic_cast
#else
#define down_cast static_cast
#endif

template <class taOther>
CollisionDelegateBuilder& CollisionDelegateBuilder::addInteraction(
    COLLISION_CONTACT_Tag,
    std::function<Decision(taOther&, const CollisionData&)> aFunc) {
    detail::EntityTypeIdAndUsage specifier{taOther::ENTITY_TYPE_ID, detail::USAGE_COL_BEGIN};
    _collisionFunctions.emplace_back(
        [func = std::move(aFunc)](EntityBase& aEntity, const CollisionData& aCollisonData) -> Decision {
            return func(down_cast<taOther&>(aEntity), aCollisonData);
        },
        specifier);
    _funcBitmask |= (0x01 << detail::USAGE_COL_BEGIN);
    return SELF;
}

template <class taOther>
CollisionDelegateBuilder& CollisionDelegateBuilder::addInteraction(
    COLLISION_PRE_SOLVE_Tag,
    std::function<Decision(taOther&, const CollisionData&)> aFunc) {
    detail::EntityTypeIdAndUsage specifier{taOther::ENTITY_TYPE_ID, detail::USAGE_COL_PRESOLVE};
    _collisionFunctions.emplace_back(
        [func = std::move(aFunc)](EntityBase& aEntity, const CollisionData& aCollisonData) -> Decision {
            return func(down_cast<taOther&>(aEntity), aCollisonData);
        },
        specifier);
    _funcBitmask |= (0x01 << detail::USAGE_COL_PRESOLVE);
    return SELF;
}

template <class taOther>
CollisionDelegateBuilder& CollisionDelegateBuilder::addInteraction(
    COLLISION_POST_SOLVE_Tag,
    std::function<void(taOther&, const CollisionData&)> aFunc) {
    detail::EntityTypeIdAndUsage specifier{taOther::ENTITY_TYPE_ID, detail::USAGE_COL_POSTSOLVE};
    _collisionFunctions.emplace_back(
        [func = std::move(aFunc)](EntityBase& aEntity, const CollisionData& aCollisonData) -> Decision {
            func(down_cast<taOther&>(aEntity), aCollisonData);
            return Decision::ACCEPT_COLLISION; // Return value doesn't matter here
        },
        specifier);
    _funcBitmask |= (0x01 << detail::USAGE_COL_POSTSOLVE);
    return SELF;
}

template <class taOther>
CollisionDelegateBuilder& CollisionDelegateBuilder::addInteraction(
    COLLISION_SEPARATE_Tag,
    std::function<void(taOther&, const CollisionData&)> aFunc) {
    detail::EntityTypeIdAndUsage specifier{taOther::ENTITY_TYPE_ID, detail::USAGE_COL_SEPARATE};
    _collisionFunctions.emplace_back(
        [func = std::move(aFunc)](EntityBase& aEntity, const CollisionData& aCollisonData) -> Decision {
            func(down_cast<taOther&>(aEntity), aCollisonData);
            return Decision::ACCEPT_COLLISION; // Return value doesn't matter here
        },
        specifier);
    _funcBitmask |= (0x01 << detail::USAGE_COL_SEPARATE);
    return SELF;
}

#undef down_cast

} // namespace alvin
HOBGOBLIN_NAMESPACE_END

#include <Hobgoblin/Private/Pmacro_undef.hpp>
#include <Hobgoblin/Private/Short_namespace.hpp>

#endif // !UHOBGOBLIN_ALVIN_COLLISION_DELEGATE_BUILDER_HPP
