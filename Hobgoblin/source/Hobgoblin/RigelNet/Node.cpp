
#include <Hobgoblin/RigelNet/Handlermgmt.hpp>
#include <Hobgoblin/RigelNet/Node.hpp>
#include <Hobgoblin/RigelNet/Udp_client.hpp>
#include <Hobgoblin/RigelNet/Udp_server.hpp>

#include <Hobgoblin/Private/Pmacro_define.hpp>

HOBGOBLIN_NAMESPACE_START
namespace rn {

// TODO Node impl.

namespace detail {

void HandleDataMessages(RN_Node& node, RN_PacketWrapper& packetWrap) {
    node._currentPacket = &packetWrap;

    while (!packetWrap.packet.endOfPacket()) {
        const RN_HandlerId handlerId = packetWrap.extractOrThrow<RN_HandlerId>();
        RN_HandlerFunc handlerFunc = RN_GlobalHandlerMapper::getInstance().handlerWithId(handlerId);
        (*handlerFunc)(node);
    }

    node._currentPacket = nullptr;
}

} // namespace detail

RN_Node::RN_Node(RN_NodeType nodeType)
    : _currentPacket{nullptr}
    , _nodeType{nodeType}
{
}

bool RN_Node::pollEvent(RN_Event& ev) {
    if (_eventQueue.empty()) {
        return false;
    }
    ev = _eventQueue.front();
    _eventQueue.pop_front();
    return true;
}

RN_NodeType RN_Node::getType() const noexcept {
    return _nodeType;
}

bool RN_Node::visitImpl(std::function<void(RN_UdpClient&)> callable) {
    if (_nodeType != RN_NodeType::UdpClient) {
        return !VISIT_IMPL_MATCHED;
    }
    callable(static_cast<RN_UdpClient&>(*this));
    return VISIT_IMPL_MATCHED;
}

bool RN_Node::visitImpl(std::function<void(RN_UdpServer&)> callable) {
    if (_nodeType != RN_NodeType::UdpServer) {
        return !VISIT_IMPL_MATCHED;
    }
    callable(static_cast<RN_UdpServer&>(*this));
    return VISIT_IMPL_MATCHED;
}

} // namespace rn
HOBGOBLIN_NAMESPACE_END

#include <Hobgoblin/Private/Pmacro_undef.hpp>
