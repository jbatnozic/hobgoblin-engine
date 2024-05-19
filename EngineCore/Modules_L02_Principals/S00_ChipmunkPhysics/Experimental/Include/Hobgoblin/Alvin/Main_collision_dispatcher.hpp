// Copyright 2024 Jovan Batnozic. Released under MS-PL licence in Serbia.
// See https://github.com/jbatnozic/Hobgoblin?tab=readme-ov-file#licence

#ifndef UHOBGOBLIN_ALVIN_MAIN_COLLISION_DISPATCHER_HPP
#define UHOBGOBLIN_ALVIN_MAIN_COLLISION_DISPATCHER_HPP

#include <Hobgoblin/Common.hpp>
#include <Hobgoblin/HGExcept.hpp>

#include <Hobgoblin/Alvin/Entity_base.hpp>

#include <type_traits>
#include <unordered_map>

#include <Hobgoblin/Private/Pmacro_define.hpp>

HOBGOBLIN_NAMESPACE_BEGIN
namespace alvin {

// to have one per context
class MainCollisionDispatcher {
public:
    MainCollisionDispatcher() {
        _registerEntityType<EntityBase>();
    }

    template <class taEntityType,
              T_ENABLE_IF(std::is_base_of_v<EntityBase, taEntityType> &&
                          !std::is_same_v<typename taEntityType::EntitySuperclass, void>)>
    MainCollisionDispatcher& registerEntityType() {
        const auto pair =
            _registry.insert(std::make_pair(taEntityType::ENTITY_TYPE_ID,
                                            taEntityType::EntitySuperclass::ENTITY_TYPE_ID));

        if (!pair.second) {
            HG_THROW_TRACED(TracedLogicError,
                            0,
                            "Entity type with ID {} was already registered.",
                            taEntityType::ENTITY_TYPE_ID);
        }

        return SELF;
    }

    void configureSpace(NeverNull<cpSpace*> aSpace) {
        _initCols(aSpace);
    }

private:
    //! Key: type ID of entity
    //! Val: type ID of key's superclass
    std::unordered_map<EntityTypeId, EntityTypeId> _registry;

    //! Register function for `EntityBase`.
    template <class taEntityType,
              T_ENABLE_IF(std::is_same_v<typename taEntityType::EntitySuperclass, void>)>
    void _registerEntityType() {
        const auto pair =
            _registry.insert(std::make_pair(taEntityType::ENTITY_TYPE_ID, ENTITY_TYPE_ID_VOID));
        if (!pair.second) {
            HG_THROW_TRACED(TracedLogicError,
                            0,
                            "Entity type with ID {} was already registered.",
                            taEntityType::ENTITY_TYPE_ID);
        }
    }

