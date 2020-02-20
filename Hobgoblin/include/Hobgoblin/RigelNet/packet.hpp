#ifndef UHOBGOBLIN_RN_PACKET_HPP
#define UHOBGOBLIN_RN_PACKET_HPP

#include <SFML/Network/Packet.hpp>

#include <utility>

#include <Hobgoblin/Private/pmacro_define.hpp>

HOBGOBLIN_NAMESPACE_START
namespace rn {

using RN_PacketBase = sf::Packet;

class RN_Packet : public RN_PacketBase {
public:

    template <class T>
    T extractValue();

protected:
};

template <class T>
T RN_Packet::extractValue() {
    T value;
    Self >> value;
    return value;
}

namespace detail {

template <class ... NoArgs>
typename std::enable_if_t<sizeof...(NoArgs) == 0, void> PackArgs(RN_PacketBase& packet) {
    // Do nothing
}

template <class ArgsHead, class ... ArgsRest>
void PackArgs(RN_PacketBase& packet, ArgsHead argsHead, ArgsRest&&... argsRest) {
    packet << argsHead;
    PackArgs<ArgsRest...>(packet, std::forward<ArgsRest>(argsRest)...);
}

} // namespace detail

} // namespace rn
HOBGOBLIN_NAMESPACE_END

#include <Hobgoblin/Private/pmacro_undef.hpp>

#endif // !UHOBGOBLIN_RN_PACKET_HPP