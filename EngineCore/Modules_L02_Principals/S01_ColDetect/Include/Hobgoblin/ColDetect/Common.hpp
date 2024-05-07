// Copyright 2024 Jovan Batnozic. Released under MS-PL licence in Serbia.
// See https://github.com/jbatnozic/Hobgoblin?tab=readme-ov-file#licence

// clang-format off

#ifndef UHOBGOBLIN_CD_COMMON_HPP
#define UHOBGOBLIN_CD_COMMON_HPP

#include <Hobgoblin/Common.hpp>
#include <Hobgoblin/Utility/Any_ptr.hpp>
#include <Hobgoblin/Utility/Rectangle.hpp>

#include <cstdint>
#include <variant>
#include <utility>

#include <Hobgoblin/Private/Pmacro_define.hpp>

HOBGOBLIN_NAMESPACE_BEGIN
namespace cd {

using BoundingBox = util::Rectangle<double>;

using EntityTag = 
    std::variant<
        util::AnyPtr,
        std::int32_t,
        std::uint32_t,
        std::int64_t,
        std::uint64_t,
        void*
    >;

using CollisionPair = std::pair<EntityTag, EntityTag>;

template <class T>
EntityTag MakeTag(T&& value) {
    EntityTag rv{};
    rv = std::forward<T>(value);
    return rv;
}

} // namespace cd
HOBGOBLIN_NAMESPACE_END

#include <Hobgoblin/Private/Pmacro_undef.hpp>
#include <Hobgoblin/Private/Short_namespace.hpp>

#endif // !UHOBGOBLIN_CD_COMMON_HPP

// clang-format on
