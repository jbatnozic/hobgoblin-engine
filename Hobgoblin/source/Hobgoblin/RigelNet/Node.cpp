
#include <Hobgoblin/RigelNet/Handlermgmt.hpp>
#include <Hobgoblin/RigelNet/Node.hpp>

#include <Hobgoblin/Private/Pmacro_define.hpp>

HOBGOBLIN_NAMESPACE_START
namespace rn {

// TODO Node impl.

namespace detail {

void HandleDataMessages(RN_Node& node, RN_Packet& packet) {
    node._currentPacket = &packet;

    while (!packet.endOfPacket()) {
        RN_HandlerId handlerId = packet.extractValue<RN_HandlerId>();
        if (!packet) {
            // TODO - Handle error
        }

        RN_HandlerFunc handlerFunc = RN_GlobalHandlerMapper::getInstance().handlerWithId(handlerId);
        (*handlerFunc)(node);
    }

    node._currentPacket = nullptr;
}

} // namespace detail

} // namespace rn
HOBGOBLIN_NAMESPACE_END

#include <Hobgoblin/Private/Pmacro_undef.hpp>
