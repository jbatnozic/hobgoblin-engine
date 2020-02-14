#ifndef UHOBGOBLIN_QAO_CONFIG_HPP
#define UHOBGOBLIN_QAO_CONFIG_HPP

#include <Hobgoblin_include/common.hpp>

#include <cstdint>
#include <limits>

#include <Hobgoblin_include/Private/pmacro_define.hpp>

HOBGOBLIN_NAMESPACE_START
namespace qao {

static constexpr PZInteger QAO_NULL_INDEX = std::numeric_limits<PZInteger>::max();
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
    PZInteger index;
};

} // namespace detail
} // namespace qao
HOBGOBLIN_NAMESPACE_END

#include <Hobgoblin_include/Private/pmacro_undef.hpp>

#endif // !UHOBGOBLIN_QAO_CONFIG_HPP