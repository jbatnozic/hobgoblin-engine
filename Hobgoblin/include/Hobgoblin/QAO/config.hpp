#ifndef UHOBGOBLIN_QAO_CONFIG_HPP
#define UHOBGOBLIN_QAO_CONFIG_HPP

#include <Hobgoblin/common.hpp>

#include <cstdint>
#include <limits>

#include <Hobgoblin/Private/Pmacro_define.hpp>

HOBGOBLIN_NAMESPACE_START
namespace qao {

static constexpr PZInteger QAO_NULL_INDEX = std::numeric_limits<PZInteger>::max();
static constexpr std::int64_t QAO_NULL_SERIAL = -1;
static constexpr std::int64_t QAO_MIN_SERIAL = 0;

struct QAO_Event {
    enum Enum {
        StartFrame,
        PreUpdate,
        Update,
        PostUpdate,
        Draw1,
        Draw2,
        DrawGUI,
        FinalizeFrame,

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

#include <Hobgoblin/Private/Pmacro_undef.hpp>
#include <Hobgoblin/Private/Short_namespace.hpp>

#endif // !UHOBGOBLIN_QAO_CONFIG_HPP