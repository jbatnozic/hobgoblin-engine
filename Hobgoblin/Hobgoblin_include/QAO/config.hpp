#ifndef UHOBGOBLIN_QAO_CONFIG_HPP
#define UHOBGOBLIN_QAO_CONFIG_HPP

#include <cstdint>

#include <Hobgoblin_include/Private/pmacro_define.hpp>

HOBGOBLIN_NAMESPACE_START
namespace qao {

static constexpr int QAO_NULL_INDEX = -1;
static constexpr std::int64_t QAO_NULL_SERIAL = -1;
static constexpr std::int64_t QAO_MIN_SERIAL = 0;

struct QAO_Event {
    enum Enum {
        FrameStart,
        PreUpdate,
        Update,
        PostUpdate,
        Draw1,
        Draw2,
        DrawGUI,
        FrameEnd,

        Count,
        NoEvent
    };
};

namespace detail {

struct QAO_SerialIndexPair {
    std::int64_t serial;
    int index;
};

} // namespace detail
} // namespace qao
HOBGOBLIN_NAMESPACE_END

#include <Hobgoblin_include/Private/pmacro_undef.hpp>

#endif // !UHOBGOBLIN_QAO_CONFIG_HPP