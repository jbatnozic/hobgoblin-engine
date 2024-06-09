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
    MainCollisionDispatcher();

    //! Prevent copying.
    MainCollisionDispatcher(const MainCollisionDispatcher&) = delete;

    //! Prevent copying.
    MainCollisionDispatcher& operator=(const MainCollisionDispatcher&) = delete;

    //! Allow cheap moving.
    //!
    //! \throws TracedLogicError if `aOther` is already bound to one or more shapes.
    MainCollisionDispatcher(MainCollisionDispatcher&& aOther);

    //! Prevent move assignment.
    MainCollisionDispatcher& operator=(MainCollisionDispatcher&&) = delete;

    //! TODO(add description)
    template <class taEntityType,
              T_ENABLE_IF(std::is_base_of_v<EntityBase, taEntityType> &&
                          !std::is_same_v<typename taEntityType::EntitySuperclass, void>)>
    MainCollisionDispatcher& registerEntityType();

    //! TODO(add description)
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

///////////////////////////////////////////////////////////////////////////
// IMPLEMENTATIONS                                                       //
///////////////////////////////////////////////////////////////////////////

template <class taEntityType,
          T_ENABLE_IF_OOC(std::is_base_of_v<EntityBase, taEntityType> &&
                          !std::is_same_v<typename taEntityType::EntitySuperclass, void>)>
MainCollisionDispatcher& MainCollisionDispatcher::registerEntityType() {
    const auto pair = _registry.insert(
        std::make_pair(taEntityType::ENTITY_TYPE_ID, taEntityType::EntitySuperclass::ENTITY_TYPE_ID));

    if (!pair.second) {
        HG_THROW_TRACED(TracedLogicError,
                        0,
                        "Entity type with ID {} was already registered.",
                        taEntityType::ENTITY_TYPE_ID);
    }

    return SELF;
}

} // namespace alvin
HOBGOBLIN_NAMESPACE_END

#include <Hobgoblin/Private/Pmacro_undef.hpp>
#include <Hobgoblin/Private/Short_namespace.hpp>

#endif // !UHOBGOBLIN_ALVIN_MAIN_COLLISION_DISPATCHER_HPP
