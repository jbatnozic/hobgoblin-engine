
#include <Hobgoblin/RigelNet/Node.hpp>
#include <Hobgoblin/RigelNet/Udp_connector.hpp>

#include <cassert>
#include <iostream> // TODO Temp.

#include <Hobgoblin/Private/Pmacro_define.hpp>

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

    RETRY:
    switch (socket.send(packet, ip, port)) {
    case sf::Socket::Done:
        return UPLOAD_PACKET_SUCCESS;

    case sf::Socket::Partial:
        goto RETRY;

    case sf::Socket::NotReady:
        //std::cout << "up - NotReady\n";
        //assert(0);
        return UPLOAD_PACKET_SUCCESS; // TODO ?????

    case sf::Socket::Error:
    case sf::Socket::Disconnected:
        return UPLOAD_PACKET_FAILURE;
    }

    assert(0 && "Unreachable");
    return UPLOAD_PACKET_FAILURE;
}

} // namespace

RN_UdpConnector::RN_UdpConnector(sf::UdpSocket& socket, const std::string& passphrase)
    : _socket{socket}
    , _passphrase{passphrase}
    , _status{RN_ConnectorStatus::Disconnected}
{
}

void RN_UdpConnector::tryAccept(sf::IpAddress addr, std::uint16_t port, RN_Packet& packet) {
    assert(_status == RN_ConnectorStatus::Disconnected);

    std::uint32_t msgType;
    std::string receivedPassphrase;

    if ((packet >> msgType)
        && msgType == UDP_MESSAGE_TYPE_HELLO
        && (packet >> receivedPassphrase)
        && receivedPassphrase == _passphrase) {

        _remoteInfo = RN_RemoteInfo{addr, port};
        _status = RN_ConnectorStatus::Accepting;

        cleanUp();
        _sendBufferHeadIndex = 1u;
        _recvBufferHeadIndex = 1u;
        prepareNextOutgoingPacket();
    }
    else {
        // TODO - Notify of erroneous message from unknown sender...
    }
}

void RN_UdpConnector::connect(sf::IpAddress addr, std::uint16_t port) {
    assert(_status == RN_ConnectorStatus::Disconnected);

    _remoteInfo = RN_RemoteInfo{addr, port};
    _status = RN_ConnectorStatus::Connecting;

    cleanUp();
    _sendBufferHeadIndex = 1u;
    _recvBufferHeadIndex = 1u;
    prepareNextOutgoingPacket();
}

void RN_UdpConnector::disconnect(bool notfiyRemote) {
    if (notfiyRemote) {
        RN_Packet packet;
        packet << UDP_MESSAGE_TYPE_DISCONNECT;
        if (UploadPacket(_socket, packet, _remoteInfo.ipAddress, _remoteInfo.port) != UPLOAD_PACKET_SUCCESS) {
            // TODO
        }
    }
    reset();
}

void RN_UdpConnector::update(RN_Node& node, PZInteger slotIndex, bool doUpload) {
    if (_status == RN_ConnectorStatus::Disconnected) {
        return;
    }

    if (connectionTimedOut()) {
        reset();
        //node->queue_event(EventFactory::create_conn_timeout(slot_index));
        return;
    }

    if (doUpload) {
        switch (_status) {
        case RN_ConnectorStatus::Accepting:  // Send CONNECT messages to the client, until a DATA message is received     
        case RN_ConnectorStatus::Connecting: // Send HELLO messages to the server, until a CONNECT message is received
            {
                RN_Packet packet;
                packet << ((_status == RN_ConnectorStatus::Accepting) ? UDP_MESSAGE_TYPE_CONNECT 
                                                                      : UDP_MESSAGE_TYPE_HELLO);
                packet << _passphrase;
                if (UploadPacket(_socket, packet, _remoteInfo.ipAddress, _remoteInfo.port) != UPLOAD_PACKET_SUCCESS) {
                    reset();
                    // TODO Maybe log event?
                }
            }
            break;

        case RN_ConnectorStatus::Connected:
            uploadAllData();
            break;

        default:
            assert(0 && "Unreachable");
            break;
        }
    }
}

void RN_UdpConnector::receivedPacket(RN_Packet& packet) {
    assert(_status != RN_ConnectorStatus::Disconnected);

    const auto msgType = packet.extractValue<std::uint32_t>();
    if (!packet) {
        // TODO - Handle error
    }

    switch (_status) {
    case RN_ConnectorStatus::Connecting:
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
                reset();
                // node->queue_event(EventFactory::create_bad_passphrase(temp)); TODO
            }
        }
        else {
            reset();
            // node->queue_event(EventFactory::create_disconnect()); TODO
        }
        break;

    case RN_ConnectorStatus::Accepting:
    case RN_ConnectorStatus::Connected:
        if (msgType == UDP_MESSAGE_TYPE_HELLO || msgType == UDP_MESSAGE_TYPE_CONNECT) {
            // Ignore
        }
        else if (msgType == UDP_MESSAGE_TYPE_DATA) {
            if (_status == RN_ConnectorStatus::Accepting) {
                initializeSession(); // New connection confirmed
                // node->queue_event(EventFactory::create_connect(slot_index)); TODO
            }
            // handle_data(pack, node, type); TODO
            receiveDataMessage(packet);
        }
        else { // Otherwise, disconnect
            reset();
            // node->queue_event(EventFactory::create_disconnect(slot_index)); TODO
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

RN_ConnectorStatus RN_UdpConnector::getStatus() const noexcept {
    return _status;
}

void RN_UdpConnector::appendToNextOutgoingPacket(const void *data, std::size_t sizeInBytes) {
    // TODO temp.
    _sendBuffer[_sendBuffer.size() - 1u].packet.append(data, sizeInBytes);
}

// Private

void RN_UdpConnector::cleanUp() {
    _sendBuffer.clear();
    _recvBuffer.clear();
    _ackOrdinals.clear();
}

void RN_UdpConnector::reset() {
    cleanUp();
    _remoteInfo = RN_RemoteInfo{};
    _status = RN_ConnectorStatus::Disconnected;
}

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
    _ackOrdinals.push_back(ordinal);
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
    _status = RN_ConnectorStatus::Connected;
    _remoteInfo.timeoutStopwatch.restart();
}

void RN_UdpConnector::prepareNextOutgoingPacket() {
    _sendBuffer.emplace_back();
    _sendBuffer.back().tag = TaggedPacket::ReadyForSending;

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
    else if (_recvBuffer[indexInBuffer].tag != TaggedPacket::WaitingForData) {
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

    prepareAck(messageOrdinal);
}

} // namespace detail
} // namespace rn
HOBGOBLIN_NAMESPACE_END

#include <Hobgoblin/Private/Pmacro_undef.hpp>
