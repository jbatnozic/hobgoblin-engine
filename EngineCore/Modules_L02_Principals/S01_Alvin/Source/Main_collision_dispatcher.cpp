// Copyright 2024 Jovan Batnozic. Released under MS-PL licence in Serbia.
// See https://github.com/jbatnozic/Hobgoblin?tab=readme-ov-file#licence

#include <Hobgoblin/Alvin/Main_collision_dispatcher.hpp>

#include <Hobgoblin/HGExcept.hpp>

#include <Hobgoblin/Private/Pmacro_define.hpp>

HOBGOBLIN_NAMESPACE_BEGIN
namespace alvin {

namespace {
template <class taPointer>
taPointer AssertNotNull(taPointer aPtr) {
    HG_ASSERT(aPtr != nullptr);
    return aPtr;
}

struct CollisionActors {
    CollisionActors(cpArbiter* aArbiter, const cpDataPointer& aUserData)
        : dispatcher{AssertNotNull(aUserData.get<MainCollisionDispatcher>())} {
        CP_ARBITER_GET_SHAPES(aArbiter, shape1, shape2);
        delegate1 = AssertNotNull(cpShapeGetUserData(shape1).get<CollisionDelegate>());
        delegate2 = AssertNotNull(cpShapeGetUserData(shape2).get<CollisionDelegate>());
    }

    MainCollisionDispatcher* dispatcher;
    CollisionDelegate*       delegate1;
    CollisionDelegate*       delegate2;
};
} // namespace

MainCollisionDispatcher::MainCollisionDispatcher() {
    _registerEntityType<EntityBase>();
}

const detail::GenericEntityCollisionFunc* MainCollisionDispatcher::_findCollisionFunc(
    const CollisionDelegate& aDelegate,
    EntityTypeId             aEntityTypeId,
    detail::Usage            aUsage) {
    if (aDelegate._hasNoFunctionsForUsage(aUsage)) {
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

void MainCollisionDispatcher::bind(NeverNull<cpSpace*> aSpace) {
    using namespace detail;

    auto* handler = cpSpaceAddDefaultCollisionHandler(aSpace);

    HG_HARD_ASSERT(handler->userData == nullptr);
    handler->userData = this;

    // clang-format off
    handler->beginFunc = [](cpArbiter* aArbiter, cpSpace* aSpace, cpDataPointer aUserData) -> cpBool {
        static constexpr auto USAGE = USAGE_COL_BEGIN;

        const auto ca = CollisionActors{aArbiter, aUserData};

        const auto* cf1 = ca.dispatcher->_findCollisionFunc(*ca.delegate1,
                                                            ca.delegate2->getEntityTypeId(),
                                                            USAGE);
        const Decision result1 = cf1 ? ((*cf1)(ca.delegate2->getEntity(), {aArbiter, aSpace, 1}))
                                     : ca.delegate1->getDefaultDecision();

        if (!ToBool(result1)) {
            return cpFalse;
        }

        const auto* cf2 = ca.dispatcher->_findCollisionFunc(*ca.delegate2,
                                                            ca.delegate1->getEntityTypeId(),
                                                            USAGE);
        const Decision result2 = cf2 ? ((*cf2)(ca.delegate1->getEntity(), {aArbiter, aSpace, 2}))
                                     : ca.delegate2->getDefaultDecision();

        return ToBool(result2);
    };

    handler->preSolveFunc = [](cpArbiter* aArbiter, cpSpace* aSpace, cpDataPointer aUserData) -> cpBool {
        static constexpr auto USAGE = USAGE_COL_PRESOLVE;

        const auto ca = CollisionActors{aArbiter, aUserData};

        const auto* cf1 = ca.dispatcher->_findCollisionFunc(*ca.delegate1,
                                                            ca.delegate2->getEntityTypeId(),
                                                            USAGE);
        const Decision result1 = cf1 ? ((*cf1)(ca.delegate2->getEntity(), {aArbiter, aSpace, 1}))
                                     : ca.delegate1->getDefaultDecision();

        if (!ToBool(result1)) {
            return cpFalse;
        }

        const auto* cf2 = ca.dispatcher->_findCollisionFunc(*ca.delegate2,
                                                            ca.delegate1->getEntityTypeId(),
                                                            USAGE);
        const Decision result2 = cf2 ? ((*cf2)(ca.delegate1->getEntity(), {aArbiter, aSpace, 2}))
                                     : ca.delegate2->getDefaultDecision();

        return ToBool(result2);
    };

    handler->postSolveFunc = [](cpArbiter* aArbiter, cpSpace* aSpace, cpDataPointer aUserData) {
        static constexpr auto USAGE = USAGE_COL_POSTSOLVE;

        const auto ca = CollisionActors{aArbiter, aUserData};

        const auto* cf1 = ca.dispatcher->_findCollisionFunc(*ca.delegate1,
                                                            ca.delegate2->getEntityTypeId(),
                                                            USAGE);
        const auto* cf2 = ca.dispatcher->_findCollisionFunc(*ca.delegate2,
                                                            ca.delegate1->getEntityTypeId(),
                                                            USAGE);

        if (cf1) {
            (*cf1)(ca.delegate2->getEntity(), {aArbiter, aSpace, 1});
        }
        if (cf2) {
            (*cf2)(ca.delegate1->getEntity(), {aArbiter, aSpace, 2});
        }
    };

    handler->separateFunc = [](cpArbiter* aArbiter, cpSpace* aSpace, cpDataPointer aUserData) {
        static constexpr auto USAGE = USAGE_COL_SEPARATE;

        const auto ca = CollisionActors{aArbiter, aUserData};

        const auto* cf1 = ca.dispatcher->_findCollisionFunc(*ca.delegate1,
                                                            ca.delegate2->getEntityTypeId(),
                                                            USAGE);
        const auto* cf2 = ca.dispatcher->_findCollisionFunc(*ca.delegate2,
                                                            ca.delegate1->getEntityTypeId(),
                                                            USAGE);

        if (cf1) {
            (*cf1)(ca.delegate2->getEntity(), {aArbiter, aSpace, 1});
        }
        if (cf2) {
            (*cf2)(ca.delegate1->getEntity(), {aArbiter, aSpace, 2});
        }
    };
    // clang-format on
}

} // namespace alvin
HOBGOBLIN_NAMESPACE_END

#include <Hobgoblin/Private/Pmacro_undef.hpp>
