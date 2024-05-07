// Copyright 2024 Jovan Batnozic. Released under MS-PL licence in Serbia.
// See https://github.com/jbatnozic/Hobgoblin?tab=readme-ov-file#licence

#ifndef UHOBGOBLIN_ALVIN_ENTITY_BASE_HPP
#define UHOBGOBLIN_ALVIN_ENTITY_BASE_HPP

#include <Hobgoblin/Common.hpp>
#include <Hobgoblin/ChipmunkPhysics.hpp>

#include <Hobgoblin/Private/Pmacro_define.hpp>

HOBGOBLIN_NAMESPACE_BEGIN
namespace alvin {

using EntityTypeId = std::uint32_t;

constexpr EntityTypeId ENTITY_TYPE_ID_VOID = 0xFFFF'FFFFu; // (UINT32_MAX)
constexpr EntityTypeId ENTITY_TYPE_ID_BASE = 0xFFFF'FFFEu; // (UINT32_MAX - 1)

class EntityBase {
public:
    using EntitySuperclass = void;
    static constexpr std::int32_t ENTITY_TYPE_ID = ENTITY_TYPE_ID_BASE;
    
    static constexpr cpBitmask ENTITY_DEFAULT_CATEGORY = CP_ALL_CATEGORIES;
    static constexpr cpBitmask ENTITY_DEFAULT_MASK = CP_ALL_CATEGORIES;
};

} // namespace alvin
HOBGOBLIN_NAMESPACE_END

#include <Hobgoblin/Private/Pmacro_undef.hpp>
#include <Hobgoblin/Private/Short_namespace.hpp>

#endif // !UHOBGOBLIN_ALVIN_ENTITY_BASE_HPP
