// Copyright 2024 Jovan Batnozic. Released under MS-PL licence in Serbia.
// See https://github.com/jbatnozic/Hobgoblin?tab=readme-ov-file#licence

#ifndef UHOBGOBLIN_RN_UDP_RECEIVE_BUFFER_HPP
#define UHOBGOBLIN_RN_UDP_RECEIVE_BUFFER_HPP

#include <Hobgoblin/Common.hpp>
#include <Hobgoblin/Utility/Packet.hpp>

#include "Socket_adapter.hpp"
#include "Udp_connector_packet_kinds.hpp"

#include <cstdint>
#include <deque>
#include <vector>

#include <Hobgoblin/Private/Pmacro_define.hpp>

HOBGOBLIN_NAMESPACE_BEGIN
namespace rn {

using PacketOrdinal = std::uint32_t;

class UdpReceiveBuffer {
public:
    UdpReceiveBuffer() = default;

    //! Resets the buffer to its initial state.
    void reset();

    // returns acks contained within
    std::vector<PacketOrdinal> dataPacketReceived(util::Packet  aPacket,
                                                  PacketOrdinal aPacketOrdinal,
                                                  std::uint32_t aPacketKind);

    bool takeNextReadyPacket(NeverNull<util::Packet*> aPacket);

private:
    struct TaggedPacket {
        enum Tag {
            WAITING_FOR_DATA,
            FRAGMENT,
            FRAGMENT_TAIL,
            READY_FOR_UNPACKING,
            UNPACKED
        };

        util::Packet packet;
        Tag          tag = WAITING_FOR_DATA;
    };

    std::deque<TaggedPacket> _packets;
    PacketOrdinal            _headOrdinal = 1;

    void _tryToAssembleFragmentedPacketAtHead();
};

} // namespace rn
HOBGOBLIN_NAMESPACE_END

#include <Hobgoblin/Private/Pmacro_undef.hpp>

#endif // !UHOBGOBLIN_RN_UDP_RECEIVE_BUFFER_HPP
