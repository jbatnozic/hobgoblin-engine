#ifndef UHOBGOBLIN_RN_NODE_HPP
#define UHOBGOBLIN_RN_NODE_HPP

#include <Hobgoblin/RigelNet/Client.hpp>
#include <Hobgoblin/RigelNet/Handlermgmt.hpp>
#include <Hobgoblin/RigelNet/Packet.hpp>
#include <Hobgoblin/RigelNet/Server.hpp>
#include <Hobgoblin/Utility/NoCopyNoMove.hpp>

#include <cassert>
#include <cstdint>
#include <deque>
#include <type_traits>
#include <variant>

#include <Hobgoblin/Private/Pmacro_define.hpp>

HOBGOBLIN_NAMESPACE_START
namespace rn {

enum class RN_NodeType {
    TcpClient,
    TcpServer,
    UdpClient,
    UdpServer
};

enum class RN_UpdateMode {
    Receive,
    Send
};

class RN_Node;

namespace detail {

// Unpack a packet of data messages and call the required handlers
void HandleDataMessages(RN_Node& node, RN_Packet& packet);

} // namespace detail

class RN_Node : NO_COPY, NO_MOVE {
public:
    RN_Node(RN_NodeType nodeType);
    virtual ~RN_Node() = 0 {}

    // Compose template

    bool pollEvent(int& ev);

    RN_NodeType getType() const noexcept;

    template <class S, class C>
    void visit(S&& serverVisitor, C&& clientVisitor);

protected:
    std::deque<int> _eventQueue;

    void  clearEvents();
    void  queueEvent(int ev);

    // virtual std::vector<RN_Packet*> getPacketsToComposeInto() const;

    virtual void compose(int receiver, const void* data, std::size_t sizeInBytes) = 0;

private:
    RN_PacketBase* _currentPacket;
    RN_NodeType _nodeType;

    template <class T>
    T extractArgument();

    template <class ... Args>
    friend void UHOBGOBLIN_RN_ComposeImpl(RN_Node& node, int receiver, detail::RN_HandlerId handlerId, Args... args);

    template <class T>
    friend typename std::remove_reference<T>::type UHOBGOBLIN_RN_ExtractArg(RN_Node& node);

    friend void detail::HandleDataMessages(RN_Node& node, RN_Packet& packet);
};

template <class T>
T RN_Node::extractArgument() {
    T retVal;
    (*_currentPacket) >> retVal;
    if (!(*_currentPacket)) {
        // TODO Handle error
    }
    return retVal;
}

/*template <class S, class C>
void RN_Node::visit(S&& serverVisitor, C&& clientVisitor) {
    switch (_nodeType) {
    case RN_NodeType::UdpServer:
        serverVisitor(static_cast<RN_Server&>(static_cast<RN_UdpServer&>(Self)));
        break;

    case RN_NodeType::UdpClient:
        clientVisitor(static_cast<RN_Client&>(static_cast<RN_UdpClient&>(Self)));
        break;

    case RN_NodeType::TcpServer:
    case RN_NodeType::TcpClient:
    default:
        assert(0 && "Unreachable");
    }
}*/

template <class ... Args>
void UHOBGOBLIN_RN_ComposeImpl(RN_Node& node, int receiver, detail::RN_HandlerId handlerId, Args... args) {
    // TODO
    RN_Packet packet;
    packet << handlerId;
    detail::PackArgs(packet, std::forward<Args>(args)...);
    node.compose(receiver, packet.getData(), packet.getDataSize());
}

template <class T>
typename std::remove_reference<T>::type UHOBGOBLIN_RN_ExtractArg(RN_Node& node) {
    return node.extractArgument<std::remove_reference<T>::type>();
}

} // namespace rn
HOBGOBLIN_NAMESPACE_END

#include <Hobgoblin/Private/Pmacro_undef.hpp>

#endif // !UHOBGOBLIN_RN_NODE_HPP
