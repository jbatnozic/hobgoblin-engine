
#include <Hobgoblin/RigelNet/node.hpp>
#include <Hobgoblin/RigelNet/udp_connector.hpp>

#include <cassert>
#include <iostream> // TODO Temp.

#include <Hobgoblin/Private/pmacro_define.hpp>

HOBGOBLIN_NAMESPACE_START
namespace rn {
namespace detail {

namespace {

constexpr std::uint32_t UDP_MESSAGE_TYPE_HELLO      = 0x3BF0E110;
constexpr std::uint32_t UDP_MESSAGE_TYPE_CONNECT    = 0x83C96CA4;
constexpr std::uint32_t UDP_MESSAGE_TYPE_DISCONNECT = 0xD0F235AB;
constexpr std::uint32_t UDP_MESSAGE_TYPE_DATA       = 0xA765B8F6;

constexpr bool UPLOAD_PACKET_SUCCESS = true;
constexpr bool UPLOAD_PACKET_FAILURE = false;

bool UploadPacket(sf::UdpSocket& socket, RN_Packet& packet, sf::IpAddress ip, std::uint16_t port) {
    if (packet.getDataSize() == 0u) return UPLOAD_PACKET_SUCCESS;

    while (true) {
        switch (socket.send(packet, ip, port)) {

        case sf::Socket::Done:
            //std::cout << "up - Done\n";
            return UPLOAD_PACKET_SUCCESS;
            break;

        case sf::Socket::Partial:
            continue;

        case sf::Socket::NotReady:
            //std::cout << "up - NotReady\n";
            //assert(0);
            return UPLOAD_PACKET_SUCCESS;
            break;

        case sf::Socket::Error:
            //std::cout << "up - Error\n";
            //assert(0);
            return UPLOAD_PACKET_FAILURE;
            break;

        case sf::Socket::Disconnected:
            //std::cout << "up - Disconnected\n";
            assert(0);
            break;

        }
    }

    assert(0);
    return UPLOAD_PACKET_FAILURE;
}

} // namespace

RN_UdpConnector::RN_UdpConnector(sf::UdpSocket& socket, const std::string& passphrase)
    : _socket{socket}
    , _passphrase{passphrase}
    , _state{State::Disconnected}
{
}

void RN_UdpConnector::tryAccept(sf::IpAddress addr, std::uint16_t port, RN_Packet& packet) {
    assert(_state == State::Disconnected);

    std::uint32_t msgType;
    std::string receivedPassphrase;

    if ((packet >> msgType)
        && msgType == UDP_MESSAGE_TYPE_HELLO
        && (packet >> receivedPassphrase)
        && receivedPassphrase == _passphrase) {

        _remoteInfo.ipAddress = addr;
        _remoteInfo.port = port;
        _remoteInfo.timeoutStopwatch.restart();

        _state = State::Accepting;
        _remoteInfo.status = RN_RemoteStatus::Connected; // TODO - Connecting...

        _sendBuffer.clear(); // TODO Temp.
        _sendBufferHeadIndex = 1u;
        _recvBufferHeadIndex = 1u;
        _recvBuffer.clear();
        prepareNextOutgoingPacket();
    }
    else {
        // TODO - Notify of erroneous message from unknown sender...
    }
}

void RN_UdpConnector::connect(sf::IpAddress addr, std::uint16_t port) {
    assert(_state == State::Disconnected);

    // reset(interval_, timeout_ms_); TODO ???

    _remoteInfo.ipAddress = addr;
    _remoteInfo.port = port;
    _remoteInfo.timeoutStopwatch.restart();

    _state = State::Connecting;
    _remoteInfo.status = RN_RemoteStatus::Connected; // TODO - Connecting...

    _sendBuffer.clear(); // TODO Temp.
    _sendBufferHeadIndex = 1u;
    _recvBufferHeadIndex = 1u;
    _recvBuffer.clear();
    prepareNextOutgoingPacket();
}

void RN_UdpConnector::reset() {
    // TODO
}

void RN_UdpConnector::update(RN_Node& node, PZInteger slotIndex, bool doUpload) {
    if (_state == State::Disconnected) {
        return;
    }

    if (connectionTimedOut()) {
        //reset();
        //node->queue_event(EventFactory::create_conn_timeout(slot_index));
        return;
    }

    if (doUpload) {
        switch (_state) {
        case State::Accepting:  // Send CONNECT messages to the client, until a DATA message is received     
        case State::Connecting: // Send HELLO messages to the server, until a CONNECT message is received
            {
                RN_Packet packet;
                packet << ((_state == State::Accepting) ? UDP_MESSAGE_TYPE_CONNECT : UDP_MESSAGE_TYPE_HELLO);
                packet << _passphrase;
                if (UploadPacket(_socket, packet, _remoteInfo.ipAddress, _remoteInfo.port) != UPLOAD_PACKET_SUCCESS) {
                    // TODO Disconnect
                    // reset();
                }
            }
            break;

        case State::Connected:
            uploadAllData();
            break;

        default:
            assert(0 && "Unreachable");
            break;
        }
    }
}

void RN_UdpConnector::receivedPacket(RN_Packet& packet) {
    assert(_state != State::Disconnected);

    const auto msgType = packet.extractValue<std::uint32_t>();
    if (!packet) {
        // TODO - Handle error
    }

    switch (_state) {
    case State::Connecting:
        if (msgType == UDP_MESSAGE_TYPE_CONNECT) {
            const std::string receivedPassphrase = packet.extractValue<std::string>();
            if (!packet) {
                // TODO - Handle error
            }

            if (receivedPassphrase == _passphrase) {
                // Client connected to server
                initializeSession();
                // node->queue_event(EventFactory::create_connect()); TODO
            }
            else {
                // disconnect(false); TODO
                // node->queue_event(EventFactory::create_bad_passphrase(temp)); TODO
            }
        }
        else {
            // disconnect(false); TODO
            // node->queue_event(EventFactory::create_disconnect()); TODO
        }
        break;

    case State::Accepting:
    case State::Connected:
        if (msgType == UDP_MESSAGE_TYPE_HELLO || msgType == UDP_MESSAGE_TYPE_CONNECT) {
            // Ignore
        }
        else if (msgType == UDP_MESSAGE_TYPE_DATA) {
            if (_state == State::Accepting) {
                initializeSession(); // New connection confirmed
                // node->queue_event(EventFactory::create_connect(slot_index)); TODO
            }
            // handle_data(pack, node, type); TODO
            receiveDataMessage(packet);
        }
        else { // Otherwise, disconnect
            // TODO - Handle error
            // disconnect(false);
            // node->queue_event(EventFactory::create_disconnect(slot_index));
        }
        break;

    default:
        assert(0 && "Unreachable");
        break;

    }
}

void RN_UdpConnector::handleDataMessages(RN_Node& node) {
    while (!_recvBuffer.empty() && _recvBuffer[0].tag == TaggedPacket::ReadyForUnpacking) {
        detail::HandleDataMessages(node, _recvBuffer[0].packet);
        _recvBuffer.pop_front();
        _recvBufferHeadIndex++;
    }
}

const RN_RemoteInfo& RN_UdpConnector::getRemoteInfo() const noexcept {
    return _remoteInfo;
}

void RN_UdpConnector::appendToNextOutgoingPacket(const void *data, std::size_t sizeInBytes) {
    // TODO temp.
    _sendBuffer[_sendBuffer.size() - 1u].packet.append(data, sizeInBytes);
}

// Private

bool RN_UdpConnector::connectionTimedOut() const {
    if (_timeoutLimit <= std::chrono::microseconds{0}) {
        return false;
    }
    if (_remoteInfo.timeoutStopwatch.getElapsedTime() >= _timeoutLimit) {
        return true;
    }
    return false;
}

void RN_UdpConnector::uploadAllData() {
    /*
    if (item.clock.getElapsedTime().asMilliseconds() > std::min(latency.asMilliseconds() * 2, 400)) { // STUB -- Could be better
        // min{2 * latency, 4 * frame_duration * interval}
        //std::cout << "Retransmit after " << item.clock.getElapsedTime().asMilliseconds() << "ms \n";
        }
    else
        continue; // Too early
    */

    PZInteger uploadCounter = 0;
    for (auto& taggedPacket : _sendBuffer) {
        if (taggedPacket.tag == TaggedPacket::Acknowledged) {
            continue;
        }

        // TODO Send, restart clock etc.
        if ((taggedPacket.tag == TaggedPacket::ReadyForSending)
            || (false /*timed out*/)) {

            if (UploadPacket(
                _socket,
                taggedPacket.packet,
                _remoteInfo.ipAddress,
                _remoteInfo.port) != UPLOAD_PACKET_SUCCESS) {

                // TODO Handle error
            }

            taggedPacket.stopwatch.restart();
            uploadCounter += 1;
            // TODO Break if uploadCounter too large [configurable]
        }

        taggedPacket.tag = TaggedPacket::NotAcknowledged;
    }

    prepareNextOutgoingPacket();
}

void RN_UdpConnector::prepareAck(std::uint32_t ordinal) {

}

void RN_UdpConnector::receivedAck(std::uint32_t ordinal) {
    if (ordinal < _sendBufferHeadIndex) {
        return; // Already acknowledged before
    }

    const auto ind = (ordinal - _sendBufferHeadIndex); // TODO PEP
    if (ind >= _sendBuffer.size()) {
        // TODO -- Error
    }

    _sendBuffer[ind].tag = TaggedPacket::Acknowledged;
    _sendBuffer[ind].packet.clear();
}

void RN_UdpConnector::initializeSession() {
    std::cout << "Session Initialized\n";
    _state = State::Connected;
    _remoteInfo.timeoutStopwatch.restart();
    // TODO Something for message buffers
}

void RN_UdpConnector::prepareNextOutgoingPacket() {
    _sendBuffer.emplace_back();
    _sendBuffer.back().tag = TaggedPacket::ReadyForSending;
    // _sendBuffer.back().clock = ??? TODO

    RN_Packet& packet = _sendBuffer.back().packet;
    // Message type:
    packet << UDP_MESSAGE_TYPE_DATA;
    // Message ordinal:
    packet << static_cast<std::uint32_t>(_sendBuffer.size() + _sendBufferHeadIndex - 1u);
    // Acknowledges (zero-terminated):
    for (std::uint32_t ackOrdinal : _ackOrdinals) {
        packet << ackOrdinal;
    }
    packet << static_cast<std::uint32_t>(0);
}

void RN_UdpConnector::receiveDataMessage(RN_Packet& packet) {
    const std::uint32_t messageOrdinal = packet.extractValue<std::uint32_t>();
    if (!packet) {
        // TODO
    }

    if (messageOrdinal < _recvBufferHeadIndex) {
        // Old data - acknowledge and ignore
        prepareAck(messageOrdinal);
        return;
    }

    const std::uint32_t indexInBuffer = (messageOrdinal - _recvBufferHeadIndex);
    if (indexInBuffer >= _recvBuffer.size()) {
        _recvBuffer.resize(indexInBuffer + 1u);
    }

    if (_recvBuffer[indexInBuffer].tag != TaggedPacket::WaitingForData) {
        // Already received this - acknowledge and ignore
        prepareAck(messageOrdinal);
        return;
    }

    while (true) {
        const std::uint32_t ackOrdinal = packet.extractValue<std::uint32_t>();
        if (!packet) {
            // Error -- STUB
        }

        if (ackOrdinal == 0u) {
            break;
        }
        
        receivedAck(ackOrdinal);
    }

    _recvBuffer[indexInBuffer].packet = std::move(packet);
    _recvBuffer[indexInBuffer].tag = TaggedPacket::ReadyForUnpacking;

    // HandleDataMessages(node, packet); TODO - Move after ALL handleDataMessage calls

    prepareAck(messageOrdinal);
}

} // namespace detail
} // namespace rn
HOBGOBLIN_NAMESPACE_END

#include <Hobgoblin/Private/pmacro_undef.hpp>