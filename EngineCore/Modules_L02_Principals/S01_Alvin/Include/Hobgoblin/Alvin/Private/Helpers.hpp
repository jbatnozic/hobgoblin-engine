// Copyright 2024 Jovan Batnozic. Released under MS-PL licence in Serbia.
// See https://github.com/jbatnozic/Hobgoblin?tab=readme-ov-file#licence

#ifndef UHOBGOBLIN_ALVIN_PRIVATE_HELPERS_HPP
#define UHOBGOBLIN_ALVIN_PRIVATE_HELPERS_HPP

#include <Hobgoblin/Alvin/Decision.hpp>
#include <Hobgoblin/Alvin/Entity_base.hpp>

#include <cstdint>
#include <functional>

#include <Hobgoblin/Private/Pmacro_define.hpp>

HOBGOBLIN_NAMESPACE_BEGIN
namespace alvin {
namespace detail {

using GenericEntityCollisionFunc =
    std::function<Decision(EntityBase&, NeverNull<cpArbiter*>, NeverNull<cpSpace*>, PZInteger)>;

enum class Usage : std::uint32_t {
    BEGIN,
    PRESOLVE,
    POSTSOLVE,
    SEPARATE
};

struct EntityTypeIdAndUsage {
    EntityTypeIdAndUsage(EntityTypeId aEntityTypeId, Usage aUsage)
        : _storage{(static_cast<std::uint64_t>(aUsage) << 32) |
                   static_cast<std::uint64_t>(aEntityTypeId)} {}

    Usage getUsage() const {
        return static_cast<Usage>((_storage >> 32) & 0x0000'0000'FFFF'FFFFull);
    }

    EntityTypeId getEntityTypeId() const {
        return static_cast<EntityTypeId>(_storage & 0x0000'0000'FFFF'FFFFull);
    }

    bool operator==(const EntityTypeIdAndUsage& aOther) const {
        return _storage == aOther._storage;
    }

    bool operator<(const EntityTypeIdAndUsage& aOther) const {
        return _storage < aOther._storage;
    }

    bool operator>(const EntityTypeIdAndUsage& aOther) const {
        return _storage > aOther._storage;
    }

private:
    // Usage in higher 32 bits, EntityTypeId in lower 32 bits
    std::uint64_t _storage;
};

struct SpecificCollisionFunc {
    SpecificCollisionFunc(GenericEntityCollisionFunc aFunc, EntityTypeIdAndUsage aSpecifier)
        : func{std::move(aFunc)}
        , specifier{aSpecifier} {}

    GenericEntityCollisionFunc func;
    EntityTypeIdAndUsage       specifier;

    bool operator<(const SpecificCollisionFunc& aOther) const {
        return specifier < aOther.specifier;
    }
};

} // namespace detail
} // namespace alvin
HOBGOBLIN_NAMESPACE_END

#include <Hobgoblin/Private/Pmacro_undef.hpp>
#include <Hobgoblin/Private/Short_namespace.hpp>

#endif // !UHOBGOBLIN_ALVIN_PRIVATE_HELPERS_HPP
