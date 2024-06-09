// Copyright 2024 Jovan Batnozic. Released under MS-PL licence in Serbia.
// See https://github.com/jbatnozic/Hobgoblin?tab=readme-ov-file#licence

#ifndef UHOBGOBLIN_ALVIN_ENTITY_BASE_HPP
#define UHOBGOBLIN_ALVIN_ENTITY_BASE_HPP

#include <Hobgoblin/ChipmunkPhysics.hpp>
#include <Hobgoblin/Common.hpp>

#include <Hobgoblin/Private/Pmacro_define.hpp>

HOBGOBLIN_NAMESPACE_BEGIN
namespace alvin {

using EntityTypeId = std::uint32_t;

constexpr EntityTypeId ENTITY_TYPE_ID_VOID = 0xFFFF'FFFFu; // (UINT32_MAX)
constexpr EntityTypeId ENTITY_TYPE_ID_BASE = 0xFFFF'FFFEu; // (UINT32_MAX - 1)

class EntityBase {
public:
    using EntitySuperclass = void;

    static constexpr EntityTypeId ENTITY_TYPE_ID = ENTITY_TYPE_ID_BASE;

    //! Bitmask of categories the entity belongs to (by default; it can be overriden
    //! for each specific entity if needed).
    //! For example, if bits 1, 3 and 7 are set, the entity belongs to categories
    //! 1, 3 and 7. As the `cpBitmask` type is guaranteed to be at least 64 bits, you
    //! will have at least 64 distinct categories to work with.
    static constexpr cpBitmask ENTITY_DEFAULT_CATEGORY = CP_ALL_CATEGORIES;

    //! Bitmask of categories the entity can colllide with (by default; it can be overriden
    //! for each specific entity if needed).
    //! For example, if bits 2 and 4 are set, the entity will collide with all other entities
    //! that have either bit 2 or bit 4 (or both) set in their category bitmask.
    static constexpr cpBitmask ENTITY_DEFAULT_MASK = CP_ALL_CATEGORIES;

    //! Pure virtual destructor.
    virtual ~EntityBase() = 0;
};

inline EntityBase::~EntityBase() = default;

static_assert(sizeof(cpBitmask) >= sizeof(std::int64_t), "cpBitmask must support at least 64 bits.");

} // namespace alvin
HOBGOBLIN_NAMESPACE_END

#include <Hobgoblin/Private/Pmacro_undef.hpp>
#include <Hobgoblin/Private/Short_namespace.hpp>

#endif // !UHOBGOBLIN_ALVIN_ENTITY_BASE_HPP
