#ifndef UHOBGOBLIN_QAO_CONFIG_HPP
#define UHOBGOBLIN_QAO_CONFIG_HPP

#include <Hobgoblin/Common.hpp>

#include <cstdint>
#include <limits>

#include <Hobgoblin/Private/Pmacro_define.hpp>

HOBGOBLIN_NAMESPACE_BEGIN
namespace qao {

static constexpr PZInteger QAO_NULL_INDEX = std::numeric_limits<PZInteger>::max();
static constexpr std::int64_t QAO_NULL_SERIAL = -1;
static constexpr std::int64_t QAO_MIN_SERIAL = 0;

struct QAO_Event {
    enum Enum {
        NONE = -1,

        PRE_UPDATE,
        BEGIN_UPDATE,
        UPDATE_1,
        UPDATE_2,
        END_UPDATE,
        POST_UPDATE,

        PRE_DRAW,
        DRAW_1,
        DRAW_2,
        DRAW_GUI,
        POST_DRAW,

        DISPLAY,

        EVENT_COUNT,
    };
};

namespace qao_detail {

struct QAO_SerialIndexPair {
    std::int64_t serial;
    PZInteger index;
};

} // namespace qao_detail
} // namespace qao
HOBGOBLIN_NAMESPACE_END

#include <Hobgoblin/Private/Pmacro_undef.hpp>
#include <Hobgoblin/Private/Short_namespace.hpp>

#endif // !UHOBGOBLIN_QAO_CONFIG_HPP