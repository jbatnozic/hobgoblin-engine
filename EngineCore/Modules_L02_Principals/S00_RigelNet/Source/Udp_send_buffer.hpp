// Copyright 2024 Jovan Batnozic. Released under MS-PL licence in Serbia.
// See https://github.com/jbatnozic/Hobgoblin?tab=readme-ov-file#licence

#ifndef UHOBGOBLIN_RN_UDP_SEND_BUFFER_HPP
#define UHOBGOBLIN_RN_UDP_SEND_BUFFER_HPP

#include <Hobgoblin/Common.hpp>
#include <Hobgoblin/Logging.hpp>
#include <Hobgoblin/RigelNet/Retransmit_predicate.hpp>
#include <Hobgoblin/Utility/Packet.hpp>
#include <Hobgoblin/Utility/Time_utils.hpp>

#include "Invalid_data_error.hpp"
#include "Packet_ordinal.hpp"
#include "Socket_adapter.hpp"
#include "Udp_connector_packet_kinds.hpp"

#include <cstdint>
#include <deque>
#include <vector>

#include <Hobgoblin/Private/Pmacro_define.hpp>

HOBGOBLIN_NAMESPACE_BEGIN
namespace rn {

class UdpReceiveBuffer;

//! Class that handles outgoing packets for a connector.
class UdpSendBuffer {
public:
    //! Constructs the send buffer.
    //! \param aMaxPacketSize maximal packet size (in bytes). Packets larger than this will be
    //!                       fragmented.
    //! \param aRetransmitPredicate reference to a retransmit predicate to use. The original
    //!                             predicate object must outlive the send buffer!
    UdpSendBuffer(PZInteger aMaxPacketSize, const RN_RetransmitPredicate& aRetransmitPredicate);

    //! Returns the length of the buffer (number of packets in it).
    //! \note if length is growing uncontrollably, it means that the packets are not being
    //!       acknowledged (maybe the remote lost connection?), or are being pushed faster
    //!       than they can be sent, or similar.
    PZInteger getLength() const;

    //! Resets the buffer to its initial state.
    void reset();

    //! Appends the given data into one or more outgoing packets (preserving the order of information).
    //!
    //! \param aData pointer to the data.
    //! \param aDataByteCount number of bytes pointed to by aData, must be greater than 0.
    void appendDataForSending(NeverNull<const void*> aData, PZInteger aDataByteCount);

    //! Adds an ACK to be sent to the remote.
    //! \note weak acks are sent in dedicated ACKS packets, and strong acks are send in outgoing DATA
    //!       packets. This method will append the provided ack to one of each.
    //! \param aPacketOrdinal ordinal of the packet to acknowledge.
    void appendAckForSending(PacketOrdinal aPacketOrdinal);

    //! Send weak acks prepared thus far using `appendAckForSending()` in a dedicated ACKS packet.
    //! Usually this will send all of the prepared acks, unless there's so many of them that not
    //! all can fit in a single packet (due to max packet size). In this case, the rest will be sent
    //! the next time this function is called.
    template <class taSendFunction>
    PZInteger sendWeakAcks(const taSendFunction& aSendFunction);

    struct AckReceivedResult {
        //! Time it took from the moment the packet was sent until it was strongly acknowledged.
        std::chrono::microseconds timeToAck;
        bool                      isSignificant;
    };

    //! Informs the buffer about an acknowledged packet. Since the packet is confirmed received
    //! by the remote, the buffer can drop it to preserve memory and doesn't have to try to
    //! resend it.
    //!
    //! \returns `true` if the received ack was significant (strong and never received before),
    //!          and `false` otherwise.
    //!
    //! \throws InvalidDataError if `aPacketOrdinal` points to a packet that hasn't been sent yet.
    AckReceivedResult ackReceived(PacketOrdinal aPacketOrdinal, bool aIsStrong);

    struct SendResult {
        PZInteger                uploadedByteCount; //!< Number of uploaded bytes.
        RN_SocketAdapter::Status socketStatus;      //!< Last status of the socket.
    };

    //! Send packet until no more outgoing packets remain, until the packet limit is reached,
    //! or until an error occurs.
    //!
    //! \param aPacketLimiter pointer to a variable of type PZInteger. Each time an attempt is
    //!                       made to send a packet, this variable is decremented by 1. If it
    //!                       reaches 0, `send()` returns.
    //! \param aCurrentMeanLatency current mean latency (round-trip) to the remote; needed for
    //!                            retransmit decisions.
    //! \param aSendFunction callable object of type `RN_SocketAdapter::Status(util::Packet&)`
    //!                      which will be used to send packets. It should return the status of
    //!                      the socket after sending. As soon as it returns anything other than
    //!                      'OK', sending stops and `send()` returns, regardless of the number
    //!                      of remaining packets or the value of the packet limiter.
    //!
    //! \return object of type `SendResult` that holds information about how many bytes were sent
    //!         and about the last status returned by `aSendFunction` (and remember that sending
    //!         stops after the first value that's not 'OK').
    template <class taSendFunction>
    SendResult sendData(NeverNull<PZInteger*>     aPacketLimiter,
                        std::chrono::microseconds aCurrentMeanLatency,
                        const taSendFunction&     aSendFunction);

