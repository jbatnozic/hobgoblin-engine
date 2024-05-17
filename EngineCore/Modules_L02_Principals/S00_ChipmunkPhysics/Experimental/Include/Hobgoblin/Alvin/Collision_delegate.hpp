// Copyright 2024 Jovan Batnozic. Released under MS-PL licence in Serbia.
// See https://github.com/jbatnozic/Hobgoblin?tab=readme-ov-file#licence

#ifndef UHOBGOBLIN_ALVIN_COLLISION_DELEGATE_HPP
#define UHOBGOBLIN_ALVIN_COLLISION_DELEGATE_HPP

#include <Hobgoblin/ChipmunkPhysics.hpp>
#include <Hobgoblin/Common.hpp>
#include <Hobgoblin/HGExcept.hpp>

#include <Hobgoblin/Alvin/Entity_base.hpp>
#include <Hobgoblin/Alvin/Private/Helpers.hpp>
#include <Hobgoblin/Alvin/Shape.hpp>

#include <optional>

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

    //! Binds to a single shape
    template <class taEntity>
    void bind(taEntity&                aEntity,
              Shape&                   aShape,
              std::optional<cpGroup>   aGroup        = std::nullopt,
              std::optional<cpBitmask> aCategory     = std::nullopt,
              std::optional<cpBitmask> aCollidesWith = std::nullopt) {
        _entity       = &aEntity;
        _entityTypeId = taEntity::ENTITY_TYPE_ID;
        cpShapeSetUserData(aShape, this);

        const auto filter = cpShapeFilterNew(aGroup.value_or(CP_NO_GROUP),
                                             aCategory.value_or(taEntity::ENTITY_DEFAULT_CATEGORY),
                                             aCollidesWith.value_or(taEntity::ENTITY_DEFAULT_MASK));
        cpShapeSetFilter(aShape, filter);
    }

    //! Binds to multiple shapes
    template <class taEntity, class taShapeBeginIterator, class taShapeEndIterator>
    void bind(taEntity&                aEntity,
              taShapeBeginIterator     aShapeBeginIterator,
              taShapeEndIterator       aShapeEndIterator,
              std::optional<cpGroup>   aGroup        = std::nullopt,
              std::optional<cpBitmask> aCategory     = std::nullopt,
              std::optional<cpBitmask> aCollidesWith = std::nullopt) {
        _entity       = &aEntity;
        _entityTypeId = taEntity::ENTITY_TYPE_ID;

        const auto filter = cpShapeFilterNew(aGroup.value_or(CP_NO_GROUP),
                                             aCategory.value_or(taEntity::ENTITY_DEFAULT_CATEGORY),
                                             aCollidesWith.value_or(taEntity::ENTITY_DEFAULT_MASK));

        auto iter = aShapeBeginIterator;
        while (iter != aShapeEndIterator) {
            auto& shape = *iter;
            cpShapeSetUserData(shape, this);
            cpShapeSetFilter(shape, filter);
            iter = std::next(iter);
        }
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
        const auto target = detail::EntityTypeIdAndUsage{aEntityTypeId, aUsage};

        std::size_t low  = 0;
        std::size_t high = _collisionFunctions.size();

        while (low < high) {
            const std::size_t pivot = (low + high) / 2;
            if (_collisionFunctions[pivot].specifier == target) {
                return &(_collisionFunctions[pivot].func);
            }
            if (_collisionFunctions[pivot].specifier < target) {
                low = pivot + 1;
                continue;
            }
            if (_collisionFunctions[pivot].specifier > target) {
                high = pivot;
                continue;
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
