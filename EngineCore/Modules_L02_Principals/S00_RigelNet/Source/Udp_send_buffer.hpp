// Copyright 2024 Jovan Batnozic. Released under MS-PL licence in Serbia.
// See https://github.com/jbatnozic/Hobgoblin?tab=readme-ov-file#licence

#ifndef UHOBGOBLIN_RN_UDP_SEND_BUFFER_HPP
#define UHOBGOBLIN_RN_UDP_SEND_BUFFER_HPP

#include <Hobgoblin/Common.hpp>
#include <Hobgoblin/Utility/Packet.hpp>
#include <Hobgoblin/Utility/Time_utils.hpp>

#include "Socket_adapter.hpp"
#include "Udp_connector_packet_kinds.hpp"

#include <cstdint>
#include <deque>
#include <vector>

#include <Hobgoblin/Private/Pmacro_define.hpp>

HOBGOBLIN_NAMESPACE_BEGIN
namespace rn {

class UdpReceiveBuffer;

using PacketOrdinal = std::uint32_t;

class UdpSendBuffer {
public:
    UdpSendBuffer(PZInteger aMaxPacketSize, const RN_RetransmitPredicate& aRetransmitPredicate);

    //! Resets the buffer to its initial state.
    void reset();

    //! Appends the given data into one or more outgoing packets
    //! (preserving the order of information).
    void appendDataForSending(const void* aData, PZInteger aDataByteCount);

    struct AckReceivedResult {
        //! Time it took from the moment the packet was sent until it was strongly acknowledged.
        std::chrono::microseconds timeToAck;
        bool isSignificant;
    };

    // Returns true if the received ack was significant (strong and never received before).
    AckReceivedResult ackReceived(PacketOrdinal aPacketOrdinal, bool aIsStrong);

    void prepareAck(PacketOrdinal aPacketOrdinal);

    struct SendResult {
        PZInteger                uploadedByteCount;
        RN_SocketAdapter::Status socketStatus;
    };

    //! aSendFunction = RN_SocketAdapter::Status(util::Packet&)
    template <class taSendFunction>
    SendResult send(NeverNull<PZInteger*> aPacketLimiter, const taSendFunction& aSendFunction);

    // for local connections
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

    std::vector<PacketOrdinal> _acknowledges;

    static constexpr PZInteger UDP_HEADER_BYTE_COUNT = 8;

    TaggedPacket& _getTailPacket();
    void          _prepareNextOutgoingDataPacket(std::uint32_t aPacketType);
    void          _changePacketKind(TaggedPacket& aTaggedPacket, std::uint32_t aNewKind);
};

template <class taSendFunction>
UdpSendBuffer::SendResult UdpSendBuffer::send(NeverNull<PZInteger*> aPacketLimiter,
                                              const taSendFunction& aSendFunction) {
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
                                 /*TODO _remoteInfo.meanLatency*/ std::chrono::milliseconds{0})) {

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

} // namespace rn
HOBGOBLIN_NAMESPACE_END

#include <Hobgoblin/Private/Pmacro_undef.hpp>

#endif // !UHOBGOBLIN_RN_UDP_SEND_BUFFER_HPP
