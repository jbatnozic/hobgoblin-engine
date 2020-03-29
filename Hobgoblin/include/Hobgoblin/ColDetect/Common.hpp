#ifndef UHOBGOBLIN_CD_COMMON_HPP
#define UHOBGOBLIN_CD_COMMON_HPP

#include <Hobgoblin/Common.hpp>
#include <Hobgoblin/Utility/Any_ptr.hpp>
#include <Hobgoblin/Utility/Rectangle.hpp>

#include <cstdint>
#include <variant>

#include <Hobgoblin/Private/Pmacro_define.hpp>

HOBGOBLIN_NAMESPACE_START
namespace cd {

using BoundingBox = util::Rectangle<double>;

using EntityTag = 
    std::variant<
        util::AnyPtr,
        PZInteger,
        std::int32_t,
        std::uint32_t,
        std::int64_t,
        std::uint64_t,
        void*
    >;

} // namespace cd
HOBGOBLIN_NAMESPACE_END

#include <Hobgoblin/Private/Pmacro_undef.hpp>
#include <Hobgoblin/Private/Short_namespace.hpp>

#endif // !UHOBGOBLIN_CD_COMMON_HPP