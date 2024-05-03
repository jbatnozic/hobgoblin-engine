// Copyright 2024 Jovan Batnozic. Released under MS-PL licence in Serbia.
// See https://github.com/jbatnozic/Hobgoblin?tab=readme-ov-file#licence

#ifndef UHOBGOBLIN_ALVIN_COLLISION_DELEGATE_HPP
#define UHOBGOBLIN_ALVIN_COLLISION_DELEGATE_HPP

#include <Hobgoblin/ChipmunkPhysics.hpp>
#include <Hobgoblin/Common.hpp>
#include <Hobgoblin/HGExcept.hpp>

#include <Hobgoblin/Alvin/Entity_base.hpp>
#include <Hobgoblin/Alvin/Private/Helpers.hpp>

#include <Hobgoblin/Private/Pmacro_define.hpp>

HOBGOBLIN_NAMESPACE_BEGIN
namespace alvin {

class CollisionDelegateBuilder;
class MainCollisionDispatcher;

class CollisionDelegate {
public:
    //! Prevent copying.
    CollisionDelegate(const CollisionDelegate&) = delete;
    //! Prevent copying.
    CollisionDelegate& operator=(const CollisionDelegate&) = delete;

    //! Allow cheap moving.
    CollisionDelegate(CollisionDelegate&&) = default;
    //! Allow cheap moving.
    CollisionDelegate& operator=(CollisionDelegate&&) = default;

    template <class taCollideable>
    void bind(taCollideable& aCollideable, HG_NEVER_NULL(cpBody*) aBody) {
        _entity = &aCollideable;
        _entityTypeId = taCollideable::ENTITY_TYPE_ID;
        cpBodySetUserData(aBody, this);
    }

    EntityBase& getEntity() const {
        HG_ASSERT(_entity != nullptr);
        return *_entity;
    }

    EntityTypeId getEntityTypeId() const {
        HG_ASSERT(_entity != nullptr);
        return _entityTypeId;
    }

    bool getDefaultDecision() const {
        return _defaultDecision;
    }

private:
    friend class CollisionDelegateBuilder;
    friend class MainCollisionDispatcher;

    explicit CollisionDelegate(std::vector<detail::SpecificCollisionFunc> aCollisionFunctions)
        : _collisionFunctions{std::move(aCollisionFunctions)} {}

    std::vector<detail::SpecificCollisionFunc> _collisionFunctions;

    EntityBase*  _entity = nullptr;
    EntityTypeId _entityTypeId;
    bool         _defaultDecision = true; // TODO: initialize

    //! Returns `true` if the delegate contains no collision
    //! functions whatsoever; `false` otherwise.
    bool _isTrivial() const {
        return _collisionFunctions.empty();
    }

    const detail::GenericEntityCollisionFunc* _findCollisionFunc(EntityTypeId  aEntityTypeId,
                                                                 detail::Usage aUsage) const {
        // TODO: do binary search
        for (const auto& fn : _collisionFunctions) {
            if (fn.specifier.getEntityTypeId() == aEntityTypeId && fn.specifier.getUsage() == aUsage) {
                return &fn.func;
            }
        }
        return nullptr;
    }
};

} // namespace alvin
HOBGOBLIN_NAMESPACE_END

#include <Hobgoblin/Private/Pmacro_undef.hpp>
#include <Hobgoblin/Private/Short_namespace.hpp>

#endif // !UHOBGOBLIN_ALVIN_COLLISION_DELEGATE_HPP
