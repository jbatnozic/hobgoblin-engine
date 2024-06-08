// Copyright 2024 Jovan Batnozic. Released under MS-PL licence in Serbia.
// See https://github.com/jbatnozic/Hobgoblin?tab=readme-ov-file#licence

#ifndef UHOBGOBLIN_ALVIN_MAIN_COLLISION_DISPATCHER_HPP
#define UHOBGOBLIN_ALVIN_MAIN_COLLISION_DISPATCHER_HPP

#include <Hobgoblin/Common.hpp>
#include <Hobgoblin/HGExcept.hpp>

#include <Hobgoblin/Alvin/Collision_delegate.hpp>
#include <Hobgoblin/Alvin/Entity_base.hpp>
#include <Hobgoblin/Alvin/Private/Helpers.hpp>

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

    void bind(NeverNull<cpSpace*> aSpace);

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
                                                                 detail::Usage            aUsage);
};

} // namespace alvin
HOBGOBLIN_NAMESPACE_END

#include <Hobgoblin/Private/Pmacro_undef.hpp>
#include <Hobgoblin/Private/Short_namespace.hpp>

#endif // !UHOBGOBLIN_ALVIN_MAIN_COLLISION_DISPATCHER_HPP
