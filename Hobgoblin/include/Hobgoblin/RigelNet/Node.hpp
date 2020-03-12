#ifndef UHOBGOBLIN_RN_NODE_HPP
#define UHOBGOBLIN_RN_NODE_HPP

#include <Hobgoblin/RigelNet/Event.hpp>
#include <Hobgoblin/RigelNet/Handlermgmt.hpp>
#include <Hobgoblin/RigelNet/Packet_wrapper.hpp>
#include <Hobgoblin/Utility/NoCopyNoMove.hpp>

#include <cassert>
#include <cstdint>
#include <deque>
#include <functional>
#include <type_traits>
#include <utility>
#include <variant>

#include <Hobgoblin/Private/Pmacro_define.hpp>

HOBGOBLIN_NAMESPACE_START
namespace rn {

// class RN_TcpClient;
// class RN_TcpServer;
class RN_UdpClient;
class RN_UdpServer;

#define CLIENT 0
#define SERVER 1

enum class RN_NodeType : int {
    // TcpClient = (0 << 1 | CLIENT),
    // TcpServer = (1 << 1 | SERVER),
    UdpClient = (2 << 1 | CLIENT),
    UdpServer = (3 << 1 | SERVER)
};

inline
bool RN_IsServer(RN_NodeType type) {
    return ((static_cast<int>(type) & 1) == SERVER);
}

#undef CLIENT
#undef SERVER

enum class RN_UpdateMode {
    Receive,
    Send
};

class RN_Node;

namespace detail {

// Unpack a packet of data messages and call the required handlers
void HandleDataMessages(RN_Node& node, RN_PacketWrapper& packetWrap);

} // namespace detail

struct RN_ComposeForAllType {};
constexpr RN_ComposeForAllType RN_COMPOSE_FOR_ALL{};

class RN_Node : NO_COPY, NO_MOVE {
public:
    RN_Node(RN_NodeType nodeType);
    virtual ~RN_Node() = 0 {}

    bool pollEvent(RN_Event& ev);

    RN_NodeType getType() const noexcept;

    template <class ...NoArgs>
    std::enable_if_t<sizeof...(NoArgs) == 0, void> visit();

    template <class ArgsHead, class ...ArgsTail>
    void visit(ArgsHead&& argsHead, ArgsTail&&... argsTail);

protected:
    std::deque<RN_Event> _eventQueue;
    friend class detail::EventFactory;

    virtual void compose(RN_ComposeForAllType receiver, const void* data, std::size_t sizeInBytes) = 0;
    virtual void compose(PZInteger receiver, const void* data, std::size_t sizeInBytes) = 0;

private:
    detail::RN_PacketWrapper* _currentPacket;
    RN_NodeType _nodeType;

    template <class T>
    T extractArgument();

    static constexpr bool VISIT_IMPL_MATCHED = true;

    // bool visitImpl(std::function<void(RN_TcpClient&)> callable);
    // bool visitImpl(std::function<void(RN_TcpServer&)> callable);
    bool visitImpl(std::function<void(RN_UdpClient&)> callable);
    bool visitImpl(std::function<void(RN_UdpServer&)> callable);

    template <class R, class ...Args>
    friend void UHOBGOBLIN_RN_ComposeImpl(RN_Node& node, R&& receiver, detail::RN_HandlerId handlerId, Args...args);

    template <class T>
    friend typename std::remove_reference<T>::type UHOBGOBLIN_RN_ExtractArg(RN_Node& node);

    friend void detail::HandleDataMessages(RN_Node& node, RN_PacketWrapper& packetWrap);
};

template <class T>
T RN_Node::extractArgument() {
    T retVal;
    _currentPacket->packet >> retVal;
    return retVal;
}

template <class ...NoArgs>
std::enable_if_t<sizeof...(NoArgs) == 0, void> RN_Node::visit() {
    // Do nothing
}

template <class ArgsHead, class ...ArgsTail>
void RN_Node::visit(ArgsHead&& argsHead, ArgsTail&&... argsTail) {
    if (visitImpl(std::forward<ArgsHead>(argsHead)) == VISIT_IMPL_MATCHED) {
        return;
    }
    visit(std::forward<ArgsTail...>(argsTail)...);
}

template <class R,  class ...Args>
void UHOBGOBLIN_RN_ComposeImpl(RN_Node& node, R&& receivers, detail::RN_HandlerId handlerId, Args... args) {
    detail::RN_PacketWrapper packetWrap;
    packetWrap.insert(handlerId);
    detail::PackArgs(packetWrap, std::forward<Args>(args)...);

    if constexpr (std::is_same_v<std::remove_cv_t<std::remove_reference_t<R>>, RN_ComposeForAllType>) {
        node.compose(RN_ComposeForAllType{}, packetWrap.packet.getData(), packetWrap.packet.getDataSize());
    }
    else if constexpr (std::is_convertible_v<R, PZInteger>) {    
        node.compose(std::forward<R>(receivers), packetWrap.packet.getData(), packetWrap.packet.getDataSize());
    } 
    else {
        for (PZInteger i : std::forward<R>(receivers)) {
            node.compose(i, packetWrap.packet.getData(), packetWrap.packet.getDataSize());
        }
    }
}

template <class T>
typename std::remove_reference<T>::type UHOBGOBLIN_RN_ExtractArg(RN_Node& node) {
    return node.extractArgument<std::remove_reference<T>::type>();
}

} // namespace rn
HOBGOBLIN_NAMESPACE_END

#include <Hobgoblin/Private/Pmacro_undef.hpp>
#include <Hobgoblin/Private/Short_namespace.hpp>

#endif // !UHOBGOBLIN_RN_NODE_HPP
