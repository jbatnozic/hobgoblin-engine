#ifndef UHOBGOBLIN_RN_NODE_HPP
#define UHOBGOBLIN_RN_NODE_HPP

//#include <Hobgoblin/Common.hpp>
#include <Hobgoblin/RigelNet/handlermgmt.hpp>
#include <Hobgoblin/RigelNet/packet.hpp>
#include <Hobgoblin/Utility/NoCopyNoMove.hpp>

#include <cstdint>
#include <deque>
#include <type_traits>

#include <Hobgoblin/Private/pmacro_define.hpp>

HOBGOBLIN_NAMESPACE_START
namespace rn {

class RN_Node;

namespace detail {

// Unpack a packet of data messages and call the required handlers
void HandleDataMessages(RN_Node& node, RN_Packet& packet);

} // namespace detail

class RN_Node : NO_COPY, NO_MOVE {
public:
    virtual ~RN_Node() = 0 {}

    // Compose template

    bool pollEvent(int& ev);

    // virtual RN_NodeType getType() const;

protected:
    std::deque<int> _eventQueue;

    void  clearEvents();
    void  queueEvent(int ev);

    // virtual std::vector<RN_Packet*> getPacketsToComposeInto() const;

    virtual void compose(int receiver, const void* data, std::size_t sizeInBytes) = 0;

private:
    RN_PacketBase* _currentPacket;

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

#include <Hobgoblin/Private/pmacro_undef.hpp>

#endif // !UHOBGOBLIN_RN_NODE_HPP
