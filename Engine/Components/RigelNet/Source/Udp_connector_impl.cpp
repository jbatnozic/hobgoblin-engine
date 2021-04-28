
#include "Udp_connector_impl.hpp"

#include "Udp_server_impl.hpp"

#include <cassert>
#include <chrono>
#include <mutex>
#include <stdexcept>
#include <utility>

#include <Hobgoblin/Private/Pmacro_define.hpp>

HOBGOBLIN_NAMESPACE_BEGIN
namespace rn {

bool RN_DefaultRetransmitPredicate(PZInteger /*aCyclesSinceLastTransmit*/, 
                                   std::chrono::microseconds aTimeSinceLastSend,
                                   std::chrono::microseconds aCurrentLatency) {
    return (aTimeSinceLastSend >= (2 * aCurrentLatency));
}

//! Class used when two Connectors are connected locally so they can communicate
//! in a thread-safe way.
class RN_UdpConnectorImpl::LocalConnectionSharedState {
public:

#define LCSS_STATUS_ACTIVE         0
#define LCSS_STATUS_ENDED_GRACEFUL 1
#define LCSS_STATUS_ENDED_ERROR    2

    LocalConnectionSharedState(RN_UdpConnectorImpl& aConnector1,
                               RN_UdpConnectorImpl& aConnector2)
        : _connector1{aConnector1}
        , _connector2{aConnector2}
    {
    }

    void putData(RN_UdpConnectorImpl& aSelf,
                 std::deque<TaggedPacket>&& aData) {
        std::lock_guard<decltype(_mutex)> lock{_mutex};
        auto& targetDeque = (&aSelf == &_connector1) ? _packetsForConnector2 
                                                     : _packetsForConnector1;
        if (targetDeque.empty()) {
            targetDeque = std::move(aData);
        }
        else {
            for (auto& curr : aData) {
                targetDeque.emplace_back();
                targetDeque.back().packetWrap = std::move(curr.packetWrap);
            }
        }
        aData.clear();
    }

    //! Returns true if any data was received
    bool getData(RN_UdpConnectorImpl& aSelf,
                 std::deque<TaggedPacket>& aData) {
        std::lock_guard<decltype(_mutex)> lock{_mutex};

        auto& sourceDeque = (&aSelf == &_connector1) ? _packetsForConnector1 
                                                     : _packetsForConnector2;
        if (sourceDeque.empty()) {
            return false;
        }

        for (auto& curr : sourceDeque) {
            aSelf.receivedPacket(curr.packetWrap);
        }

        sourceDeque.clear();
        return true;
    }

    int getStatus() const {
        std::lock_guard<decltype(_mutex)> lock{_mutex};
        return _status;
    }

    void setStatus(int aNewStatus) {
        std::lock_guard<decltype(_mutex)> lock{_mutex};
        // Status values rise in order of severity,
        // so keep the highest value (worst one):
        if (aNewStatus > _status) {
            _status = aNewStatus;
        }
    }

private:
    RN_UdpConnectorImpl& _connector1;
    RN_UdpConnectorImpl& _connector2;
    std::deque<TaggedPacket> _packetsForConnector1;
    std::deque<TaggedPacket> _packetsForConnector2;
    mutable std::mutex _mutex;
    int _status = LCSS_STATUS_ACTIVE;
};

namespace {

constexpr std::uint32_t UDP_PACKET_TYPE_HELLO      = 0x3BF0E110;
constexpr std::uint32_t UDP_PACKET_TYPE_CONNECT    = 0x83C96CA4;
constexpr std::uint32_t UDP_PACKET_TYPE_DISCONNECT = 0xD0F235AB;
constexpr std::uint32_t UDP_PACKET_TYPE_DATA       = 0xA765B8F6;
constexpr std::uint32_t UDP_PACKET_TYPE_ACKS       = 0x71AC2519;

class FatalPacketTypeReceived : public std::runtime_error {
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
                                         detail::EventFactory eventFactory,
                                         PZInteger aMaxPacketSize)
    : _socket{socket}
    , _timeoutLimit{timeoutLimit}
    , _passphrase{passphrase}
    , _retransmitPredicate{retransmitPredicate}
    , _eventFactory{eventFactory}
    , _maxPacketSize{aMaxPacketSize}
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

