
#include <Hobgoblin/RigelNet/Node.hpp>
#include <Hobgoblin/RigelNet/Udp_connector.hpp>

#include <cassert>
#include <chrono>
#include <stdexcept>

#include <Hobgoblin/Private/Pmacro_define.hpp>

HOBGOBLIN_NAMESPACE_START
namespace rn {
namespace detail {

namespace {

constexpr std::uint32_t UDP_PACKET_TYPE_HELLO = 0x3BF0E110;
constexpr std::uint32_t UDP_PACKET_TYPE_CONNECT = 0x83C96CA4;
constexpr std::uint32_t UDP_PACKET_TYPE_DISCONNECT = 0xD0F235AB;
constexpr std::uint32_t UDP_PACKET_TYPE_DATA = 0xA765B8F6;
constexpr std::uint32_t UDP_PACKET_TYPE_ACKS = 0x71AC2519;

constexpr bool UPLOAD_PACKET_SUCCESS = true;
constexpr bool UPLOAD_PACKET_FAILURE = false;

bool UploadPacket(sf::UdpSocket& socket, util::Packet& packet, sf::IpAddress ip, std::uint16_t port) {                 
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
    return timeSinceLastSend > 2 * currentLatency; // TODO Test and optimize
}

class FatalMessageTypeReceived : public std::runtime_error {
public:
    using std::runtime_error::runtime_error;
};

} // namespace

RN_UdpConnector::RN_UdpConnector(sf::UdpSocket& socket, const std::chrono::microseconds& timeoutLimit, 
                                 const std::string& passphrase, EventFactory eventFactory)
    : _eventFactory{eventFactory}
    , _socket{socket}
    , _timeoutLimit{timeoutLimit}
    , _passphrase{passphrase}
    , _status{RN_ConnectorStatus::Disconnected}
{
}

bool RN_UdpConnector::tryAccept(sf::IpAddress addr, std::uint16_t port, RN_PacketWrapper& packetWrap) {
    assert(_status == RN_ConnectorStatus::Disconnected);

    const std::uint32_t msgType = packetWrap.extract<std::uint32_t>();
    const std::string receivedPassphrase = packetWrap.extract<std::string>();
    if (!packetWrap.packet) {
        // TODO Notify of error
        return false;
    }

    if (msgType == UDP_PACKET_TYPE_HELLO
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
        return false;
    }

    return true;
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
    assert(_status != RN_ConnectorStatus::Disconnected);

    if (notfiyRemote && _status == RN_ConnectorStatus::Connected) {
        util::Packet packet;
        packet << UDP_PACKET_TYPE_DISCONNECT;
        if (UploadPacket(_socket, packet, _remoteInfo.ipAddress, _remoteInfo.port) != UPLOAD_PACKET_SUCCESS) {
            // Do nothing - The connector is getting disconnected anyway...
        }
    }
    reset();
}

void RN_UdpConnector::checkForTimeout() {
    assert(_status != RN_ConnectorStatus::Disconnected);

    if (isConnectionTimedOut()) {
        reset();

        if (_status == RN_ConnectorStatus::Accepting || _status == RN_ConnectorStatus::Connecting) {
            _eventFactory.createConnectAttemptFailed(RN_Event::ConnectAttemptFailed::Reason::TimedOut);
        }
        else {
            _eventFactory.createDisconnected(RN_Event::Disconnected::Reason::TimedOut, "Connection timed out");
        }

        return;
    }
}

void RN_UdpConnector::send(RN_Node& node) {
    assert(_status != RN_ConnectorStatus::Disconnected);

    switch (_status) {
    case RN_ConnectorStatus::Accepting:  // Send CONNECT messages to the client, until a DATA message is received  
    {
        util::Packet packet;
        packet << UDP_PACKET_TYPE_CONNECT << _passphrase << _clientIndex.value();
        if (UploadPacket(_socket, packet, _remoteInfo.ipAddress, _remoteInfo.port) != UPLOAD_PACKET_SUCCESS) {
            reset();
            _eventFactory.createConnectAttemptFailed(RN_Event::ConnectAttemptFailed::Reason::Error);
        }
    }
    break;

    case RN_ConnectorStatus::Connecting: // Send HELLO messages to the server, until a CONNECT message is received
    {
        util::Packet packet;
        packet << UDP_PACKET_TYPE_HELLO << _passphrase;
        if (UploadPacket(_socket, packet, _remoteInfo.ipAddress, _remoteInfo.port) != UPLOAD_PACKET_SUCCESS) {
            reset();
            _eventFactory.createConnectAttemptFailed(RN_Event::ConnectAttemptFailed::Reason::Error);
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

void RN_UdpConnector::receivedPacket(RN_PacketWrapper& packetWrap) {
    assert(_status != RN_ConnectorStatus::Disconnected);

    try {
        const auto packetType = packetWrap.extractOrThrow<std::uint32_t>();

        switch (packetType) {
        case UDP_PACKET_TYPE_HELLO:
            processHelloPacket(packetWrap);
            break;

        case UDP_PACKET_TYPE_CONNECT:
            processConnectPacket(packetWrap);
            break;

        case UDP_PACKET_TYPE_DISCONNECT:
            processDisconnectPacket(packetWrap);
            break;

        case UDP_PACKET_TYPE_DATA:
            processDataPacket(packetWrap);
            break;

        case UDP_PACKET_TYPE_ACKS:
            processAcksPacket(packetWrap);
            break;

        default:
            throw FatalMessageTypeReceived{"Received UNKNOWN message type"};
            break;
        }
    }
    catch (FatalMessageTypeReceived& ex) {
        reset();
        if (_status == RN_ConnectorStatus::Connected) {
            _eventFactory.createDisconnected(RN_Event::Disconnected::Reason::Error, ex.what());
        }
        else {
            _eventFactory.createConnectAttemptFailed(RN_Event::ConnectAttemptFailed::Reason::Error);
        }
    }
    //catch (RN_PacketReadError & ex) {
    //    // TODO
    //}
}

void RN_UdpConnector::sendAcks() {
    assert(_status == RN_ConnectorStatus::Connected);

    if (_ackOrdinals.empty()) {
        return;
    }

    util::Packet packet;
    packet << UDP_PACKET_TYPE_ACKS;
    for (std::uint32_t ackOrdinal : _ackOrdinals) {
        packet << ackOrdinal;
    }

    if (UploadPacket(_socket, packet, _remoteInfo.ipAddress, _remoteInfo.port) != UPLOAD_PACKET_SUCCESS) {
        reset();
        _eventFactory.createConnectAttemptFailed(RN_Event::ConnectAttemptFailed::Reason::Error);
    }
}

void RN_UdpConnector::handleDataMessages(RN_Node& node) {
    assert(_status != RN_ConnectorStatus::Disconnected);

    while (!_recvBuffer.empty() && _recvBuffer[0].tag == TaggedPacket::ReadyForUnpacking) {
        // TODO Handle nonexistant handler and packet extraction errors (if the user's handler throws... oh, well...)
        detail::HandleDataMessages(node, _recvBuffer[0].packetWrap); 
        _recvBuffer.pop_front();
        _recvBufferHeadIndex++;
    }
}

void RN_UdpConnector::setClientIndex(std::optional<PZInteger> clientIndex) {
    _clientIndex = clientIndex;
}

std::optional<PZInteger> RN_UdpConnector::getClientIndex() const {
    return _clientIndex;
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
    // TODO temp. -- seems to be working pretty well!
    _sendBuffer[_sendBuffer.size() - 1u].packetWrap.packet.append(data, sizeInBytes);
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
    _clientIndex.reset();
}

bool RN_UdpConnector::isConnectionTimedOut() const {
    if (_timeoutLimit <= std::chrono::microseconds{0}) {
        return false;
    }
    if (_remoteInfo.timeoutStopwatch.getElapsedTime() >= _timeoutLimit) {
        return true;
    }
    return false;
}

void RN_UdpConnector::uploadAllData() {
    // TODO - Error handling here
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
        if (taggedPacket.tag == TaggedPacket::AcknowledgedWeakly ||
            taggedPacket.tag == TaggedPacket::AcknowledgedStrongly) {
            continue;
        }

        // TODO Send, restart clock etc.
        if ((taggedPacket.tag == TaggedPacket::ReadyForSending)
            || ShouldRetransmit(taggedPacket.stopwatch.getElapsedTime(), _remoteInfo.latency)) {

            if (UploadPacket(
                _socket,
                taggedPacket.packetWrap.packet,
                _remoteInfo.ipAddress,
                _remoteInfo.port) != UPLOAD_PACKET_SUCCESS) {

                // TODO Handle error, log event
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

void RN_UdpConnector::receivedAck(std::uint32_t ordinal, bool strong) {
    if (ordinal < _sendBufferHeadIndex) {
        return; // Already acknowledged before
    }

    const auto ind = (ordinal - _sendBufferHeadIndex);
    if (ind >= _sendBuffer.size()) {
        // TODO -- Error
    }

    if (!strong) {
        _sendBuffer[ind].tag = TaggedPacket::AcknowledgedWeakly;
        _sendBuffer[ind].packetWrap.packet.clear();
    } 
    else {
        // TODO Temp - Time between send & ack = latency
        // (Temp because it should average out all the values or something)
        _remoteInfo.latency = _sendBuffer[ind].stopwatch.getElapsedTime<std::chrono::microseconds>();
        _remoteInfo.timeoutStopwatch.restart();

        _sendBuffer[ind].tag = TaggedPacket::AcknowledgedStrongly;
        _sendBuffer[ind].packetWrap.packet.clear();

        if (ind == 0) {
            while (!_sendBuffer.empty() && _sendBuffer[0].tag == TaggedPacket::AcknowledgedStrongly) {
                _sendBuffer.pop_front();
                _sendBufferHeadIndex += 1;
            }
        }
    }
}

void RN_UdpConnector::initializeSession() {
    _status = RN_ConnectorStatus::Connected;
    _remoteInfo.timeoutStopwatch.restart();
}

void RN_UdpConnector::prepareNextOutgoingPacket() {
    _sendBuffer.emplace_back();
    _sendBuffer.back().tag = TaggedPacket::ReadyForSending;

    util::Packet& packet = _sendBuffer.back().packetWrap.packet;
    // Message type:
    packet << UDP_PACKET_TYPE_DATA;
    // Message ordinal:
    packet << static_cast<std::uint32_t>(_sendBuffer.size() + _sendBufferHeadIndex - 1u);
    // Strong Acknowledges (zero-terminated):
    for (std::uint32_t ackOrdinal : _ackOrdinals) {
        packet << ackOrdinal;
    }
    packet << static_cast<std::uint32_t>(0);
    _ackOrdinals.clear();
}

void RN_UdpConnector::receiveDataMessage(RN_PacketWrapper& packetWrap) {
    const std::uint32_t messageOrdinal = packetWrap.extractOrThrow<std::uint32_t>();

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
        const std::uint32_t ackOrdinal = packetWrap.extractOrThrow<std::uint32_t>();
        if (ackOrdinal == 0u) {
            break;
        }
        receivedAck(ackOrdinal, true);
    }

    _recvBuffer[indexInBuffer].packetWrap = std::move(packetWrap);
    _recvBuffer[indexInBuffer].tag = TaggedPacket::ReadyForUnpacking;

    prepareAck(messageOrdinal);
}


void RN_UdpConnector::processHelloPacket(RN_PacketWrapper& packetWrap) {
    switch (_status) {
    case RN_ConnectorStatus::Connecting:
        throw FatalMessageTypeReceived{"Received HELLO message (status: Connecting)"};
        break;

    case RN_ConnectorStatus::Accepting:
        NO_OP();
        break;

    case RN_ConnectorStatus::Connected:
        // TODO erroneous, nonfatal
        // TODO log event
        break;

    default:
        assert(0 && "Unreachable");
        break;
    }
}

void RN_UdpConnector::processConnectPacket(RN_PacketWrapper& packetWrap) {
    switch (_status) {
    case RN_ConnectorStatus::Connecting:
    {
        std::string receivedPassphrase = packetWrap.extractOrThrow<std::string>();
        const PZInteger receivedClientIndex = packetWrap.extractOrThrow<PZInteger>();
        if (receivedPassphrase == _passphrase) {
            // Client connected to server
            _clientIndex = receivedClientIndex;
            initializeSession();
            _eventFactory.createConnected();
        }
        else {
            reset();
            _eventFactory.createBadPassphrase(std::move(receivedPassphrase));
        }
    }
    break;

    case RN_ConnectorStatus::Accepting:
        throw FatalMessageTypeReceived{"Received CONNECT message (status: Accepting)"};
        break;

    case RN_ConnectorStatus::Connected:
        // TODO erroneous, nonfatal
        // TODO log event
        break;

    default:
        assert(0 && "Unreachable");
        break;
    }
}

void RN_UdpConnector::processDisconnectPacket(RN_PacketWrapper& packetWrap) {
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

void RN_UdpConnector::processDataPacket(RN_PacketWrapper& packetWrap) {
    switch (_status) {
    case RN_ConnectorStatus::Connecting:
        throw FatalMessageTypeReceived{"Received DATA message (status: Connecting)"};
        break;

    case RN_ConnectorStatus::Accepting:
        initializeSession(); // New connection confirmed
        _eventFactory.createConnected();
        SWITCH_FALLTHROUGH;

    case RN_ConnectorStatus::Connected:
        receiveDataMessage(packetWrap);
        break;

    default:
        assert(0 && "Unreachable");
        break;
    }
}

void RN_UdpConnector::processAcksPacket(RN_PacketWrapper& packetWrap) {
    switch (_status) {
    case RN_ConnectorStatus::Connecting:
        throw FatalMessageTypeReceived{"Received ACKS message (status: Connecting)"};
        break;

    case RN_ConnectorStatus::Accepting:
        throw FatalMessageTypeReceived{"Received ACKS message (status: Accepting)"};
        break;

    case RN_ConnectorStatus::Connected:
        while (!packetWrap.packet.endOfPacket()) {
            const std::uint32_t ackOrd = packetWrap.extractOrThrow<std::uint32_t>();
            receivedAck(ackOrd, false);
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
