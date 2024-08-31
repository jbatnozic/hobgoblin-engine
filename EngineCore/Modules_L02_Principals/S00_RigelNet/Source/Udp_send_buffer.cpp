// Copyright 2024 Jovan Batnozic. Released under MS-PL licence in Serbia.
// See https://github.com/jbatnozic/Hobgoblin?tab=readme-ov-file#licence

#include "Udp_send_buffer.hpp"
#include "Udp_connector_packet_kinds.hpp"

#include <Hobgoblin/HGExcept.hpp>
#include <Hobgoblin/Logging.hpp>

#include <string>

#include <Hobgoblin/Private/Pmacro_define.hpp>

HOBGOBLIN_NAMESPACE_BEGIN
namespace rn {

namespace {
constexpr auto LOG_ID = "Hobgoblin.RigelNet";

constexpr PZInteger MAX_STRONG_ACKNOWLEDGES_PER_PACKET = 16;
// clang-format off
constexpr PZInteger MAX_PACKET_HEADER_BYTE_COUNT = 
      sizeof(std::uint32_t) * 1                                  // Packet type
    + sizeof(PacketOrdinal) * 1                                  // Packet ordinal
    + sizeof(PacketOrdinal) * MAX_STRONG_ACKNOWLEDGES_PER_PACKET // Strong acknowledges
    + sizeof(PacketOrdinal) * 1                                  // Acknowledges terminator
    ;
// clang-format on

//! Endianess-agnostic implementation of ntoh for 32bit integers
std::int32_t ntoh32(std::int32_t net) {
    // clang-format off
    std::uint8_t buf[4];
    std::memcpy(&buf, &net, sizeof(buf));

    return ((std::uint32_t) buf[3] <<  0)
         | ((std::uint32_t) buf[2] <<  8)
         | ((std::uint32_t) buf[1] << 16)
         | ((std::uint32_t) buf[0] << 24);
    // clang-format on
}

std::int32_t hton32(std::int32_t host) {
    return ntoh32(host); // These functions actually perform the same operation :)
}
} // namespace

UdpCommunicationBuffer::UdpCommunicationBuffer(PZInteger                     aMaxPacketSize,
                                               const RN_RetransmitPredicate& aRetransmitPredicate)
    : _maxPacketSize{aMaxPacketSize}
    , _retransmitPredicate{aRetransmitPredicate} {
    _prepareNextOutgoingDataPacket(UDP_PACKET_KIND_DATA);
}

void UdpCommunicationBuffer::reset() {
    _packets.clear();
    _headOrdinal = 1;
    _acknowledges.clear();

    _prepareNextOutgoingDataPacket(UDP_PACKET_KIND_DATA);
}

void UdpCommunicationBuffer::appendDataForSending(const void* aData, PZInteger aDataByteCount) {
    HG_HARD_ASSERT(aData != nullptr);
    HG_HARD_ASSERT(aDataByteCount > 0);

    // We want to send independent DATA packets whenever possible,
    // and fragmented only when necessary.
    if (auto& tail = _getTailPacket(); tail.packet.getDataSize() + aDataByteCount <= _maxPacketSize) {
        tail.packet.appendBytes(aData, aDataByteCount);
        return;
    } else if (aDataByteCount + MAX_PACKET_HEADER_BYTE_COUNT <= _maxPacketSize) {
        _prepareNextOutgoingDataPacket(UDP_PACKET_KIND_DATA);
        auto& tail = _getTailPacket();
        tail.packet.appendBytes(aData, aDataByteCount);
        return;
    }

    // At this point, we have to send a fragmented packet

    // Prepare the current latest outgoing packet (finalize it if it's full enough, and
    // set its type to DATA_MORE otherwise):
    {
        auto& tail = _getTailPacket();

        // This is kind of an arbitrarily chosen limit, but if the latest outgoing packet is at
        // least 50% full, we'll send it independently so avoid dependencies between packets.
        if (tail.packet.getDataSize() > _maxPacketSize / 2) {
            _prepareNextOutgoingDataPacket(UDP_PACKET_KIND_DATA_MORE);
        } else {
            // Otherwise we must edit the type of the packet onto which we're going to start
            // appending the data to DATA_MORE, so that the recepient knows not to do anything
            // with it until the remaining fragments are also received and assembled.
            _changePacketKind(tail, UDP_PACKET_KIND_DATA_MORE);
        }
    }

    // Pack the data into multiple consecutive packets:
    PZInteger bytesPacked = 0;
    while (true) {
        auto& tail = _getTailPacket();

        // assert(pztos(_maxPacketSize) >= sendBufTail.packet.getDataSize());

        const PZInteger remainingCapacity = _maxPacketSize - tail.packet.getDataSize();
        const PZInteger bytesToPackNow    = std::min(remainingCapacity, aDataByteCount - bytesPacked);

        tail.packet.appendBytes(static_cast<const char*>(aData) + bytesPacked, bytesToPackNow);
        bytesPacked += bytesToPackNow;

        if (bytesPacked < aDataByteCount) {
            _prepareNextOutgoingDataPacket(UDP_PACKET_KIND_DATA_MORE);
        } else {
            break;
        }
    }

    // Mark the last outgoing packet as DATA_TAIL:
    {
        auto& tail = _getTailPacket();
        _changePacketKind(tail, UDP_PACKET_KIND_DATA_TAIL);
    }

    // We don't want chaining of multiple fragmented packets, so finalize the tail and
    // start the next regular packet:
    _prepareNextOutgoingDataPacket(UDP_PACKET_KIND_DATA);
}

#if 0
void RN_UdpConnectorImpl::_receivedAck(std::uint32_t ordinal, bool strong) {
    ...
    } else {
        const auto timeToAck = _sendBuffer[ind].stopwatch.getElapsedTime<std::chrono::microseconds>();
        _newMeanLatency += timeToAck;
        if (_newLatencySampleSize == 0) {
            _newOptimisticLatency  = decltype(_newOptimisticLatency){0};
            _newPessimisticLatency = decltype(_newPessimisticLatency){0};
        } else {
            _newOptimisticLatency  = std::min(_newOptimisticLatency, timeToAck);
            _newPessimisticLatency = std::max(_newPessimisticLatency, timeToAck);
        }
        _newLatencySampleSize += 1;
        _remoteInfo.timeoutStopwatch.restart();

        _sendBuffer[ind].tag = TaggedPacket::AcknowledgedStrongly;
        _sendBuffer[ind].packet.clear();

        ...
    }
}
#endif