        _resetBuffers();
        _prepareNextOutgoingPacket();
    }
    else {
        // TODO - Notify of erroneous message from unknown sender...
        return false;
    }

    return true;
}

bool RN_UdpConnectorImpl::tryAcceptLocal(RN_UdpConnectorImpl& localPeer, const std::string& passphrase) {
    assert(_status == RN_ConnectorStatus::Disconnected);

    if (passphrase == _passphrase) {
        _localSharedState = std::make_shared<LocalConnectionSharedState>(SELF, localPeer);
        localPeer._localSharedState = _localSharedState;

        _remoteInfo = RN_RemoteInfo{sf::IpAddress::LocalHost, 0};
        _status = RN_ConnectorStatus::Connected;
        _resetBuffers();
        _prepareNextOutgoingPacket();
        
        _eventFactory.createConnected();
        _startSession();

        return true;
    }

    // TODO - Notify of erroneous message from unknown sender...
    return false;
}

void RN_UdpConnectorImpl::connect(sf::IpAddress addr, std::uint16_t port) {
    assert(_status == RN_ConnectorStatus::Disconnected);

    _remoteInfo = RN_RemoteInfo{addr, port};
    _status = RN_ConnectorStatus::Connecting;

    _resetBuffers();
    _prepareNextOutgoingPacket();
}

void RN_UdpConnectorImpl::connectLocal(RN_ServerInterface& server) {
    assert(_status == RN_ConnectorStatus::Disconnected);

    auto* udpServer = dynamic_cast<RN_UdpServerImpl*>(&server);
    if (!udpServer) {
        throw util::TracedLogicError("Incompatible node types for local connection");
        return;
    }

    const int clientIndex = udpServer->acceptLocalConnection(SELF, _passphrase);
    if (clientIndex < 0) {
        throw util::TracedRuntimeError("Local connection refused");
        return;
    }

    assert(_localSharedState != nullptr);

    _remoteInfo = RN_RemoteInfo{sf::IpAddress::LocalHost, 0};
    _status = RN_ConnectorStatus::Connected;

    _resetBuffers();
    _prepareNextOutgoingPacket();

    _clientIndex = clientIndex;
    _eventFactory.createConnected();
}

void RN_UdpConnectorImpl::disconnect(bool notfiyRemote) {
    assert(_status != RN_ConnectorStatus::Disconnected);

    if (notfiyRemote && _status == RN_ConnectorStatus::Connected) {
        if (!_isConnectedLocally()) {
            util::Packet packet;
            packet << UDP_PACKET_TYPE_DISCONNECT;

            // Ignore all recoverable errors - The connector is getting disconnected anyway...
            _socket.send(packet, _remoteInfo.ipAddress, _remoteInfo.port);
        }
    }

    _resetAll();
}

