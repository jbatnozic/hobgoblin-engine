#ifndef UHOBGOBLIN_GRAPHICS_ORIGIN_OFFSET_HPP
#define UHOBGOBLIN_GRAPHICS_ORIGIN_OFFSET_HPP

#include <Hobgoblin/Math.hpp>

#include <Hobgoblin/Private/Pmacro_define.hpp>

HOBGOBLIN_NAMESPACE_BEGIN
namespace gr {

struct OriginOffset {
    math::Vector2f offset;

    enum Kind {
        RELATIVE_TO_TOP_LEFT,
        RELATIVE_TO_CENTER
    };

    Kind kind;
};

} // namespace gr
HOBGOBLIN_NAMESPACE_END

#include <Hobgoblin/Private/Pmacro_undef.hpp>
#include <Hobgoblin/Private/Short_namespace.hpp>

#endif // !UHOBGOBLIN_GRAPHICS_ORIGIN_OFFSET_HPP
