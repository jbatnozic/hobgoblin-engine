
#include "Udp_connector_impl.hpp"
#include "Udp_server_impl.hpp"

#include <Hobgoblin/Common.hpp>

#include <cassert>
#include <cstring>
#include <chrono>
#include <mutex>
#include <stdexcept>
#include <utility>

#include <Hobgoblin/Private/Pmacro_define.hpp>

HOBGOBLIN_NAMESPACE_BEGIN
namespace rn {

namespace {
constexpr auto UDP_HEADER_BYTE_COUNT = 8u;
} // namespace

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
        {
            std::lock_guard<decltype(_mutex)> lock{_mutex};
            auto& targetDeque = (&aSelf == &_connector1) ? _packetsForConnector2 
                                                         : _packetsForConnector1;
            if (targetDeque.empty()) {
                targetDeque = std::move(aData);
            }
            else {
                for (auto& curr : aData) {
                    targetDeque.emplace_back();
                    targetDeque.back().packet = std::move(curr.packet);
                }
            }
        }
        aData.clear();
    }

    //! Returns true if any data was received
    bool getData(RN_UdpConnectorImpl& aSelf) {
        std::deque<TaggedPacket> temp;
        {
            std::lock_guard<decltype(_mutex)> lock{_mutex};

            auto& sourceDeque = (&aSelf == &_connector1) ? _packetsForConnector1 
                                                         : _packetsForConnector2;
            std::swap(sourceDeque, temp);
        }

        if (temp.empty()) {
            return false;
        }

        for (auto& curr : temp) {
            aSelf.receivedPacket(curr.packet);
        }

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
constexpr std::uint32_t UDP_PACKET_TYPE_DATA_MORE  = 0x782A2A78;
constexpr std::uint32_t UDP_PACKET_TYPE_DATA_TAIL  = 0x00DA7A11;
constexpr std::uint32_t UDP_PACKET_TYPE_ACKS       = 0x71AC2519;

class FatalPacketTypeReceived : public std::runtime_error {
public:
    using std::runtime_error::runtime_error;
};

void HandleDataMessages(util::Packet& receivedPacket,
                        RN_NodeInterface& node, 
                        util::Packet*& pointerToCurrentPacket) {
    pointerToCurrentPacket = &receivedPacket;

    while (!receivedPacket.endOfPacket()) {
        const auto handlerId = receivedPacket.extractOrThrow<detail::RN_HandlerId>();
        auto handlerFunc = detail::RN_GlobalHandlerMapper::getInstance().handlerWithId(handlerId);
        if (handlerFunc == nullptr) {
            throw RN_IllegalMessage("Requested handler does not exist");
        }
        (*handlerFunc)(node);
    }

    pointerToCurrentPacket = nullptr;
}

//! Endianess-agnostic implementation of ntoh for 32bit integers
std::int32_t ntoh32(std::int32_t net) {
    uint8_t buf[4];
    std::memcpy(&buf, &net, sizeof(buf));

    return ((std::uint32_t) buf[3] <<  0)
         | ((std::uint32_t) buf[2] <<  8)
         | ((std::uint32_t) buf[1] << 16)
         | ((std::uint32_t) buf[0] << 24);
}

std::int32_t hton32(std::int32_t host) {
    return ntoh32(host); // These functions actually perform the same operation :)
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

bool RN_UdpConnectorImpl::tryAccept(sf::IpAddress addr, std::uint16_t port, util::Packet& packet) {
    assert(_status == RN_ConnectorStatus::Disconnected);

    const std::uint32_t msgType = packet.extract<std::uint32_t>();
    const std::string receivedPassphrase = packet.extract<std::string>();
    if (!packet) {
        // TODO Notify of error
        return false;
    }

    if (msgType == UDP_PACKET_TYPE_HELLO
        && receivedPassphrase == _passphrase) {

        _remoteInfo = RN_RemoteInfo{addr, port};
        _status = RN_ConnectorStatus::Accepting;

        _resetBuffers();
        _prepareNextOutgoingDataPacket(UDP_PACKET_TYPE_DATA);
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
        _prepareNextOutgoingDataPacket(UDP_PACKET_TYPE_DATA);
        
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
    _prepareNextOutgoingDataPacket(UDP_PACKET_TYPE_DATA);
}

void RN_UdpConnectorImpl::connectLocal(RN_ServerInterface& server) {
    assert(_status == RN_ConnectorStatus::Disconnected);

    auto* udpServer = dynamic_cast<RN_UdpServerImpl*>(&server);
    if (!udpServer) {
        throw TracedLogicError("Incompatible node types for local connection");
    }

    const int clientIndex = udpServer->acceptLocalConnection(SELF, _passphrase);
    if (clientIndex < 0) {
        throw TracedRuntimeError("Local connection refused");
    }

    assert(_localSharedState != nullptr);

    _remoteInfo = RN_RemoteInfo{sf::IpAddress::LocalHost, 0};
    _status = RN_ConnectorStatus::Connected;

    _resetBuffers();
    _prepareNextOutgoingDataPacket(UDP_PACKET_TYPE_DATA);

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
    }
}

RN_Telemetry RN_UdpConnectorImpl::send() {
    assert(_status != RN_ConnectorStatus::Disconnected);
    RN_Telemetry telemetry;

    switch (_status) {
    case RN_ConnectorStatus::Accepting:  // Send CONNECT packets to the client, until a DATA packet is received  
    {
        util::Packet packet;
        packet << UDP_PACKET_TYPE_CONNECT << _passphrase << _clientIndex.value();

        // Safe to ignore recoverable errors here - Disconnected doesn't happen with UDP and
        // NotReady is irrelevant because CONNECTs keep getting resent until acknowledged anyway
        _socket.send(packet, _remoteInfo.ipAddress, _remoteInfo.port);
        telemetry.uploadByteCount += stopz(packet.getDataSize() + UDP_HEADER_BYTE_COUNT);
    }
    break;

    case RN_ConnectorStatus::Connecting: // Send HELLO packets to the server, until a CONNECT packet is received
    {
        util::Packet packet;
        packet << UDP_PACKET_TYPE_HELLO << _passphrase;

        // Safe to ignore recoverable errors here - Disconnected doesn't happen with UDP and
        // NotReady is irrelevant because HELLOs keep getting resent until acknowledged anyway
        _socket.send(packet, _remoteInfo.ipAddress, _remoteInfo.port);
        telemetry.uploadByteCount += stopz(packet.getDataSize() + UDP_HEADER_BYTE_COUNT);
    }
    break;

    case RN_ConnectorStatus::Connected:
        if (!_isConnectedLocally()) {
            telemetry.uploadByteCount += _uploadAllData();
        }
        else {
            _transferAllDataToLocalPeer();
        }
        break;

    default:
        HARD_ASSERT(false && "Unreachable");
        break;
    }

    return telemetry;
}

void RN_UdpConnectorImpl::prepToReceive() {
    _newMeanLatency = decltype(_newMeanLatency){0};
    _newLatencySampleSize = 0;
}

void RN_UdpConnectorImpl::receivedPacket(util::Packet& packet) {
    assert(_status != RN_ConnectorStatus::Disconnected);

    try {
        const auto packetType = packet.extractOrThrow<std::uint32_t>();

        switch (packetType) {
        case UDP_PACKET_TYPE_HELLO:
            _processHelloPacket(packet);
            break;

        case UDP_PACKET_TYPE_CONNECT:
            _processConnectPacket(packet);
            break;

        case UDP_PACKET_TYPE_DISCONNECT:
            _processDisconnectPacket(packet);
            break;

        case UDP_PACKET_TYPE_DATA:
            _processDataPacket(packet);
            break;

        case UDP_PACKET_TYPE_DATA_MORE:
            _processDataMorePacket(packet);
            break;

        case UDP_PACKET_TYPE_DATA_TAIL:
            _processDataTailPacket(packet);
            break;

        case UDP_PACKET_TYPE_ACKS:
            _processAcksPacket(packet);
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

void RN_UdpConnectorImpl::receivingFinished() {
    if (_newLatencySampleSize > 0) {
        _remoteInfo.meanLatency = (_newMeanLatency / _newLatencySampleSize);
        _remoteInfo.optimisticLatency  = _newOptimisticLatency;
        _remoteInfo.pessimisticLatency = _newPessimisticLatency;
    }
}

RN_Telemetry RN_UdpConnectorImpl::sendAcks() {
    assert(_status == RN_ConnectorStatus::Connected);

    if (_ackOrdinals.empty()) {
        return {};
    }

    util::Packet packet;
    packet << UDP_PACKET_TYPE_ACKS;
    for (std::uint32_t ackOrdinal : _ackOrdinals) {
        packet << ackOrdinal;
    }

    // Safe to ignore recoverable errors here - Disconnected doesn't happen with UDP and
    // NotReady is irrelevant because Acks keep getting resent until acknowledged back anyway
    _socket.send(packet, _remoteInfo.ipAddress, _remoteInfo.port);

    RN_Telemetry telemetry;
    telemetry.uploadByteCount = stopz(packet.getDataSize() + UDP_HEADER_BYTE_COUNT);
    return telemetry;
}

void RN_UdpConnectorImpl::handleDataMessages(RN_NodeInterface& node, 
                                             util::Packet*& pointerToCurrentPacket) {
    assert(_status != RN_ConnectorStatus::Disconnected);

    if (_skipNextDataPacketProcessing) {
        _skipNextDataPacketProcessing = false;
        return;
    }

    if (_isConnectedLocally()) {
        if (_localSharedState->getData(SELF)) {
            _remoteInfo.timeoutStopwatch.restart();
        }
    }

    try {
        while (!_recvBuffer.empty()) {
            _tryToAssembleFragmentedPacketAtHead();
            if (_recvBuffer[0].tag == TaggedPacket::ReadyForUnpacking) {
                HandleDataMessages(_recvBuffer[0].packet, node, 
                                   /* reference to pointer -> */ pointerToCurrentPacket);
                _recvBuffer.pop_front();
                _recvBufferHeadIndex++;
            }
            else if (_recvBuffer[0].tag == TaggedPacket::Unpacked) {
                _recvBuffer.pop_front();
                _recvBufferHeadIndex++;
            }
            else {
                break;
            }
        }
    }
    catch (util::Packet::ReadError& ex) {
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
            HARD_ASSERT(false && "Unreachable");
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
    assert(data != nullptr && sizeInBytes > 0);

    // We want to send independent DATA packets whenever possible, 
    // and fragmented only when necessary.
    if (sizeInBytes < _maxPacketSize) {
        TaggedPacket* sendBufTail = &_sendBuffer.back();
        if (sendBufTail->packet.getDataSize() + sizeInBytes > _maxPacketSize) {
            _prepareNextOutgoingDataPacket(UDP_PACKET_TYPE_DATA);
            sendBufTail = &_sendBuffer.back();
        }
        sendBufTail->packet.append(data, sizeInBytes);
    }
    else {
        // Prepare the current latest outgoing packet (finalize it if it's full enough, and
        // set its type to DATA_MORE otherwise):
        {
            TaggedPacket& sendBufTail = _sendBuffer.back();
            // Note: Data in the packet is stored in network order (big-endian)
            auto* sendBufTailPacketType = 
                static_cast<std::uint32_t*>(sendBufTail.packet.getMutableData());

            // This is kind of an arbitrarily chosen limit, but if the latest outgoing packet is at
            // least 50% full, we'll send it independently so avoid dependencies between packets.
            if (sendBufTail.packet.getDataSize() >= _maxPacketSize / 2) {
                _prepareNextOutgoingDataPacket(UDP_PACKET_TYPE_DATA_MORE);
            }
            else {
                // Otherwise we must edit the type of the packet onto which we're going to start 
                // appending the data to DATA_MORE, so that the recepient knows not to do anything
                // with it until the remaining fragments are also received and assembled.
                *sendBufTailPacketType = hton32(UDP_PACKET_TYPE_DATA_MORE);
            }
        }

        // Pack the data into multiple consecutive packets:
        std::size_t bytesPacked = 0;
        while (true) {
            TaggedPacket& sendBufTail = _sendBuffer.back();
            assert(pztos(_maxPacketSize) >= sendBufTail.packet.getDataSize());
            const std::size_t remainingCapacity =
                pztos(_maxPacketSize) - sendBufTail.packet.getDataSize();
            
            const std::size_t bytesToPackNow = std::min(remainingCapacity, 
                                                        sizeInBytes - bytesPacked);

            sendBufTail.packet.append(static_cast<const char*>(data) + bytesPacked,
                                          bytesToPackNow);
            bytesPacked += bytesToPackNow;

            if (bytesPacked < sizeInBytes) {
                _prepareNextOutgoingDataPacket(UDP_PACKET_TYPE_DATA_MORE);
            }
            else {
                break;
            }
        }

        // Mark the last outgoing packet as DATA_TAIL:
        {
            static const std::int32_t kTypeInNetworkOrder = hton32(UDP_PACKET_TYPE_DATA_TAIL);

            TaggedPacket& sendBufTail = _sendBuffer.back();

            // The first 4 bytes of a packet determine its type (but mind the endianess)
            std::memcpy(sendBufTail.packet.getMutableData(), 
                        &kTypeInNetworkOrder,
                        sizeof(kTypeInNetworkOrder));
        }

        // We don't want chaining of multiple fragmented packets, so finalize the tail and
        // start the next regular packet:
        _prepareNextOutgoingDataPacket(UDP_PACKET_TYPE_DATA);
    }
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

PZInteger RN_UdpConnectorImpl::_uploadAllData() {
    PZInteger uploadedByteCount = 0;
    PZInteger uploadCounter = 0;
    for (auto& taggedPacket : _sendBuffer) {
        if (taggedPacket.tag == TaggedPacket::AcknowledgedWeakly ||
            taggedPacket.tag == TaggedPacket::AcknowledgedStrongly) {
            continue;
        }

        bool socketCannotSendMore = false;
        if ((taggedPacket.tag == TaggedPacket::ReadyForSending)
            || _retransmitPredicate(taggedPacket.cyclesSinceLastTransmit, 
                                    taggedPacket.stopwatch.getElapsedTime(),
                                    _remoteInfo.meanLatency)) {

            switch (_socket.send(taggedPacket.packet, _remoteInfo.ipAddress, _remoteInfo.port)) {
            case RN_SocketAdapter::Status::OK:
                uploadedByteCount += stopz(taggedPacket.packet.getDataSize() + UDP_HEADER_BYTE_COUNT);
                // All good, carry on
                break;

            case RN_SocketAdapter::Status::NotReady:
                uploadedByteCount += stopz(taggedPacket.packet.getDataSize() + UDP_HEADER_BYTE_COUNT);
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
            const auto UPLOAD_COUNTER_MAX = 128;
            if (socketCannotSendMore || uploadCounter == UPLOAD_COUNTER_MAX) {
                break;
            }
        }
            
        taggedPacket.cyclesSinceLastTransmit += 1;
        taggedPacket.tag = TaggedPacket::NotAcknowledged;
    }

    _prepareNextOutgoingDataPacket(UDP_PACKET_TYPE_DATA);

    return uploadedByteCount;
}

void RN_UdpConnectorImpl::_transferAllDataToLocalPeer() {
    assert(_isConnectedLocally());

    _sendBufferHeadIndex += _sendBuffer.size();
    _localSharedState->putData(SELF, std::move(_sendBuffer));  
    _sendBuffer.clear();
    _prepareNextOutgoingDataPacket(UDP_PACKET_TYPE_DATA);
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
        _sendBuffer[ind].packet.clear();
    } 
    else {
        const auto timeToAck = _sendBuffer[ind].stopwatch.getElapsedTime<std::chrono::microseconds>();
        _newMeanLatency += timeToAck;
        if (_newLatencySampleSize == 0) {
            _newOptimisticLatency  = decltype(_newOptimisticLatency){0};
            _newPessimisticLatency = decltype(_newPessimisticLatency){0};
        }
        else {
            _newOptimisticLatency  = std::min(_newOptimisticLatency, timeToAck);
            _newPessimisticLatency = std::max(_newPessimisticLatency, timeToAck);
        }
        _newLatencySampleSize += 1;
        _remoteInfo.timeoutStopwatch.restart();

        _sendBuffer[ind].tag = TaggedPacket::AcknowledgedStrongly;
        _sendBuffer[ind].packet.clear();

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
    // It's important that events have a chance to be polled and
    // processed before we process any data packets.
    _skipNextDataPacketProcessing = true;
}

void RN_UdpConnectorImpl::_prepareNextOutgoingDataPacket(std::uint32_t packetType) {
    _sendBuffer.emplace_back();
    _sendBuffer.back().tag = TaggedPacket::ReadyForSending;

    util::Packet& packet = _sendBuffer.back().packet;
    // Message type:
    packet << packetType;
    // Message ordinal:
    packet << static_cast<std::uint32_t>(_sendBuffer.size() + _sendBufferHeadIndex - 1u);
    // Strong Acknowledges (zero-terminated):
    for (std::uint32_t ackOrdinal : _ackOrdinals) {
        packet << ackOrdinal;
    }
    packet << static_cast<std::uint32_t>(0);
    _ackOrdinals.clear();
}

void RN_UdpConnectorImpl::_tryToAssembleFragmentedPacketAtHead() {
    if (_recvBuffer.front().tag != TaggedPacket::WaitingForMore) {
        return;
    }

    bool allFragmentsPresent = false;
    for (const auto& taggedPacket : _recvBuffer) {
        switch (taggedPacket.tag) {
        case TaggedPacket::WaitingForData:
            // Still waiting to receive fragments, we can quit right away
            return;

        case TaggedPacket::WaitingForMore:
            // All good, keep going
            break;

        case TaggedPacket::WaitingForMore_Tail:
            // All fragments present!
            allFragmentsPresent = true;
            goto BREAK_FOR;

        default:
            // This isn't supposed to happen
            throw RN_IllegalMessage("Impossible to assemble fragmented packet");
        }
    }
BREAK_FOR:

    if (!allFragmentsPresent) {
        return;
    }

    // Append all data to head packet, tag it ReadyForUnpacking, and other fragments as Unpacked:
    for (std::size_t i = 1; ; i += 1) {
        TaggedPacket& curr = _recvBuffer[i];

        const auto currDataSize = curr.packet.getRemainingDataSize();
        _recvBuffer[0].packet.append(curr.packet.extractBytes(currDataSize),
                                     currDataSize);

        curr.packet.clear();

        if (curr.tag != TaggedPacket::WaitingForMore_Tail) {
            curr.tag = TaggedPacket::Unpacked;
        }
        else {
            curr.tag = TaggedPacket::Unpacked;
            break;
        }
    }
    _recvBuffer[0].tag = TaggedPacket::ReadyForUnpacking;
}

void RN_UdpConnectorImpl::_saveDataPacket(util::Packet& packet,
                                          std::uint32_t packetType) {
    const std::uint32_t packetOrdinal = packet.extractOrThrow<std::uint32_t>();

    if (packetOrdinal < _recvBufferHeadIndex) {
        // Old data - acknowledge and ignore
        _prepareAck(packetOrdinal);
        return;
    }

    const std::size_t indexInBuffer = (packetOrdinal - _recvBufferHeadIndex);
    if (indexInBuffer >= _recvBuffer.size()) {
        _recvBuffer.resize(indexInBuffer + 1u);
    }
    else if (_recvBuffer[indexInBuffer].tag != TaggedPacket::WaitingForData) {
        // Already received this - acknowledge and ignore
        _prepareAck(packetOrdinal);
        return;
    }

    while (true) {
        const std::uint32_t ackOrdinal = packet.extractOrThrow<std::uint32_t>();
        if (ackOrdinal == 0u) {
            break;
        }
        _receivedAck(ackOrdinal, true);
    }

    _recvBuffer[indexInBuffer].packet = std::move(packet);
    
    if (packetType == UDP_PACKET_TYPE_DATA) {
        _recvBuffer[indexInBuffer].tag = TaggedPacket::ReadyForUnpacking;
    }
    else if (packetType == UDP_PACKET_TYPE_DATA_MORE) {
        _recvBuffer[indexInBuffer].tag = TaggedPacket::WaitingForMore;
    }
    else if (packetType == UDP_PACKET_TYPE_DATA_TAIL) {
        _recvBuffer[indexInBuffer].tag = TaggedPacket::WaitingForMore_Tail;
    }
    else {
        HARD_ASSERT(false && "_saveDataPacket called with invalid packet type");
    }

    _prepareAck(packetOrdinal);
}


void RN_UdpConnectorImpl::_processHelloPacket(util::Packet& packet) {
    switch (_status) {
    case RN_ConnectorStatus::Connecting:
        throw FatalPacketTypeReceived{"Received HELLO packet (status: Connecting)"};

    case RN_ConnectorStatus::Accepting:
        NO_OP();
        break;

    case RN_ConnectorStatus::Connected:
        // TODO erroneous, nonfatal
        // TODO log event
        break;

    default:
        HARD_ASSERT(0 && "Unreachable");
        break;
    }
}

void RN_UdpConnectorImpl::_processConnectPacket(util::Packet& packet) {
    switch (_status) {
    case RN_ConnectorStatus::Connecting:
    {
        auto receivedPassphrase = packet.extractOrThrow<std::string>();
        const PZInteger receivedClientIndex = packet.extractOrThrow<PZInteger>();
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

    case RN_ConnectorStatus::Connected:
        // TODO erroneous, nonfatal
        // TODO log event
        break;

    default:
        HARD_ASSERT(false && "Unreachable");
        break;
    }
}

void RN_UdpConnectorImpl::_processDisconnectPacket(util::Packet& packet) {
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
        HARD_ASSERT(false && "Unreachable");
        break;
    }
}

void RN_UdpConnectorImpl::_processDataPacket(util::Packet& packet) {
    switch (_status) {
    case RN_ConnectorStatus::Connecting:
        throw FatalPacketTypeReceived{"Received DATA packet (status: Connecting)"};

    case RN_ConnectorStatus::Accepting:       
        _eventFactory.createConnected();
        _startSession(); // New connection confirmed
        SWITCH_FALLTHROUGH;

    case RN_ConnectorStatus::Connected:
        _saveDataPacket(packet, UDP_PACKET_TYPE_DATA);
        break;

    default:
        HARD_ASSERT(false && "Unreachable");
        break;
    }
}

void RN_UdpConnectorImpl::_processDataMorePacket(util::Packet& packet) {
    switch (_status) {
    case RN_ConnectorStatus::Connecting:
        throw FatalPacketTypeReceived{"Received DATA_MORE packet (status: Connecting)"};

    case RN_ConnectorStatus::Accepting:       
        _eventFactory.createConnected();
        _startSession(); // New connection confirmed
        SWITCH_FALLTHROUGH;

    case RN_ConnectorStatus::Connected:
        _saveDataPacket(packet, UDP_PACKET_TYPE_DATA_MORE);
        break;

    default:
        HARD_ASSERT(false && "Unreachable");
        break;
    }
}

void RN_UdpConnectorImpl::_processDataTailPacket(util::Packet& packet) {
    switch (_status) {
    case RN_ConnectorStatus::Connecting:
        throw FatalPacketTypeReceived{"Received DATA_TAIL packet (status: Connecting)"};

    case RN_ConnectorStatus::Accepting:       
        _eventFactory.createConnected();
        _startSession(); // New connection confirmed
        SWITCH_FALLTHROUGH;

    case RN_ConnectorStatus::Connected:
        _saveDataPacket(packet, UDP_PACKET_TYPE_DATA_TAIL);
        break;

    default:
        HARD_ASSERT(false && "Unreachable");
        break;
    }
}

void RN_UdpConnectorImpl::_processAcksPacket(util::Packet& packet) {
    switch (_status) {
    case RN_ConnectorStatus::Connecting:
        throw FatalPacketTypeReceived{"Received ACKS packet (status: Connecting)"};

    case RN_ConnectorStatus::Accepting:
        throw FatalPacketTypeReceived{"Received ACKS packet (status: Accepting)"};

    case RN_ConnectorStatus::Connected:
        while (!packet.endOfPacket()) {
            const std::uint32_t ackOrd = packet.extractOrThrow<std::uint32_t>();
            _receivedAck(ackOrd, false);
        }
        break;

    default:
        HARD_ASSERT(false && "Unreachable");
        break;
    }
}

} // namespace rn
HOBGOBLIN_NAMESPACE_END

#include <Hobgoblin/Private/Pmacro_undef.hpp>