void UdpCommunicationBuffer::ackReceived(PacketOrdinal aPacketOrdinal, bool aIsStrong) {
    if (aPacketOrdinal < _headOrdinal) {
        return; // Already acknowledged before
    }

    const std::uint32_t indexInBuffer = (aPacketOrdinal - _headOrdinal);
    if (indexInBuffer >= _packets.size()) {
        // TODO -- Error
        HG_THROW_TRACED(TracedRuntimeError, 0, "Received ACK for packet that's not yet sent!");
    }

    auto& target = _packets[indexInBuffer];

    if (!aIsStrong) {
        switch (target.tag) {
        case TaggedPacket::NOT_ACKNOWLEDGED:
            target.tag = TaggedPacket::ACKNOWLEDGED_WEAKLY;
            break;

        case TaggedPacket::ACKNOWLEDGED_WEAKLY:
        case TaggedPacket::ACKNOWLEDGED_STRONGLY:
            // No action needed
            break;

        default:
            HG_UNREACHABLE("Unexpected value for target.tag at this point ({}).", (int)target.tag);
        }
        target.packet.clear();
        return;
    }

    // TODO(timing stuff)

    target.tag = TaggedPacket::ACKNOWLEDGED_STRONGLY;
    target.packet.clear();

    if (indexInBuffer == 0) {
        while (!_packets.empty() && _packets.front().tag == TaggedPacket::ACKNOWLEDGED_STRONGLY) {
            _packets.pop_front();
            _headOrdinal += 1;
        }
        if (_packets.empty()) {
            _prepareNextOutgoingDataPacket(UDP_PACKET_KIND_DATA);
        }
    }
}

std::vector<util::Packet> UdpCommunicationBuffer::exportPackets() {
    std::vector<util::Packet> result;
    result.reserve(_packets.size());
    
    while (_packets.size() > 1) {
        auto& packet = _packets.front();
        result.emplace_back(std::move(packet.packet));
        _packets.pop_front();
        _headOrdinal += 1;
    }

    if (_packets.front().packet.getDataSize() > 0) {
        auto& packet = _packets.front();
        result.emplace_back(std::move(packet.packet));
        _packets.pop_front();
        _headOrdinal += 1;

        _prepareNextOutgoingDataPacket(UDP_PACKET_KIND_DATA);
    }

    return result;
}

///////////////////////////////////////////////////////////////////////////
// MARK: PRIVATE METHODS                                                 //
///////////////////////////////////////////////////////////////////////////

UdpCommunicationBuffer::TaggedPacket& UdpCommunicationBuffer::_getTailPacket() {
    HG_HARD_ASSERT(!_packets.empty());
    return _packets.back();
}

void UdpCommunicationBuffer::_prepareNextOutgoingDataPacket(std::uint32_t aPacketType) {
    _packets.emplace_back();
    _packets.back().tag = TaggedPacket::Tag::READY_FOR_SENDING;

    util::Packet& packet = _packets.back().packet;

    // Message type:
    packet << aPacketType;

    // Message ordinal:
    packet << static_cast<PacketOrdinal>(_packets.size() + pztos(_headOrdinal) - 1u);

    // Strong Acknowledges (zero-terminated):
    if (_acknowledges.size() <= pztos(MAX_STRONG_ACKNOWLEDGES_PER_PACKET)) {
        for (PacketOrdinal ack : _acknowledges) {
            packet << ack;
        }
        packet << (PacketOrdinal)0;
        _acknowledges.clear();
    } else {
        const auto originalSize = stopz(_acknowledges.size());
        HG_LOG_WARN(LOG_ID, "Excessive amount of pending acknowledges ({})!", originalSize);

        for (PZInteger i = 0; i < MAX_STRONG_ACKNOWLEDGES_PER_PACKET; i += 1) {
            packet << _acknowledges[pztos(i)];
        }
        packet << (PacketOrdinal)0;
        _acknowledges.erase(_acknowledges.begin(),
                            _acknowledges.begin() + MAX_STRONG_ACKNOWLEDGES_PER_PACKET);
        HG_ASSERT(stopz(_acknowledges.size()) == originalSize - MAX_STRONG_ACKNOWLEDGES_PER_PACKET);
    }
}

void UdpCommunicationBuffer::_changePacketKind(TaggedPacket& aTaggedPacket, std::uint32_t aNewKind) {
    const auto newKindInNetworkOrder =
        static_cast<std::uint32_t>(hton32(static_cast<std::int32_t>(aNewKind)));

    auto& packet = aTaggedPacket.packet;
    HG_HARD_ASSERT(packet.getDataSize() >= sizeof(newKindInNetworkOrder));

    // The first 4 bytes of a packet determine its kind
    auto* kindPtr = packet.getMutableData();

    std::memcpy(packet.getMutableData(), &newKindInNetworkOrder, sizeof(newKindInNetworkOrder));
}

} // namespace rn
HOBGOBLIN_NAMESPACE_END

#include <Hobgoblin/Private/Pmacro_undef.hpp>