void RN_UdpConnectorImpl::checkForTimeout() {
    assert(_status != RN_ConnectorStatus::Disconnected);

    if (_isConnectionTimedOut()) {
        _resetAll();

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
    case RN_ConnectorStatus::Accepting:  // Send CONNECT packets to the client, until a DATA packet is received  
    {
        util::Packet packet;
        packet << UDP_PACKET_TYPE_CONNECT << _passphrase << _clientIndex.value();

        // Safe to ignore recoverable errors here - Disconnected doesn't happen with UDP and
        // NotReady is irrelevant because CONNECTs keep getting resent until acknowledged anyway
        _socket.send(packet, _remoteInfo.ipAddress, _remoteInfo.port);
    }
    break;

    case RN_ConnectorStatus::Connecting: // Send HELLO packets to the server, until a CONNECT packet is received
    {
        util::Packet packet;
        packet << UDP_PACKET_TYPE_HELLO << _passphrase;

        // Safe to ignore recoverable errors here - Disconnected doesn't happen with UDP and
        // NotReady is irrelevant because HELLOs keep getting resent until acknowledged anyway
        _socket.send(packet, _remoteInfo.ipAddress, _remoteInfo.port);
    }
    break;

    case RN_ConnectorStatus::Connected:
        if (!_isConnectedLocally()) {
            _uploadAllData();
        }
        else {
            _transferAllDataToLocalPeer();
        }
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
            _processHelloPacket(packetWrap);
            break;

        case UDP_PACKET_TYPE_CONNECT:
            _processConnectPacket(packetWrap);
            break;

        case UDP_PACKET_TYPE_DISCONNECT:
            _processDisconnectPacket(packetWrap);
            break;

        case UDP_PACKET_TYPE_DATA:
            _processDataPacket(packetWrap);
            break;

        case UDP_PACKET_TYPE_ACKS:
            _processAcksPacket(packetWrap);
            break;

        default:
            throw FatalPacketTypeReceived{"Received UNKNOWN packet type"};
            break;
        }
    }
    catch (FatalPacketTypeReceived& ex) {
        _resetAll();
        if (_status == RN_ConnectorStatus::Connected) {
            _eventFactory.createDisconnected(RN_Event::Disconnected::Reason::Error, ex.what());
            if (_isConnectedLocally()) {
                _localSharedState->setStatus(LCSS_STATUS_ENDED_ERROR);
            }
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

    // Safe to ignore recoverable errors here - Disconnected doesn't happen with UDP and
    // NotReady is irrelevant because Acks keep getting resent until acknowledged back anyway
    _socket.send(packet, _remoteInfo.ipAddress, _remoteInfo.port);
}

void RN_UdpConnectorImpl::handleDataMessages(RN_NodeInterface& node, 
                                             detail::RN_PacketWrapper*& pointerToCurrentPacket) {
    assert(_status != RN_ConnectorStatus::Disconnected);

    if (_isConnectedLocally()) {
        if (_localSharedState->getData(SELF, _recvBuffer)) {
            _remoteInfo.timeoutStopwatch.restart();
        }
    }

    try {
        while (!_recvBuffer.empty() && _recvBuffer[0].tag == TaggedPacket::ReadyForUnpacking) {
            HandleDataMessages(_recvBuffer[0].packetWrap, node, pointerToCurrentPacket);
            _recvBuffer.pop_front();
            _recvBufferHeadIndex++;
        }
    }
    catch (RN_PacketReadError& ex) {
        _eventFactory.createDisconnected(RN_Event::Disconnected::Reason::Error, ex.whatString());
        if (_isConnectedLocally()) {
            _localSharedState->setStatus(LCSS_STATUS_ENDED_ERROR);
        }
        _resetAll();
    }
    catch (RN_IllegalMessage& ex) {
        _eventFactory.createDisconnected(RN_Event::Disconnected::Reason::Error, ex.whatString());
        _resetAll();
    }

    if (_isConnectedLocally()) {
        switch (_localSharedState->getStatus()) {
        case LCSS_STATUS_ACTIVE:
            // All good, carry on
            break;

        case LCSS_STATUS_ENDED_GRACEFUL:
            _eventFactory.createDisconnected(RN_Event::Disconnected::Reason::Graceful, 
                                             "Remote terminated the connection.");
            _resetAll();
            break;

        case LCSS_STATUS_ENDED_ERROR:
            _eventFactory.createDisconnected(RN_Event::Disconnected::Reason::Error, 
                                             "Connection closed due to an error.");
            _resetAll();
            break;

        default:
            assert(false && "Unreachable!");
            NO_OP();
        }
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

bool RN_UdpConnectorImpl::isConnectedLocally() const noexcept {
    return _isConnectedLocally();
}

PZInteger RN_UdpConnectorImpl::getSendBufferSize() const {
    return static_cast<PZInteger>(_sendBuffer.size());
}

PZInteger RN_UdpConnectorImpl::getRecvBufferSize() const {
    return static_cast<PZInteger>(_recvBuffer.size());
}

void RN_UdpConnectorImpl::appendToNextOutgoingPacket(const void *data, std::size_t sizeInBytes) {
    if (sizeInBytes > _maxPacketSize) {
        throw util::TracedLogicError("Cannot send packets larger than " 
                                     + std::to_string(_maxPacketSize) + " bytes.");
    }
    
    TaggedPacket* latestTaggedPacket = &_sendBuffer.back();
    if (latestTaggedPacket->packetWrap.packet.getDataSize() + sizeInBytes > _maxPacketSize) {
        _prepareNextOutgoingPacket();
        latestTaggedPacket = &_sendBuffer.back();
    }

    latestTaggedPacket->packetWrap.packet.append(data, sizeInBytes);
}

///////////////////////////////////////////////////////////////////////////
// PRIVATE METHODS                                                       //
///////////////////////////////////////////////////////////////////////////

bool RN_UdpConnectorImpl::_isConnectedLocally() const noexcept {
    return (_localSharedState != nullptr);
}

void RN_UdpConnectorImpl::_resetBuffers() {
    _sendBuffer.clear();
    _recvBuffer.clear();
    _ackOrdinals.clear();

    _sendBufferHeadIndex = 1;
    _recvBufferHeadIndex = 1;
}

void RN_UdpConnectorImpl::_resetAll() {
    _resetBuffers();

    _remoteInfo = RN_RemoteInfo{};
    _status = RN_ConnectorStatus::Disconnected;
    _clientIndex.reset();

    if (_isConnectedLocally()) {
        _localSharedState->setStatus(LCSS_STATUS_ENDED_GRACEFUL);
        _localSharedState.reset();
    }
}

bool RN_UdpConnectorImpl::_isConnectionTimedOut() const {
    if (_timeoutLimit <= std::chrono::microseconds{0}) {
        return false;
    }
    if ((_remoteInfo.timeoutStopwatch.getElapsedTime() >= _timeoutLimit) &&
        !_isConnectedLocally()) {
        return true;
    }
    return false;
}

void RN_UdpConnectorImpl::_uploadAllData() {
    PZInteger uploadCounter = 0;
    for (auto& taggedPacket : _sendBuffer) {
        if (taggedPacket.tag == TaggedPacket::AcknowledgedWeakly ||
            taggedPacket.tag == TaggedPacket::AcknowledgedStrongly) {
            continue;
        }

        bool socketCannotSendMore = false;
        if ((taggedPacket.tag == TaggedPacket::ReadyForSending)
            || _retransmitPredicate(taggedPacket.cyclesSinceLastTransmit, taggedPacket.stopwatch.getElapsedTime(),
                                    _remoteInfo.latency)) {

            switch (_socket.send(taggedPacket.packetWrap.packet, _remoteInfo.ipAddress, _remoteInfo.port)) {
            case RN_SocketAdapter::Status::OK:
                // All good, carry on
                break;

            case RN_SocketAdapter::Status::NotReady:
                socketCannotSendMore = true;
                break;

            case RN_SocketAdapter::Status::Disconnected:
                // Normally we wouldn't expect to get this status from UDP sockets. However,
                // it case it does somehow happen, treat it as the remote gracefully terminating
                // the connection.
                _eventFactory.createDisconnected(RN_Event::Disconnected::Reason::Graceful,
                                                 "Remote terminated the connection");
                _resetAll();
                break;

            default: 
                assert(false && "Unreachable");
            }

            taggedPacket.stopwatch.restart();
            taggedPacket.cyclesSinceLastTransmit = 0;
            uploadCounter += 1;

            // TODO Configurable uploadCounter max
            if (socketCannotSendMore || uploadCounter == 1000) {
                break;
            }
        }
            
        taggedPacket.cyclesSinceLastTransmit += 1;
        taggedPacket.tag = TaggedPacket::NotAcknowledged;
    }

    _prepareNextOutgoingPacket();
}

void RN_UdpConnectorImpl::_transferAllDataToLocalPeer() {
    assert(_isConnectedLocally());

    _sendBufferHeadIndex += _sendBuffer.size();
    _localSharedState->putData(SELF, std::move(_sendBuffer));  
    _sendBuffer.clear();
    _prepareNextOutgoingPacket();
}

void RN_UdpConnectorImpl::_prepareAck(std::uint32_t ordinal) {
    if (_isConnectedLocally()) {
        // No acks needed in local connections.
        return;
    }
    _ackOrdinals.push_back(ordinal);    
}

void RN_UdpConnectorImpl::_receivedAck(std::uint32_t ordinal, bool strong) {
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

void RN_UdpConnectorImpl::_startSession() {
    _status = RN_ConnectorStatus::Connected;
    _remoteInfo.timeoutStopwatch.restart();
}

void RN_UdpConnectorImpl::_prepareNextOutgoingPacket() {
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

void RN_UdpConnectorImpl::_saveDataPacket(detail::RN_PacketWrapper& packetWrap) {
    const std::uint32_t packetOrdinal = packetWrap.extractOrThrow<std::uint32_t>();

    if (packetOrdinal < _recvBufferHeadIndex) {
        // Old data - acknowledge and ignore
        _prepareAck(packetOrdinal);
        return;
    }

    const std::uint32_t indexInBuffer = (packetOrdinal - _recvBufferHeadIndex);
    if (indexInBuffer >= _recvBuffer.size()) {
        _recvBuffer.resize(indexInBuffer + 1u);
    }
    else if (_recvBuffer[indexInBuffer].tag != TaggedPacket::WaitingForData) {
        // Already received this - acknowledge and ignore
        _prepareAck(packetOrdinal);
        return;
    }

    while (true) {
        const std::uint32_t ackOrdinal = packetWrap.extractOrThrow<std::uint32_t>();
        if (ackOrdinal == 0u) {
            break;
        }
        _receivedAck(ackOrdinal, true);
    }

    _recvBuffer[indexInBuffer].packetWrap = std::move(packetWrap);
    _recvBuffer[indexInBuffer].tag = TaggedPacket::ReadyForUnpacking;

    _prepareAck(packetOrdinal);
}


void RN_UdpConnectorImpl::_processHelloPacket(detail::RN_PacketWrapper& packetWrap) {
    switch (_status) {
    case RN_ConnectorStatus::Connecting:
        throw FatalPacketTypeReceived{"Received HELLO packet (status: Connecting)"};
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

void RN_UdpConnectorImpl::_processConnectPacket(detail::RN_PacketWrapper& packetWrap) {
    switch (_status) {
    case RN_ConnectorStatus::Connecting:
    {
        std::string receivedPassphrase = packetWrap.extractOrThrow<std::string>();
        const PZInteger receivedClientIndex = packetWrap.extractOrThrow<PZInteger>();
        if (receivedPassphrase == _passphrase) {
            // Client connected to server
            _clientIndex = receivedClientIndex;
            _eventFactory.createConnected();
            _startSession();
        }
        else {
            _eventFactory.createBadPassphrase(std::move(receivedPassphrase));
            _resetAll();
        }
    }
    break;

    case RN_ConnectorStatus::Accepting:
        throw FatalPacketTypeReceived{"Received CONNECT packet (status: Accepting)"};
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

void RN_UdpConnectorImpl::_processDisconnectPacket(detail::RN_PacketWrapper& packetWrap) {
    switch (_status) {
    case RN_ConnectorStatus::Connecting:
    case RN_ConnectorStatus::Accepting:
    case RN_ConnectorStatus::Connected:
        // TODO - Extract reason
        _eventFactory.createDisconnected(RN_Event::Disconnected::Reason::Graceful,
                                         "Remote terminated the connection");
        _resetAll();
        break;

    default:
        assert(0 && "Unreachable");
        break;
    }
}

void RN_UdpConnectorImpl::_processDataPacket(detail::RN_PacketWrapper& packetWrap) {
    switch (_status) {
    case RN_ConnectorStatus::Connecting:
        throw FatalPacketTypeReceived{"Received DATA packet (status: Connecting)"};
        break;

    case RN_ConnectorStatus::Accepting:       
        _eventFactory.createConnected();
        _startSession(); // New connection confirmed
        SWITCH_FALLTHROUGH;

    case RN_ConnectorStatus::Connected:
        _saveDataPacket(packetWrap);
        break;

    default:
        assert(0 && "Unreachable");
        break;
    }
}

void RN_UdpConnectorImpl::_processAcksPacket(detail::RN_PacketWrapper& packetWrap) {
    switch (_status) {
    case RN_ConnectorStatus::Connecting:
        throw FatalPacketTypeReceived{"Received ACKS packet (status: Connecting)"};
        break;

    case RN_ConnectorStatus::Accepting:
        throw FatalPacketTypeReceived{"Received ACKS packet (status: Accepting)"};
        break;

    case RN_ConnectorStatus::Connected:
        while (!packetWrap.packet.endOfPacket()) {
            const std::uint32_t ackOrd = packetWrap.extractOrThrow<std::uint32_t>();
            _receivedAck(ackOrd, false);
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
