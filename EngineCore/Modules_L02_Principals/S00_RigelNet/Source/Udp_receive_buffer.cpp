// Copyright 2024 Jovan Batnozic. Released under MS-PL licence in Serbia.
// See https://github.com/jbatnozic/Hobgoblin?tab=readme-ov-file#licence

#include "Udp_receive_buffer.hpp"
#include "Invalid_data_error.hpp"
#include "Udp_connector_packet_kinds.hpp"

#include <Hobgoblin/HGExcept.hpp>
#include <Hobgoblin/Logging.hpp>
#include <Hobgoblin/RigelNet/Handlermgmt.hpp>

#include <Hobgoblin/Private/Pmacro_define.hpp>

HOBGOBLIN_NAMESPACE_BEGIN
namespace rn {

namespace {
constexpr auto LOG_ID = "Hobgoblin.RigelNet";
} // namespace

PZInteger UdpReceiveBuffer::getLength() const {
    return stopz(_packets.size());
}

void UdpReceiveBuffer::reset() {
    _packets.clear();
    _headOrdinal = 1;
}

std::vector<PacketOrdinal> UdpReceiveBuffer::storeDataPacket(util::Packet  aPacket,
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
        return {};
    }

    std::vector<PacketOrdinal> acks;
    while (!aPacket.endOfPacket()) {
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
        HG_THROW_TRACED(InvalidDataError, 0, "Invalid packet kind {}.", aPacketKind);
    }

    return acks;
}

bool UdpReceiveBuffer::takeNextReadyPacket(NeverNull<util::Packet*> aPacket) {
    while (!_packets.empty()) {
        switch (const auto tag = _packets[0].tag) {
        case TaggedPacket::WAITING_FOR_DATA:
            return false;

        case TaggedPacket::FRAGMENT:
            goto BREAK_WHILE;

        case TaggedPacket::FRAGMENT_TAIL:
            HG_THROW_TRACED(InvalidDataError, 0, "Unecpexted FRAGMENT_TAIL at head of receive buffer.");

        case TaggedPacket::READY_FOR_UNPACKING:
            goto BREAK_WHILE;

        case TaggedPacket::UNPACKED:
            _packets.pop_front();
            _headOrdinal += 1;
            break;

        default:
            HG_UNREACHABLE("Invalid value for TaggedPacket::Tag ({}).", (int)tag);
        }
    }
BREAK_WHILE:

    _tryToAssembleFragmentedPacketAtHead();

    if (_packets.empty() || _packets[0].tag != TaggedPacket::READY_FOR_UNPACKING) {
        return false;
    }

    // It spams too much; uncomment if you need to debug.
    // HG_LOG_INFO(LOG_ID,
    //             "Packet {} taken for handling ({} bytes total, {} remaining).",
    //             _headOrdinal,
    //             _packets[0].packet.getDataSize(),
    //             _packets[0].packet.getRemainingDataSize());

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
            HG_THROW_TRACED(InvalidDataError, 0, "Impossible to assemble fragmented packet.");
        }
    }
BREAK_FOR:

    if (!allFragmentsPresent) {
        return;
    }

    // Append all data to head packet, tag it ReadyForUnpacking, and other fragments as Unpacked:
    for (std::size_t i = 1;; i += 1) {
        TaggedPacket& curr = _packets[i];

        // Note: some leading bytes have been read previously (packet kind and acks),
        //       the rest are untouched.
        const auto remainingBytes = curr.packet.getRemainingDataSize();
        const auto bytesWritten =
            _packets[0].packet.write(curr.packet.readInPlace(remainingBytes), remainingBytes);
        HG_ASSERT(bytesWritten == remainingBytes);

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
