// Copyright 2024 Jovan Batnozic. Released under MS-PL licence in Serbia.
// See https://github.com/jbatnozic/Hobgoblin?tab=readme-ov-file#licence

#ifndef UHOBGOBLIN_ALVIN_COLLISION_DELEGATE_BUILDER_HPP
#define UHOBGOBLIN_ALVIN_COLLISION_DELEGATE_BUILDER_HPP

#include <Hobgoblin/ChipmunkPhysics.hpp>
#include <Hobgoblin/Common.hpp>

#include <Hobgoblin/Alvin/Collision_delegate.hpp>
#include <Hobgoblin/Alvin/Entity_base.hpp>
#include <Hobgoblin/Alvin/Private/Helpers.hpp>

#include <algorithm>
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
HG_DECLARE_TAG_TYPE(COLLISION_SEPARATION);

class MainCollisionDispatcher;

class CollisionDelegateBuilder {
public:
    //! TODO(description)
    template <class taOther>
    CollisionDelegateBuilder& addInteraction(
        COLLISION_CONTACT_Tag,
        std::function<Decision(taOther&, NeverNull<cpArbiter*>, NeverNull<cpSpace*>, PZInteger)> aFunc);

    //! TODO(description)
    template <class taOther>
    CollisionDelegateBuilder& addInteraction(
        COLLISION_PRE_SOLVE_Tag,
        std::function<Decision(taOther&, NeverNull<cpArbiter*>, NeverNull<cpSpace*>, PZInteger)> aFunc);

    //! TODO(description)
    template <class taOther>
    CollisionDelegateBuilder& addInteraction(
        COLLISION_POST_SOLVE_Tag,
        std::function<void(taOther&, NeverNull<cpArbiter*>, NeverNull<cpSpace*>, PZInteger)> aFunc);

    //! TODO(description)
    template <class taOther>
    CollisionDelegateBuilder& addInteraction(
        COLLISION_SEPARATION_Tag,
        std::function<void(taOther&, NeverNull<cpArbiter*>, NeverNull<cpSpace*>, PZInteger)> aFunc);

    CollisionDelegateBuilder& setDefaultDecision(Decision aDecision) {
        _defaultDecision = aDecision;
        return SELF;
    }

    //! TODO(description)
    CollisionDelegate finalize() {
        std::sort(_collisionFunctions.begin(), _collisionFunctions.end());
        return CollisionDelegate{std::move(_collisionFunctions), _defaultDecision};
    }

private:
    std::vector<detail::SpecificCollisionFunc> _collisionFunctions;
    Decision                                   _defaultDecision = Decision::ACCEPT_COLLISION;
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
    std::function<Decision(taOther&, HG_NEVER_NULL(cpArbiter*), HG_NEVER_NULL(cpSpace*), PZInteger)>
        aFunc) {
    detail::EntityTypeIdAndUsage specifier{taOther::ENTITY_TYPE_ID, detail::Usage::BEGIN};
    _collisionFunctions.emplace_back(
        [func = std::move(aFunc)](EntityBase& aEntity,
                                  HG_NEVER_NULL(cpArbiter*) aArbiter,
                                  HG_NEVER_NULL(cpSpace*) aSpace,
                                  PZInteger aOrder) -> Decision {
            return func(down_cast<taOther&>(aEntity), aArbiter, aSpace, aOrder);
        },
        specifier);
    return SELF;
}

template <class taOther>
CollisionDelegateBuilder& CollisionDelegateBuilder::addInteraction(
    COLLISION_PRE_SOLVE_Tag,
    std::function<Decision(taOther&, HG_NEVER_NULL(cpArbiter*), HG_NEVER_NULL(cpSpace*), PZInteger)>
        aFunc) {
    detail::EntityTypeIdAndUsage specifier{taOther::ENTITY_TYPE_ID, detail::Usage::PRESOLVE};
    _collisionFunctions.emplace_back(
        [func = std::move(aFunc)](EntityBase& aEntity,
                                  HG_NEVER_NULL(cpArbiter*) aArbiter,
                                  HG_NEVER_NULL(cpSpace*) aSpace,
                                  PZInteger aOrder) -> Decision {
            return func(down_cast<taOther&>(aEntity), aArbiter, aSpace, aOrder);
        },
        specifier);
    return SELF;
}

template <class taOther>
CollisionDelegateBuilder& CollisionDelegateBuilder::addInteraction(
    COLLISION_POST_SOLVE_Tag,
    std::function<void(taOther&, HG_NEVER_NULL(cpArbiter*), HG_NEVER_NULL(cpSpace*), PZInteger)> aFunc) {
    detail::EntityTypeIdAndUsage specifier{taOther::ENTITY_TYPE_ID, detail::Usage::POSTSOLVE};
    _collisionFunctions.emplace_back(
        [func = std::move(aFunc)](EntityBase& aEntity,
                                  HG_NEVER_NULL(cpArbiter*) aArbiter,
                                  HG_NEVER_NULL(cpSpace*) aSpace,
                                  PZInteger aOrder) -> Decision {
            func(down_cast<taOther&>(aEntity), aArbiter, aSpace, aOrder);
            return Decision::ACCEPT_COLLISION; // Return value doesn't matter here
        },
        specifier);
    return SELF;
}

template <class taOther>
CollisionDelegateBuilder& CollisionDelegateBuilder::addInteraction(
    COLLISION_SEPARATION_Tag,
    std::function<void(taOther&, HG_NEVER_NULL(cpArbiter*), HG_NEVER_NULL(cpSpace*), PZInteger)> aFunc) {
    detail::EntityTypeIdAndUsage specifier{taOther::ENTITY_TYPE_ID, detail::Usage::SEPARATE};
    _collisionFunctions.emplace_back(
        [func = std::move(aFunc)](EntityBase& aEntity,
                                  HG_NEVER_NULL(cpArbiter*) aArbiter,
                                  HG_NEVER_NULL(cpSpace*) aSpace,
                                  PZInteger aOrder) -> Decision {
            func(down_cast<taOther&>(aEntity), aArbiter, aSpace, aOrder);
            return Decision::ACCEPT_COLLISION; // Return value doesn't matter here
        },
        specifier);
    return SELF;
}

#undef down_cast

} // namespace alvin
HOBGOBLIN_NAMESPACE_END

#include <Hobgoblin/Private/Pmacro_undef.hpp>
#include <Hobgoblin/Private/Short_namespace.hpp>

#endif // !UHOBGOBLIN_ALVIN_COLLISION_DELEGATE_BUILDER_HPP