    //! Moves all the prepared packet out of the buffer, in the order in which they need to be sent.
    //! \note this method exists solely to support local connections; DO NOT use it in true online
    //!       scenarios!
    std::vector<util::Packet> exportPackets();

private:
    PZInteger _maxPacketSize;

    const RN_RetransmitPredicate& _retransmitPredicate;

    struct TaggedPacket {
        enum Tag {
            READY_FOR_SENDING,
            NOT_ACKNOWLEDGED,
            ACKNOWLEDGED_WEAKLY,
            ACKNOWLEDGED_STRONGLY,
        };

        util::Packet    packet;
        util::Stopwatch stopwatch; //!< Measures time since last upload (or upload attempt).
        PZInteger       cyclesSinceLastTransmit = 0;
        Tag             tag                     = READY_FOR_SENDING;
    };

    std::deque<TaggedPacket> _packets;
    PacketOrdinal            _headOrdinal = 1;

    std::vector<PacketOrdinal> _weakAcks;
    std::vector<PacketOrdinal> _strongAcks;

    static constexpr PZInteger UDP_HEADER_BYTE_COUNT = 8;

    TaggedPacket& _getTailPacket();
    void          _prepareNextOutgoingDataPacket(std::uint32_t aPacketType);
    void          _changePacketKind(TaggedPacket& aTaggedPacket, std::uint32_t aNewKind);
};

template <class taSendFunction>
UdpSendBuffer::SendResult UdpSendBuffer::sendData(NeverNull<PZInteger*>     aPacketLimiter,
                                                  std::chrono::microseconds aCurrentMeanLatency,
                                                  const taSendFunction&     aSendFunction) {
    PZInteger uploadedByteCount = 0;

    for (auto& taggedPacket : _packets) {
        if (*aPacketLimiter == 0) {
            break;
        }

        if (taggedPacket.tag == TaggedPacket::ACKNOWLEDGED_WEAKLY ||
            taggedPacket.tag == TaggedPacket::ACKNOWLEDGED_STRONGLY) {
            continue;
        }

        if ((taggedPacket.tag == TaggedPacket::READY_FOR_SENDING) ||
            _retransmitPredicate(taggedPacket.cyclesSinceLastTransmit,
                                 taggedPacket.stopwatch.getElapsedTime(),
                                 aCurrentMeanLatency)) {

            *aPacketLimiter -= 1;

            switch (RN_SocketAdapter::Status status = aSendFunction(taggedPacket.packet)) {
            case RN_SocketAdapter::Status::OK:
                uploadedByteCount += stopz(taggedPacket.packet.getDataSize() + UDP_HEADER_BYTE_COUNT);
                break;

            case RN_SocketAdapter::Status::NotReady:
                uploadedByteCount += stopz(taggedPacket.packet.getDataSize() + UDP_HEADER_BYTE_COUNT);
                return {uploadedByteCount, RN_SocketAdapter::Status::NotReady};

            case RN_SocketAdapter::Status::Disconnected:
                return {uploadedByteCount, RN_SocketAdapter::Status::Disconnected};

            default:
                HG_UNREACHABLE("Invalid value for RN_SocketAdapter::Status ({}).", (int)status);
            }

            taggedPacket.stopwatch.restart();
            taggedPacket.cyclesSinceLastTransmit = 0;
        }

        taggedPacket.cyclesSinceLastTransmit += 1;
        taggedPacket.tag = TaggedPacket::NOT_ACKNOWLEDGED;
    } // end for

    _prepareNextOutgoingDataPacket(UDP_PACKET_KIND_DATA);

    return {uploadedByteCount, RN_SocketAdapter::Status::OK};
}

template <class taSendFunction>
PZInteger UdpSendBuffer::sendWeakAcks(const taSendFunction& aSendFunction) {
    static constexpr auto LOG_ID = "Hobgoblin.RigelNet";

    if (_weakAcks.empty()) {
        return 0;
    }

    util::Packet packet;
    packet << UDP_PACKET_KIND_ACKS;

    const std::size_t limit =
        static_cast<std::size_t>(_maxPacketSize - packet.getDataSize()) / sizeof(PacketOrdinal);

    if (_weakAcks.size() <= limit) {
        for (PacketOrdinal ack : _weakAcks) {
            packet << ack;
        }
        _weakAcks.clear();
    } else {
        const auto originalSize = stopz(_weakAcks.size());
        HG_LOG_WARN(LOG_ID, "Excessive amount of pending weak acks ({})!", originalSize);

        for (std::size_t i = 0; i < limit; i += 1) {
            packet << _weakAcks[pztos(i)];
        }

        _weakAcks.erase(_weakAcks.begin(), _weakAcks.begin() + limit);
        HG_ASSERT(stopz(_weakAcks.size()) == originalSize - limit);
    }

    const PZInteger dataSize = packet.getDataSize();
    aSendFunction(packet);
    return dataSize;
}

} // namespace rn
HOBGOBLIN_NAMESPACE_END

#include <Hobgoblin/Private/Pmacro_undef.hpp>

#endif // !UHOBGOBLIN_RN_UDP_SEND_BUFFER_HPP
