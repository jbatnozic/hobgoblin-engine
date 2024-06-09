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

#include <cstdint>
#include <optional>

#include <Hobgoblin/Private/Pmacro_define.hpp>

HOBGOBLIN_NAMESPACE_BEGIN
namespace alvin {

class CollisionDelegateBuilder;
class MainCollisionDispatcher;

class CollisionDelegate {
public:
    //! Deleted constructor.
    //!
    //! To construct an instance of this class, use a `CollisionDelegateBuilder`.
    CollisionDelegate() = delete;

    //! Prevent copying.
    CollisionDelegate(const CollisionDelegate&) = delete;

    //! Prevent copying.
    CollisionDelegate& operator=(const CollisionDelegate&) = delete;

    //! Allow cheap moving.
    //!
    //! \throws TracedLogicError if `aOther` is already bound to one or more shapes.
    CollisionDelegate(CollisionDelegate&& aOther);

    //! Prevent move assignment.
    CollisionDelegate& operator=(CollisionDelegate&&) = delete;

    //! Binds to a single shape.
    //!
    //! \note Once a delegate is bound, it cannot be moved anymore!
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

    //! Binds to multiple shapes.
    //!
    //! \note Once a delegate is bound:
    //!       - it cannot be moved anymore,
    //!       - it has to outlive the shapes it is bound to.
    template <class taEntity, class taShapeBeginIterator, class taShapeEndIterator>
    void bindMultiple(taEntity&                aEntity,
                      taShapeBeginIterator     aShapeBeginIterator,
                      taShapeEndIterator       aShapeEndIterator,
                      std::optional<cpGroup>   aGroup        = std::nullopt,
                      std::optional<cpBitmask> aCategory     = std::nullopt,
                      std::optional<cpBitmask> aCollidesWith = std::nullopt) {
        HG_HARD_ASSERT(aShapeBeginIterator != aShapeEndIterator);

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

    Decision getDefaultDecision() const {
        return _defaultDecision;
    }

private:
    friend class CollisionDelegateBuilder;
    friend class MainCollisionDispatcher;

    CollisionDelegate(std::vector<detail::SpecificCollisionFunc> aCollisionFunctions,
                      Decision                                   aDefaultDecision,
                      std::uint8_t                               aFuncBitmask)
        : _collisionFunctions{std::move(aCollisionFunctions)}
        , _defaultDecision{aDefaultDecision}
        , _funcBitmask{aFuncBitmask} {}

    std::vector<detail::SpecificCollisionFunc> _collisionFunctions;

    EntityBase*  _entity = nullptr;
    EntityTypeId _entityTypeId;
    Decision     _defaultDecision;
    std::uint8_t _funcBitmask = 0;
    bool         _isBound     = false;

    bool _hasNoFunctionsForUsage(detail::Usage aUsage) const {
        return (_funcBitmask & (0x01 << aUsage)) == 0;
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

///////////////////////////////////////////////////////////////////////////
// IMPLEMENTATIONS                                                       //
///////////////////////////////////////////////////////////////////////////

inline CollisionDelegate::CollisionDelegate(CollisionDelegate&& aOther)
    : _collisionFunctions{}
    , _entity{aOther._entity}
    , _entityTypeId{aOther._entityTypeId}
    , _defaultDecision{aOther._defaultDecision}
    , _isBound{false} {
    if (aOther._isBound) {
        HG_THROW_TRACED(TracedLogicError, 0, "Attempting to move a bound CollisionDelegate.");
    }
    _collisionFunctions = std::move(aOther._collisionFunctions);
}

#ifdef COLLISION_DELEGATE_ENABLE_MOVE_ASSIGNMENT
inline CollisionDelegate& CollisionDelegate::operator=(CollisionDelegate&& aOther) {
    if (aOther._isBound) {
        HG_THROW_TRACED(TracedLogicError, 0, "Attempting to move a bound CollisionDelegate.");
    }
    if (this != &aOther) {
        _collisionFunctions = std::move(aOther._collisionFunctions);
        _entity             = aOther._entity;
        _entityTypeId       = aOther._entityTypeId;
        _defaultDecision    = aOther._defaultDecision;
        _isBound            = false;
    }
    return SELF;
}
#endif

} // namespace alvin
HOBGOBLIN_NAMESPACE_END

#include <Hobgoblin/Private/Pmacro_undef.hpp>
#include <Hobgoblin/Private/Short_namespace.hpp>

#endif // !UHOBGOBLIN_ALVIN_COLLISION_DELEGATE_HPP
