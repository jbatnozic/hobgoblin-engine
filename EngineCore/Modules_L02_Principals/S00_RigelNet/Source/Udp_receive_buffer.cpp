// Copyright 2024 Jovan Batnozic. Released under MS-PL licence in Serbia.
// See https://github.com/jbatnozic/Hobgoblin?tab=readme-ov-file#licence

#include "Udp_receive_buffer.hpp"
#include "Udp_connector_packet_kinds.hpp"

#include <Hobgoblin/HGExcept.hpp>
#include <Hobgoblin/Logging.hpp>

#include <string>

#include <Hobgoblin/Private/Pmacro_define.hpp>

HOBGOBLIN_NAMESPACE_BEGIN
namespace rn {

namespace {
constexpr auto LOG_ID = "Hobgoblin.RigelNet";
} // namespace

void UdpReceiveBuffer::reset() {
    _packets.clear();
    _headOrdinal = 1;
}

std::vector<PacketOrdinal> UdpReceiveBuffer::dataPacketReceived(util::Packet  aPacket,
                                                                PacketOrdinal aPacketOrdinal,
                                                                std::uint32_t aPacketKind) {
    if (aPacketOrdinal < _headOrdinal) {
        // Old data - ignore
        return {};
    }

    const std::size_t indexInBuffer = pztos(aPacketOrdinal - _headOrdinal);
    if (indexInBuffer >= _packets.size()) {
        _packets.resize(indexInBuffer + 1u);
    } else if (_packets[indexInBuffer].tag != TaggedPacket::WAITING_FOR_DATA) {
        // Already received - ignore
        return{};
    }

    std::vector<PacketOrdinal> acks;
    while (true) {
        const std::uint32_t ackOrdinal = aPacket.extract<PacketOrdinal>();
        if (ackOrdinal == 0u) {
            break;
        }
        acks.push_back(ackOrdinal);
    }

    _packets[indexInBuffer].packet = std::move(aPacket);

    if (aPacketKind == UDP_PACKET_KIND_DATA) {
        _packets[indexInBuffer].tag = TaggedPacket::READY_FOR_UNPACKING;
    } else if (aPacketKind == UDP_PACKET_KIND_DATA_MORE) {
        _packets[indexInBuffer].tag = TaggedPacket::FRAGMENT;
    } else if (aPacketKind == UDP_PACKET_KIND_DATA_TAIL) {
        _packets[indexInBuffer].tag = TaggedPacket::FRAGMENT_TAIL;
    } else {
        HG_THROW_TRACED(TracedRuntimeError, 0, "Invalid packet kind {}.", aPacketKind);
    }

    return acks;
}

bool UdpReceiveBuffer::takeNextReadyPacket(NeverNull<util::Packet*> aPacket) {
    while (!_packets.empty() && _packets[0].tag == TaggedPacket::UNPACKED) {
        _packets.pop_front();
        _headOrdinal += 1;
    }

    _tryToAssembleFragmentedPacketAtHead();

    if (_packets.empty() || _packets[0].tag != TaggedPacket::READY_FOR_UNPACKING) {
        return false;
    }

    *aPacket = std::move(_packets[0].packet);
    _packets.pop_front();
    _headOrdinal += 1;

    return true;
}

void UdpReceiveBuffer::_tryToAssembleFragmentedPacketAtHead() {
    if (_packets.empty() || _packets.front().tag != TaggedPacket::FRAGMENT) {
        return;
    }

    bool allFragmentsPresent = false;
    for (const auto& taggedPacket : _packets) {
        switch (taggedPacket.tag) {
        case TaggedPacket::WAITING_FOR_DATA:
            // Still waiting to receive fragments, we can quit right away
            return;

        case TaggedPacket::FRAGMENT:
            // All good, keep going
            break;

        case TaggedPacket::FRAGMENT_TAIL:
            // All fragments present!
            allFragmentsPresent = true;
            goto BREAK_FOR;

        default:
            // This isn't supposed to happen
            // throw RN_IllegalMessage{"Impossible to assemble fragmented packet."}; // TODO
            (void)0;
        }
    }
BREAK_FOR:

    if (!allFragmentsPresent) {
        return;
    }

    // Append all data to head packet, tag it ReadyForUnpacking, and other fragments as Unpacked:
    for (std::size_t i = 1;; i += 1) {
        TaggedPacket& curr = _packets[i];

        const auto currDataSize = curr.packet.getRemainingDataSize();
        _packets[0].packet.appendBytes(curr.packet.extractBytes(currDataSize), currDataSize);

        curr.packet.clear();

        if (curr.tag != TaggedPacket::FRAGMENT_TAIL) {
            curr.tag = TaggedPacket::UNPACKED;
        } else {
            curr.tag = TaggedPacket::UNPACKED;
            break;
        }
    }
    _packets[0].tag = TaggedPacket::READY_FOR_UNPACKING;
}

} // namespace rn
HOBGOBLIN_NAMESPACE_END

#include <Hobgoblin/Private/Pmacro_undef.hpp>