    const detail::GenericEntityCollisionFunc* _findCollisionFunc(const CollisionDelegate& aDelegate,
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

    void _initCols(cpSpace* aSpace) {
        using namespace detail;

        auto* handler     = cpSpaceAddDefaultCollisionHandler(aSpace);
        handler->userData = this;

        handler->beginFunc =
            [](cpArbiter* aArbiter, cpSpace* aSpace, cpDataPointer aUserData) -> cpBool {
            // Note: AnyPtr::get guarantees not-null
            auto* dispatcher = aUserData.get<MainCollisionDispatcher>();

            // Get pointers to shapes
            CP_ARBITER_GET_SHAPES(aArbiter, shape1, shape2);
            // Get pointers to collision delegates
            const auto* del1 = cpShapeGetUserData(shape1).get<CollisionDelegate>();
            const auto* del2 = cpShapeGetUserData(shape2).get<CollisionDelegate>();
            // Get pointers to collision functions
            const auto* cf1 =
                dispatcher->_findCollisionFunc(*del1, del2->getEntityTypeId(), USAGE_COL_BEGIN);
            const auto* cf2 =
                dispatcher->_findCollisionFunc(*del2, del1->getEntityTypeId(), USAGE_COL_BEGIN);
            // Get results of collision functions (or default values)
            const Decision result1 =
                cf1 ? ((*cf1)(del2->getEntity(), aArbiter, aSpace, 1)) : del1->getDefaultDecision();
            const Decision result2 =
                cf2 ? ((*cf2)(del1->getEntity(), aArbiter, aSpace, 2)) : del2->getDefaultDecision();

            return (detail::ToBool(result1) && detail::ToBool(result2));
        };

        handler->preSolveFunc =
            [](cpArbiter* aArbiter, cpSpace* aSpace, cpDataPointer aUserData) -> cpBool {
            // Note: AnyPtr::get guarantees not-null
            auto* dispatcher = aUserData.get<MainCollisionDispatcher>();

            // Get pointers to shapes
            CP_ARBITER_GET_SHAPES(aArbiter, shape1, shape2);
            // Get pointers to collision delegates
            const auto* del1 = cpShapeGetUserData(shape1).get<CollisionDelegate>();
            const auto* del2 = cpShapeGetUserData(shape2).get<CollisionDelegate>();
            // Get pointers to collision functions
            const auto* cf1 =
                dispatcher->_findCollisionFunc(*del1, del2->getEntityTypeId(), USAGE_COL_PRESOLVE);
            const auto* cf2 =
                dispatcher->_findCollisionFunc(*del2, del1->getEntityTypeId(), USAGE_COL_PRESOLVE);
            // Get results of collision functions (or default values)
            const Decision result1 =
                cf1 ? ((*cf1)(del2->getEntity(), aArbiter, aSpace, 1)) : del1->getDefaultDecision();
            const Decision result2 =
                cf2 ? ((*cf2)(del1->getEntity(), aArbiter, aSpace, 2)) : del2->getDefaultDecision();

            return (detail::ToBool(result1) && detail::ToBool(result2));
        };

        handler->postSolveFunc = [](cpArbiter* aArbiter, cpSpace* aSpace, cpDataPointer aUserData) {
            // Note: AnyPtr::get guarantees not-null
            auto* dispatcher = aUserData.get<MainCollisionDispatcher>();

            // Get pointers to shapes
            CP_ARBITER_GET_SHAPES(aArbiter, shape1, shape2);
            // Get pointers to collision delegates
            const auto* del1 = cpShapeGetUserData(shape1).get<CollisionDelegate>();
            const auto* del2 = cpShapeGetUserData(shape2).get<CollisionDelegate>();
            // Get pointers to collision functions
            const auto* cf1 =
                dispatcher->_findCollisionFunc(*del1, del2->getEntityTypeId(), USAGE_COL_POSTSOLVE);
            const auto* cf2 =
                dispatcher->_findCollisionFunc(*del2, del1->getEntityTypeId(), USAGE_COL_POSTSOLVE);
            // Get results of collision functions (or default values)
            if (cf1) {
                (*cf1)(del2->getEntity(), aArbiter, aSpace, 1);
            }
            if (cf2) {
                (*cf2)(del1->getEntity(), aArbiter, aSpace, 2);
            }
        };

        handler->separateFunc = [](cpArbiter* aArbiter, cpSpace* aSpace, cpDataPointer aUserData) {
            // Note: AnyPtr::get guarantees not-null
            auto* dispatcher = aUserData.get<MainCollisionDispatcher>();

            // Get pointers to shapes
            CP_ARBITER_GET_SHAPES(aArbiter, shape1, shape2);
            // Get pointers to collision delegates
            const auto* del1 = cpShapeGetUserData(shape1).get<CollisionDelegate>();
            const auto* del2 = cpShapeGetUserData(shape2).get<CollisionDelegate>();
            // Get pointers to collision functions
            const auto* cf1 =
                dispatcher->_findCollisionFunc(*del1, del2->getEntityTypeId(), USAGE_COL_SEPARATE);
            const auto* cf2 =
                dispatcher->_findCollisionFunc(*del2, del1->getEntityTypeId(), USAGE_COL_SEPARATE);
            // Get results of collision functions (or default values)
            if (cf1) {
                (*cf1)(del2->getEntity(), aArbiter, aSpace, 1);
            }
            if (cf2) {
                (*cf2)(del1->getEntity(), aArbiter, aSpace, 2);
            }
        };
    }
};

} // namespace alvin
HOBGOBLIN_NAMESPACE_END

#include <Hobgoblin/Private/Pmacro_undef.hpp>
#include <Hobgoblin/Private/Short_namespace.hpp>

#endif // !UHOBGOBLIN_ALVIN_MAIN_COLLISION_DISPATCHER_HPP
