// Copyright 2024 Jovan Batnozic. Released under MS-PL licence in Serbia.
// See https://github.com/jbatnozic/Hobgoblin?tab=readme-ov-file#licence

#include <Hobgoblin/Alvin/Main_collision_dispatcher.hpp>

#include <Hobgoblin/Private/Pmacro_define.hpp>

HOBGOBLIN_NAMESPACE_BEGIN
namespace alvin {

const detail::GenericEntityCollisionFunc* MainCollisionDispatcher::_findCollisionFunc(
    const CollisionDelegate& aDelegate,
    EntityTypeId             aEntityTypeId,
    detail::Usage            aUsage) {
    if (aDelegate._isTrivial()) { // TODO: if (aDelegate.funcCount(aUsage) == 0) return nullptr;
        return nullptr;
    }

    while (aEntityTypeId != ENTITY_TYPE_ID_VOID) {
        if (const auto* ptr = aDelegate._findCollisionFunc(aEntityTypeId, aUsage)) {
            return ptr;
        }

        const auto iter = _registry.find(aEntityTypeId);
        if (iter != _registry.end()) {
            aEntityTypeId = iter->second;
        } else {
            break;
        }
    }

    return nullptr;
}

void MainCollisionDispatcher::_initCols(cpSpace* aSpace) {
    using namespace detail;

    auto* handler     = cpSpaceAddDefaultCollisionHandler(aSpace);
    handler->userData = this;

    handler->beginFunc = [](cpArbiter* aArbiter, cpSpace* aSpace, cpDataPointer aUserData) -> cpBool {
        auto* dispatcher = aUserData.get<MainCollisionDispatcher>();
        HG_HARD_ASSERT(dispatcher != nullptr);

        // Get pointers to shapes
        CP_ARBITER_GET_SHAPES(aArbiter, shape1, shape2);
        // Get pointers to collision delegates
        const auto* del1 = cpShapeGetUserData(shape1).get<CollisionDelegate>(); // TODO: null safety
        const auto* del2 = cpShapeGetUserData(shape2).get<CollisionDelegate>();
        // Get pointers to collision functions
        const auto* cf1 =
            dispatcher->_findCollisionFunc(*del1, del2->getEntityTypeId(), Usage::BEGIN);
        const auto* cf2 =
            dispatcher->_findCollisionFunc(*del2, del1->getEntityTypeId(), Usage::BEGIN);
        // Get results of collision functions (or default values)
        const Decision result1 =
            cf1 ? ((*cf1)(del2->getEntity(), aArbiter, aSpace, 1)) : del1->getDefaultDecision();
        const Decision result2 =
            cf2 ? ((*cf2)(del1->getEntity(), aArbiter, aSpace, 2)) : del2->getDefaultDecision();

        return (detail::ToBool(result1) && detail::ToBool(result2));
    };

    handler->preSolveFunc = [](cpArbiter* aArbiter, cpSpace* aSpace, cpDataPointer aUserData) -> cpBool {
        auto* dispatcher = aUserData.get<MainCollisionDispatcher>();
        HG_HARD_ASSERT(dispatcher != nullptr);

        // Get pointers to shapes
        CP_ARBITER_GET_SHAPES(aArbiter, shape1, shape2);
        // Get pointers to collision delegates
        const auto* del1 = cpShapeGetUserData(shape1).get<CollisionDelegate>();
        const auto* del2 = cpShapeGetUserData(shape2).get<CollisionDelegate>();
        // Get pointers to collision functions
        const auto* cf1 =
            dispatcher->_findCollisionFunc(*del1, del2->getEntityTypeId(), Usage::PRESOLVE);
        const auto* cf2 =
            dispatcher->_findCollisionFunc(*del2, del1->getEntityTypeId(), Usage::PRESOLVE);
        // Get results of collision functions (or default values)
        const Decision result1 =
            cf1 ? ((*cf1)(del2->getEntity(), aArbiter, aSpace, 1)) : del1->getDefaultDecision();
        const Decision result2 =
            cf2 ? ((*cf2)(del1->getEntity(), aArbiter, aSpace, 2)) : del2->getDefaultDecision();

        return (detail::ToBool(result1) && detail::ToBool(result2));
    };

    handler->postSolveFunc = [](cpArbiter* aArbiter, cpSpace* aSpace, cpDataPointer aUserData) {
        auto* dispatcher = aUserData.get<MainCollisionDispatcher>();
        HG_HARD_ASSERT(dispatcher != nullptr);

        // Get pointers to shapes
        CP_ARBITER_GET_SHAPES(aArbiter, shape1, shape2);
        // Get pointers to collision delegates
        const auto* del1 = cpShapeGetUserData(shape1).get<CollisionDelegate>();
        const auto* del2 = cpShapeGetUserData(shape2).get<CollisionDelegate>();
        // Get pointers to collision functions
        const auto* cf1 =
            dispatcher->_findCollisionFunc(*del1, del2->getEntityTypeId(), Usage::POSTSOLVE);
        const auto* cf2 =
            dispatcher->_findCollisionFunc(*del2, del1->getEntityTypeId(), Usage::POSTSOLVE);
        // Get results of collision functions (or default values)
        if (cf1) {
            (*cf1)(del2->getEntity(), aArbiter, aSpace, 1);
        }
        if (cf2) {
            (*cf2)(del1->getEntity(), aArbiter, aSpace, 2);
        }
    };

    handler->separateFunc = [](cpArbiter* aArbiter, cpSpace* aSpace, cpDataPointer aUserData) {
        auto* dispatcher = aUserData.get<MainCollisionDispatcher>();
        HG_HARD_ASSERT(dispatcher != nullptr);

        // Get pointers to shapes
        CP_ARBITER_GET_SHAPES(aArbiter, shape1, shape2);
        // Get pointers to collision delegates
        const auto* del1 = cpShapeGetUserData(shape1).get<CollisionDelegate>();
        const auto* del2 = cpShapeGetUserData(shape2).get<CollisionDelegate>();
        // Get pointers to collision functions
        const auto* cf1 =
            dispatcher->_findCollisionFunc(*del1, del2->getEntityTypeId(), Usage::SEPARATE);
        const auto* cf2 =
            dispatcher->_findCollisionFunc(*del2, del1->getEntityTypeId(), Usage::SEPARATE);
        // Get results of collision functions (or default values)
        if (cf1) {
            (*cf1)(del2->getEntity(), aArbiter, aSpace, 1);
        }
        if (cf2) {
            (*cf2)(del1->getEntity(), aArbiter, aSpace, 2);
        }
    };
}

} // namespace alvin
HOBGOBLIN_NAMESPACE_END

#include <Hobgoblin/Private/Pmacro_undef.hpp>