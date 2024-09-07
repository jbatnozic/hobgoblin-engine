// Copyright 2024 Jovan Batnozic. Released under MS-PL licence in Serbia.
// See https://github.com/jbatnozic/Hobgoblin?tab=readme-ov-file#licence

#include "Udp_send_buffer.hpp"
#include "Udp_connector_packet_kinds.hpp"

#include <Hobgoblin/HGExcept.hpp>
#include <Hobgoblin/Logging.hpp>

#include <algorithm>
#include <string>

#include <Hobgoblin/Private/Pmacro_define.hpp>

HOBGOBLIN_NAMESPACE_BEGIN
namespace rn {

namespace {
constexpr auto LOG_ID = "Hobgoblin.RigelNet";

constexpr PZInteger MIN_STRONG_ACKNOWLEDGES_PER_PACKET = 0;
constexpr PZInteger MAX_STRONG_ACKNOWLEDGES_PER_PACKET = 16;
// clang-format off
constexpr PZInteger MAX_PACKET_HEADER_BYTE_COUNT = 
      sizeof(std::uint32_t) * 1                                  // Packet type
    + sizeof(PacketOrdinal) * 1                                  // Packet ordinal
    + sizeof(PacketOrdinal) * MAX_STRONG_ACKNOWLEDGES_PER_PACKET // Strong acknowledges
    + sizeof(PacketOrdinal) * 1                                  // Acknowledges terminator
    ;
constexpr PZInteger MIN_PACKET_HEADER_BYTE_COUNT = 
      sizeof(std::uint32_t) * 1                                  // Packet type
    + sizeof(PacketOrdinal) * 1                                  // Packet ordinal
    + sizeof(PacketOrdinal) * MIN_STRONG_ACKNOWLEDGES_PER_PACKET // Strong acknowledges
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

UdpSendBuffer::UdpSendBuffer(PZInteger                     aMaxPacketSize,
                             const RN_RetransmitPredicate& aRetransmitPredicate)
    : _maxPacketSize{aMaxPacketSize}
    , _retransmitPredicate{aRetransmitPredicate} {
    _prepareNextOutgoingDataPacket(UDP_PACKET_KIND_DATA);
}

void UdpSendBuffer::reset() {
    _packets.clear();
    _headOrdinal = 1;
    _acknowledges.clear();

    _prepareNextOutgoingDataPacket(UDP_PACKET_KIND_DATA);
}

PZInteger UdpSendBuffer::getLength() const {
    return stopz(_packets.size());
}

void UdpSendBuffer::appendDataForSending(NeverNull<const void*> aData, PZInteger aDataByteCount) {
    HG_HARD_ASSERT(aDataByteCount > 0);

    const auto headerSizeOfNextPacket = [this]() -> PZInteger {
        return MIN_PACKET_HEADER_BYTE_COUNT +
               std::min(stopz(_acknowledges.size()), MAX_STRONG_ACKNOWLEDGES_PER_PACKET) *
                   sizeof(PacketOrdinal);
    };

    // We want to send independent DATA packets whenever possible,
    // and fragmented only when necessary.
    if (auto& tail = _getTailPacket(); tail.packet.getDataSize() + aDataByteCount <= _maxPacketSize) {
        tail.packet.appendBytes(aData, aDataByteCount);
        return;
    } else if (aDataByteCount + MAX_PACKET_HEADER_BYTE_COUNT <= _maxPacketSize) {
        _prepareNextOutgoingDataPacket(UDP_PACKET_KIND_DATA);
        auto& tail = _getTailPacket();
        tail.packet.appendBytes(aData, aDataByteCount);
        HG_ASSERT(tail.packet.getDataSize() <= _maxPacketSize);
        return;
    } else if (aDataByteCount + headerSizeOfNextPacket() <= _maxPacketSize) {
        _prepareNextOutgoingDataPacket(UDP_PACKET_KIND_DATA);
        auto& tail = _getTailPacket();
        tail.packet.appendBytes(aData, aDataByteCount);
        HG_ASSERT(tail.packet.getDataSize() <= _maxPacketSize);
        return;
    }

    // At this point, we have to send a fragmented packet

    const auto packetCountBefore    = _packets.size();
    bool       reusedExistingPacket = false;

    // Prepare the current latest outgoing packet (finalize it if it's full enough, and
    // set its type to DATA_MORE otherwise):
    {
        auto& tail = _getTailPacket();

        // This is kind of an arbitrarily chosen limit, but if the latest outgoing packet is at
        // least 50% full, we'll send it independently to avoid dependencies between packets.
        if (tail.packet.getDataSize() > _maxPacketSize / 2) {
            _prepareNextOutgoingDataPacket(UDP_PACKET_KIND_DATA_MORE);
        } else {
            // Otherwise we must edit the type of the packet onto which we're going to start
            // appending the data to DATA_MORE, so that the recepient knows not to do anything
            // with it until the remaining fragments are also received and assembled.
            _changePacketKind(tail, UDP_PACKET_KIND_DATA_MORE);
            reusedExistingPacket = true;
        }
    }

    // Pack the data into multiple consecutive packets:
    PZInteger bytesPacked = 0;
    while (true) {
        auto& tail = _getTailPacket();

        HG_HARD_ASSERT(pztos(_maxPacketSize) >= tail.packet.getDataSize());

        const PZInteger remainingCapacity = _maxPacketSize - tail.packet.getDataSize();
        const PZInteger bytesToPackNow    = std::min(remainingCapacity, aDataByteCount - bytesPacked);

        tail.packet.appendBytes(static_cast<const char*>(aData.get()) + bytesPacked, bytesToPackNow);
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

    // This is just for verification: if we managed to 'piggyback' off a previously existing
    // DATA packet, we expect that at least 1 new packet was added. Otherwise, we expect that
    // at least 2 new packets were added (at least 1 FRAGMENT and 1 TAIL).
    const auto packetCountAfter = _packets.size();
    if (reusedExistingPacket) {
        HG_HARD_ASSERT(packetCountBefore + 1 <= packetCountAfter);
    } else {
        HG_HARD_ASSERT(packetCountBefore + 2 <= packetCountAfter);
    }

    // We don't want chaining of multiple fragmented packets, so finalize the tail and
    // start the next regular packet:
    _prepareNextOutgoingDataPacket(UDP_PACKET_KIND_DATA);
}

void UdpSendBuffer::appendAckForSending(PacketOrdinal aPacketOrdinal) {
    _acknowledges.push_back(aPacketOrdinal);
}

UdpSendBuffer::AckReceivedResult UdpSendBuffer::ackReceived(PacketOrdinal aPacketOrdinal,
                                                            bool          aIsStrong) {
    if (aPacketOrdinal < _headOrdinal) {
        return {{}, false}; // Already acknowledged before
    }

    const std::uint32_t indexInBuffer = (aPacketOrdinal - _headOrdinal);
    if (indexInBuffer >= _packets.size()) {
        HG_THROW_TRACED(InvalidDataError,
                        0,
                        "Received ACK for packet that's not yet sent ({}).",
                        aPacketOrdinal);
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
        return {{}, false};
    }

    const auto timeToAck = target.stopwatch.getElapsedTime<std::chrono::microseconds>();

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

    return {timeToAck, true};
}

std::vector<util::Packet> UdpSendBuffer::exportPackets() {
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

UdpSendBuffer::TaggedPacket& UdpSendBuffer::_getTailPacket() {
    HG_HARD_ASSERT(!_packets.empty());
    return _packets.back();
}

void UdpSendBuffer::_prepareNextOutgoingDataPacket(std::uint32_t aPacketType) {
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

void UdpSendBuffer::_changePacketKind(TaggedPacket& aTaggedPacket, std::uint32_t aNewKind) {
    const auto newKindInNetworkOrder =
        static_cast<std::uint32_t>(hton32(static_cast<std::int32_t>(aNewKind)));

    auto& packet = aTaggedPacket.packet;
    HG_HARD_ASSERT(packet.getDataSize() >= sizeof(newKindInNetworkOrder));

    // The first 4 bytes of a packet determine its kind
    auto* kindPtr = packet.getMutableData();

    std::memcpy(kindPtr, &newKindInNetworkOrder, sizeof(newKindInNetworkOrder));
}

} // namespace rn
HOBGOBLIN_NAMESPACE_END

#include <Hobgoblin/Private/Pmacro_undef.hpp>
