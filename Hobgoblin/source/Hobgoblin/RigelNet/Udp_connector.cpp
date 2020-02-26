
#include <Hobgoblin/RigelNet/Node.hpp>
#include <Hobgoblin/RigelNet/Udp_connector.hpp>

#include <cassert>
#include <chrono>
#include <iostream> // TODO Temp.

#include <Hobgoblin/Private/Pmacro_define.hpp>

HOBGOBLIN_NAMESPACE_START
namespace rn {
namespace detail {

namespace {

constexpr std::uint32_t UDP_PACKET_TYPE_HELLO      = 0x3BF0E110;
constexpr std::uint32_t UDP_PACKET_TYPE_CONNECT    = 0x83C96CA4;
constexpr std::uint32_t UDP_PACKET_TYPE_DISCONNECT = 0xD0F235AB;
constexpr std::uint32_t UDP_PACKET_TYPE_DATA       = 0xA765B8F6;
constexpr std::uint32_t UDP_PACKET_TYPE_ACKS       = 0x71AC2519;

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

bool ShouldRetransmit(std::chrono::microseconds timeSinceLastSend, std::chrono::microseconds currentLatency) {
    //return timeSinceLastSend > 2 * currentLatency;
    return false; // TODO
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
        && msgType == UDP_PACKET_TYPE_HELLO
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
    // TODO Assert correct state
    if (notfiyRemote) {
        RN_Packet packet;
        packet << UDP_PACKET_TYPE_DISCONNECT;
        if (UploadPacket(_socket, packet, _remoteInfo.ipAddress, _remoteInfo.port) != UPLOAD_PACKET_SUCCESS) {
            // TODO
        }
    }
    reset();
}

void RN_UdpConnector::checkForTimeout() {
    assert(_status != RN_ConnectorStatus::Disconnected);
    if (connectionTimedOut()) {
        reset();
        //node->queue_event(EventFactory::create_conn_timeout(slot_index));
        return;
    }
}

void RN_UdpConnector::send(RN_Node& node) {
    assert(_status != RN_ConnectorStatus::Disconnected);

    switch (_status) {
    case RN_ConnectorStatus::Accepting:  // Send CONNECT messages to the client, until a DATA message is received     
    case RN_ConnectorStatus::Connecting: // Send HELLO messages to the server, until a CONNECT message is received
        {
            RN_Packet packet;
            packet << ((_status == RN_ConnectorStatus::Accepting) ? UDP_PACKET_TYPE_CONNECT 
                                                                    : UDP_PACKET_TYPE_HELLO);
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

void RN_UdpConnector::receivedPacket(RN_Packet& packet) {
    assert(_status != RN_ConnectorStatus::Disconnected);

    const auto packetType = packet.extractValue<std::uint32_t>();
    if (!packet) {
        // TODO - Handle error
    }

    switch (packetType) {
    case UDP_PACKET_TYPE_HELLO:
        processHelloPacket(packet);
        break;

    case UDP_PACKET_TYPE_CONNECT:
        processConnectPacket(packet);
        break;

    case UDP_PACKET_TYPE_DISCONNECT:
        processDisconnectPacket(packet);
        break;

    case UDP_PACKET_TYPE_DATA:
        processDataPacket(packet);
        break;

    case UDP_PACKET_TYPE_ACKS:
        processAcksPacket(packet);
        break;

    default:
        // TODO erroneous, fatal
        reset();
        break;
    }
}

void RN_UdpConnector::sendAcks() {
    // TODO Assert correct state

    if (_ackOrdinals.empty()) {
        return;
    }

    RN_Packet packet;
    packet << UDP_PACKET_TYPE_ACKS;
    for (std::uint32_t ackOrdinal : _ackOrdinals) {
        packet << ackOrdinal;
    }

    _ackOrdinals.clear();

    if (UploadPacket(_socket, packet, _remoteInfo.ipAddress, _remoteInfo.port) != UPLOAD_PACKET_SUCCESS) {
        reset();
        // TODO Maybe log event?
    }
}

void RN_UdpConnector::handleDataMessages(RN_Node& node) {
    // TODO Assert correct state

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

PZInteger RN_UdpConnector::getSendBufferSize() const {
    return static_cast<PZInteger>(_sendBuffer.size());
}

PZInteger RN_UdpConnector::getRecvBufferSize() const {
    return static_cast<PZInteger>(_recvBuffer.size());
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
            || ShouldRetransmit(taggedPacket.stopwatch.getElapsedTime(), _remoteInfo.latency)) {

            const int* pOrd = static_cast<const int*>(taggedPacket.packet.getData());
            RN_Packet dummyPacket;
            dummyPacket.append(pOrd + 1, sizeof(int));
            int ord = dummyPacket.extractValue<int>();
            std::cout << "SEND: " << ord << '\n';

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
    std::cout << "ACK: " << ordinal << '\n';

    if (ordinal < _sendBufferHeadIndex) {
        return; // Already acknowledged before
    }

    const auto ind = (ordinal - _sendBufferHeadIndex); // TODO PEP
    if (ind >= _sendBuffer.size()) {
        // TODO -- Error
    }

    // TODO Temp - Time between send & ack = latency
    // Temp because it should average out all the values or something
    _remoteInfo.latency = _sendBuffer[ind].stopwatch.getElapsedTime<std::chrono::microseconds>();
    _remoteInfo.timeoutStopwatch.restart();

    _sendBuffer[ind].tag = TaggedPacket::Acknowledged;
    _sendBuffer[ind].packet.clear();

    if (ind == 0) {
        while (!_sendBuffer.empty() && _sendBuffer[0].tag == TaggedPacket::Acknowledged) {
            _sendBuffer.pop_front();
            _sendBufferHeadIndex += 1;
        }
    }
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
    packet << UDP_PACKET_TYPE_DATA;
    // Message ordinal:
    packet << static_cast<std::uint32_t>(_sendBuffer.size() + _sendBufferHeadIndex - 1u);
    // Acknowledges (zero-terminated):
    for (std::uint32_t ackOrdinal : _ackOrdinals) {
        packet << ackOrdinal;
    }
    packet << static_cast<std::uint32_t>(0);
    _ackOrdinals.clear(); // TODO Pep
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


void RN_UdpConnector::processHelloPacket(RN_Packet& packet) {
    switch (_status) {
    case RN_ConnectorStatus::Connecting:
        // TODO erroneous, fatal
        break;

    case RN_ConnectorStatus::Accepting:
        NO_OP();
        break;

    case RN_ConnectorStatus::Connected:
        // TODO erroneous, nonfatal
        break;

    default:
        assert(0 && "Unreachable");
        break;
    }
}

void RN_UdpConnector::processConnectPacket(RN_Packet& packet) {
    switch (_status) {
    case RN_ConnectorStatus::Connecting:
        {
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
        break;

    case RN_ConnectorStatus::Accepting:
        // TODO erroneous, fatal
        break;

    case RN_ConnectorStatus::Connected:
        // TODO erroneous, nonfatal
        break;

    default:
        assert(0 && "Unreachable");
        break;
    }
}

void RN_UdpConnector::processDisconnectPacket(RN_Packet& packet) {
    switch (_status) {
    case RN_ConnectorStatus::Connecting:
    case RN_ConnectorStatus::Accepting:
    case RN_ConnectorStatus::Connected:
        // TODO - Extract reason
        reset();
        break;

    default:
        assert(0 && "Unreachable");
        break;
    }
}

void RN_UdpConnector::processDataPacket(RN_Packet& packet) {
    switch (_status) {
    case RN_ConnectorStatus::Connecting:
        // TODO erroneous, fatal
        break;

    case RN_ConnectorStatus::Accepting:
        initializeSession(); // New connection confirmed
        SWITCH_FALLTHROUGH;

    case RN_ConnectorStatus::Connected:
        receiveDataMessage(packet);
        break;

    default:
        assert(0 && "Unreachable");
        break;
    }
}

void RN_UdpConnector::processAcksPacket(RN_Packet& packet) {
    switch (_status) {
    case RN_ConnectorStatus::Connecting:
    case RN_ConnectorStatus::Accepting:
        // TODO erroneous, fatal
        break;

    case RN_ConnectorStatus::Connected:
        while (!packet.endOfPacket()) {
            const std::uint32_t ackOrd = packet.extractValue<std::uint32_t>();
            if (!packet) {
                // TODO Handle error
            }
            receivedAck(ackOrd);
        }
        break;

    default:
        assert(0 && "Unreachable");
        break;
    }
}

} // namespace detail
} // namespace rn
HOBGOBLIN_NAMESPACE_END

#include <Hobgoblin/Private/Pmacro_undef.hpp>
