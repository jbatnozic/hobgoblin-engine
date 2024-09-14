// Copyright 2024 Jovan Batnozic. Released under MS-PL licence in Serbia.
// See https://github.com/jbatnozic/Hobgoblin?tab=readme-ov-file#licence

#ifndef UHOBGOBLIN_RN_UDP_RECEIVE_BUFFER_HPP
#define UHOBGOBLIN_RN_UDP_RECEIVE_BUFFER_HPP

#include <Hobgoblin/Common.hpp>
#include <Hobgoblin/Utility/Packet.hpp>

#include "Packet_ordinal.hpp"
#include "Socket_adapter.hpp"
#include "Udp_connector_packet_kinds.hpp"

#include <cstdint>
#include <deque>
#include <vector>

#include <Hobgoblin/Private/Pmacro_define.hpp>

HOBGOBLIN_NAMESPACE_BEGIN
namespace rn {

//! Class that handles incoming packets for a connector.
class UdpReceiveBuffer {
public:
    //! Default constructor.
    UdpReceiveBuffer() = default;

    //! Returns the length of the buffer (number of packets in it).
    //! \note if length is growing uncontrollably, it means that one of the packets
    //!       wasn't received and so all the packets that come after it can't be processed.
    //!       This shouldn't really happen due to RigelNet's retransmit behaviour.
    PZInteger getLength() const;

    //! Resets the buffer to its initial state.
    void reset();

    //! Stores a received Data packet, if this same packet (detemined by its ordinal) hasn't
    //! already been received before.
    //!
    //! \param aPacket the received packet. The function assumes that the first 8 bytes have
    //!                already been read from it (packet kind and ordinal).
    //! \param aPacketOrdinal ordinal of the received packet.
    //! \param aPacketKind kind of the received packet.
    //!
    //! \returns a vector of strong acks contained in this packet (which will be empty if this
    //!          same packet has already been received and stores before).
    //!
    //! \throws InvalidDataError in case the kind of the packet is invalid (not data).
    std::vector<PacketOrdinal> storeDataPacket(util::Packet  aPacket,
                                               PacketOrdinal aPacketOrdinal,
                                               std::uint32_t aPacketKind);

    //! Attempt to take the next packet ready for processing. If such a packet exists, its
    //! contents will be moved into the packet pointed to by the passed pointer and `true`
    //! will be returned. Otherwise, nothing happens and `false` is returned.
    //!
    //! \throws InvalidDataError in case invalid data is found in the buffer.
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
