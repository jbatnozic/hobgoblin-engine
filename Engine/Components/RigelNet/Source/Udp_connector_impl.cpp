
#include "Udp_connector_impl.hpp"

#define HOBGOBLIN_RN_ZEROTIER_SUPPORT // TODO Temp.
#ifdef  HOBGOBLIN_RN_ZEROTIER_SUPPORT
#include <Ztcpp.hpp>
namespace zt = jbatnozic::ztcpp;
#endif

#include <cassert>
#include <chrono>
#include <stdexcept>

#include <Hobgoblin/Private/Pmacro_define.hpp>

HOBGOBLIN_NAMESPACE_START
namespace rn {

namespace {

constexpr std::uint32_t UDP_PACKET_TYPE_HELLO      = 0x3BF0E110;
constexpr std::uint32_t UDP_PACKET_TYPE_CONNECT    = 0x83C96CA4;
constexpr std::uint32_t UDP_PACKET_TYPE_DISCONNECT = 0xD0F235AB;
constexpr std::uint32_t UDP_PACKET_TYPE_DATA       = 0xA765B8F6;
constexpr std::uint32_t UDP_PACKET_TYPE_ACKS       = 0x71AC2519;

class FatalMessageTypeReceived : public std::runtime_error {
public:
    using std::runtime_error::runtime_error;
};

void HandleDataMessages(detail::RN_PacketWrapper& receivedPacket,
                        RN_NodeInterface& node, 
                        detail::RN_PacketWrapper*& pointerToCurrentPacket) {
    pointerToCurrentPacket = &receivedPacket;

    while (!receivedPacket.packet.endOfPacket()) {
        const auto handlerId = receivedPacket.extractOrThrow<detail::RN_HandlerId>();
        auto handlerFunc = detail::RN_GlobalHandlerMapper::getInstance().handlerWithId(handlerId);
        if (handlerFunc == nullptr) {
            throw RN_IllegalMessage("Requested handler does not exist");
        }
        (*handlerFunc)(node);
    }

    pointerToCurrentPacket = nullptr;
}

} // namespace

RN_UdpConnectorImpl::RN_UdpConnectorImpl(RN_SocketAdapter& socket, 
                                         const std::chrono::microseconds& timeoutLimit, 
                                         const std::string& passphrase, 
                                         const RN_RetransmitPredicate& retransmitPredicate,
                                         detail::EventFactory eventFactory)
    : _socket{socket}
    , _timeoutLimit{timeoutLimit}
    , _passphrase{passphrase}
    , _retransmitPredicate{retransmitPredicate}
    , _eventFactory{eventFactory}
    , _status{RN_ConnectorStatus::Disconnected}
{
}

bool RN_UdpConnectorImpl::tryAccept(sf::IpAddress addr, std::uint16_t port, detail::RN_PacketWrapper& packetWrap) {
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

        destroy();
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

void RN_UdpConnectorImpl::connect(sf::IpAddress addr, std::uint16_t port) {
    assert(_status == RN_ConnectorStatus::Disconnected);

    _remoteInfo = RN_RemoteInfo{addr, port};
    _status = RN_ConnectorStatus::Connecting;

    destroy();
    _sendBufferHeadIndex = 1u;
    _recvBufferHeadIndex = 1u;
    prepareNextOutgoingPacket();
}

void RN_UdpConnectorImpl::disconnect(bool notfiyRemote) {
    assert(_status != RN_ConnectorStatus::Disconnected);

    if (notfiyRemote && _status == RN_ConnectorStatus::Connected) {
        util::Packet packet;
        packet << UDP_PACKET_TYPE_DISCONNECT;
        if (!_socket.send(packet, _remoteInfo.ipAddress, _remoteInfo.port)) {
            // Do nothing - The connector is getting disconnected anyway...
        }
    }
    reset();
}

void RN_UdpConnectorImpl::checkForTimeout() {
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

void RN_UdpConnectorImpl::send() {
    assert(_status != RN_ConnectorStatus::Disconnected);

    switch (_status) {
    case RN_ConnectorStatus::Accepting:  // Send CONNECT messages to the client, until a DATA message is received  
    {
        util::Packet packet;
        packet << UDP_PACKET_TYPE_CONNECT << _passphrase << _clientIndex.value();
        if (!_socket.send(packet, _remoteInfo.ipAddress, _remoteInfo.port)) {
            reset();
            _eventFactory.createConnectAttemptFailed(RN_Event::ConnectAttemptFailed::Reason::Error);
        }
    }
    break;

    case RN_ConnectorStatus::Connecting: // Send HELLO messages to the server, until a CONNECT message is received
    {
        util::Packet packet;
        packet << UDP_PACKET_TYPE_HELLO << _passphrase;
        if (!_socket.send(packet, _remoteInfo.ipAddress, _remoteInfo.port)) {
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

void RN_UdpConnectorImpl::receivedPacket(detail::RN_PacketWrapper& packetWrap) {
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

void RN_UdpConnectorImpl::sendAcks() {
    assert(_status == RN_ConnectorStatus::Connected);

    if (_ackOrdinals.empty()) {
        return;
    }

    util::Packet packet;
    packet << UDP_PACKET_TYPE_ACKS;
    for (std::uint32_t ackOrdinal : _ackOrdinals) {
        packet << ackOrdinal;
    }

    if (!_socket.send(packet, _remoteInfo.ipAddress, _remoteInfo.port)) {
        reset();
        _eventFactory.createConnectAttemptFailed(RN_Event::ConnectAttemptFailed::Reason::Error);
    }
}

void RN_UdpConnectorImpl::handleDataMessages(RN_NodeInterface& node, 
                                             detail::RN_PacketWrapper*& pointerToCurrentPacket) {
    assert(_status != RN_ConnectorStatus::Disconnected);

    try {
        while (!_recvBuffer.empty() && _recvBuffer[0].tag == TaggedPacket::ReadyForUnpacking) {
            HandleDataMessages(_recvBuffer[0].packetWrap, node, pointerToCurrentPacket);
            _recvBuffer.pop_front();
            _recvBufferHeadIndex++;
        }
    }
    catch (RN_PacketReadError& ex) {
        _eventFactory.createDisconnected(RN_Event::Disconnected::Reason::Error, ex.whatString());
        reset();
    }
    catch (RN_IllegalMessage& ex) {
        _eventFactory.createDisconnected(RN_Event::Disconnected::Reason::Error, ex.whatString());
        reset();
    }
}

void RN_UdpConnectorImpl::setClientIndex(std::optional<PZInteger> clientIndex) {
    _clientIndex = clientIndex;
}

std::optional<PZInteger> RN_UdpConnectorImpl::getClientIndex() const {
    return _clientIndex;
}

const RN_RemoteInfo& RN_UdpConnectorImpl::getRemoteInfo() const noexcept {
    return _remoteInfo;
}

RN_ConnectorStatus RN_UdpConnectorImpl::getStatus() const noexcept {
    return _status;
}

PZInteger RN_UdpConnectorImpl::getSendBufferSize() const {
    return static_cast<PZInteger>(_sendBuffer.size());
}

PZInteger RN_UdpConnectorImpl::getRecvBufferSize() const {
    return static_cast<PZInteger>(_recvBuffer.size());
}

void RN_UdpConnectorImpl::appendToNextOutgoingPacket(const void *data, std::size_t sizeInBytes) {
    if (sizeInBytes > MAX_PACKET_SIZE) {
        throw util::TracedLogicError("Cannot send packets larger than " 
                                     + std::to_string(MAX_PACKET_SIZE) + " bytes.");
    }
    
    TaggedPacket* latestTaggedPacket = &_sendBuffer.back();
    if (latestTaggedPacket->packetWrap.packet.getDataSize() + sizeInBytes > MAX_PACKET_SIZE) {
        prepareNextOutgoingPacket();
        latestTaggedPacket = &_sendBuffer.back();
    }

    latestTaggedPacket->packetWrap.packet.append(data, sizeInBytes);
}

// Private

void RN_UdpConnectorImpl::destroy() {
    _sendBuffer.clear();
    _recvBuffer.clear();
    _ackOrdinals.clear();
}

void RN_UdpConnectorImpl::reset() {
    destroy();
    _remoteInfo = RN_RemoteInfo{};
    _status = RN_ConnectorStatus::Disconnected;
    _clientIndex.reset();
}

bool RN_UdpConnectorImpl::isConnectionTimedOut() const {
    if (_timeoutLimit <= std::chrono::microseconds{0}) {
        return false;
    }
    if (_remoteInfo.timeoutStopwatch.getElapsedTime() >= _timeoutLimit) {
        return true;
    }
    return false;
}

void RN_UdpConnectorImpl::uploadAllData() {
    PZInteger uploadCounter = 0;
    for (auto& taggedPacket : _sendBuffer) {
        if (taggedPacket.tag == TaggedPacket::AcknowledgedWeakly ||
            taggedPacket.tag == TaggedPacket::AcknowledgedStrongly) {
            continue;
        }

        // TODO Send, restart clock etc.
        if ((taggedPacket.tag == TaggedPacket::ReadyForSending)
            || _retransmitPredicate(taggedPacket.cyclesSinceLastTransmit, taggedPacket.stopwatch.getElapsedTime(),
                                    _remoteInfo.latency)) {

            if (!_socket.send(taggedPacket.packetWrap.packet, _remoteInfo.ipAddress, _remoteInfo.port)) {
                // TODO Handle error, log event
                //std::cout << "UPLOAD ERROR size=" << taggedPacket.packetWrap.packet.getDataSize() << '\n';
            }

            taggedPacket.stopwatch.restart();
            taggedPacket.cyclesSinceLastTransmit = 0;
            uploadCounter += 1;
            // TODO Break if uploadCounter too large [configurable]
        }
            
        taggedPacket.cyclesSinceLastTransmit += 1;
        taggedPacket.tag = TaggedPacket::NotAcknowledged;
    }

    prepareNextOutgoingPacket();
}

void RN_UdpConnectorImpl::prepareAck(std::uint32_t ordinal) {
    _ackOrdinals.push_back(ordinal);
}

void RN_UdpConnectorImpl::receivedAck(std::uint32_t ordinal, bool strong) {
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

void RN_UdpConnectorImpl::initializeSession() {
    _status = RN_ConnectorStatus::Connected;
    _remoteInfo.timeoutStopwatch.restart();
}

void RN_UdpConnectorImpl::prepareNextOutgoingPacket() {
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

void RN_UdpConnectorImpl::receiveDataMessage(detail::RN_PacketWrapper& packetWrap) {
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


void RN_UdpConnectorImpl::processHelloPacket(detail::RN_PacketWrapper& packetWrap) {
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

void RN_UdpConnectorImpl::processConnectPacket(detail::RN_PacketWrapper& packetWrap) {
    switch (_status) {
    case RN_ConnectorStatus::Connecting:
    {
        std::string receivedPassphrase = packetWrap.extractOrThrow<std::string>();
        const PZInteger receivedClientIndex = packetWrap.extractOrThrow<PZInteger>();
        if (receivedPassphrase == _passphrase) {
            // Client connected to server
            _clientIndex = receivedClientIndex;
            _eventFactory.createConnected();
            initializeSession();
        }
        else {
            _eventFactory.createBadPassphrase(std::move(receivedPassphrase));
            reset();
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

void RN_UdpConnectorImpl::processDisconnectPacket(detail::RN_PacketWrapper& packetWrap) {
    switch (_status) {
    case RN_ConnectorStatus::Connecting:
    case RN_ConnectorStatus::Accepting:
    case RN_ConnectorStatus::Connected:
        // TODO - Extract reason
        _eventFactory.createDisconnected(RN_Event::Disconnected::Reason::Graceful,
                                         "Remote terminated the connection");
        reset();
        break;

    default:
        assert(0 && "Unreachable");
        break;
    }
}

void RN_UdpConnectorImpl::processDataPacket(detail::RN_PacketWrapper& packetWrap) {
    switch (_status) {
    case RN_ConnectorStatus::Connecting:
        throw FatalMessageTypeReceived{"Received DATA message (status: Connecting)"};
        break;

    case RN_ConnectorStatus::Accepting:       
        _eventFactory.createConnected();
        initializeSession(); // New connection confirmed
        SWITCH_FALLTHROUGH;

    case RN_ConnectorStatus::Connected:
        receiveDataMessage(packetWrap);
        break;

    default:
        assert(0 && "Unreachable");
        break;
    }
}

void RN_UdpConnectorImpl::processAcksPacket(detail::RN_PacketWrapper& packetWrap) {
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

} // namespace rn
HOBGOBLIN_NAMESPACE_END

#include <Hobgoblin/Private/Pmacro_undef.hpp>
