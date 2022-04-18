
#include "Udp_client_impl.hpp"

#include <Hobgoblin/Common.hpp>

#include <utility>

#include <Hobgoblin/Private/Pmacro_define.hpp>

HOBGOBLIN_NAMESPACE_BEGIN
namespace rn {

RN_UdpClientImpl::RN_UdpClientImpl(std::string aPassphrase,
                                   RN_NetworkingStack aNetworkingStack,
                                   PZInteger aMaxPacketSize)
    : _socket{RN_Protocol::UDP, aNetworkingStack}
    , _maxPacketSize{aMaxPacketSize}
    , _connector{ _socket
                , _timeoutLimit
                , _passphrase
                , _retransmitPredicate
                , detail::EventFactory{_eventQueue}
                , _maxPacketSize 
                }
    , _passphrase{std::move(aPassphrase)}
    , _retransmitPredicate{RN_DefaultRetransmitPredicate}
{
    _socket.init(_maxPacketSize);
}

RN_UdpClientImpl::~RN_UdpClientImpl() {
    disconnect(false);
}

///////////////////////////////////////////////////////////////////////////
// CLIENT CONTROL                                                        //
///////////////////////////////////////////////////////////////////////////

void RN_UdpClientImpl::connect(std::uint16_t localPort, sf::IpAddress serverIp, std::uint16_t serverPort) {
    HARD_ASSERT(!_running || _connector.getStatus() == RN_ConnectorStatus::Disconnected);
    
    _socket.bind(sf::IpAddress::Any, localPort);
    _connector.connect(serverIp, serverPort);
    _running = true;
}

void RN_UdpClientImpl::connectLocal(RN_ServerInterface& server) {
    HARD_ASSERT(!_running || _connector.getStatus() == RN_ConnectorStatus::Disconnected);
    _connector.connectLocal(server);
    _running = true;
}

void RN_UdpClientImpl::disconnect(bool notifyRemote) {
    if (_connector.getStatus() != RN_ConnectorStatus::Disconnected) {
        _connector.disconnect(notifyRemote);
    }
    _running = false;

    // Safe to call multiple times
    _socket.close();
}

void RN_UdpClientImpl::setTimeoutLimit(std::chrono::microseconds limit) {
    _timeoutLimit = limit;
}

void RN_UdpClientImpl::setRetransmitPredicate(RN_RetransmitPredicate pred) {
    _retransmitPredicate = pred;
}

void RN_UdpClientImpl::update(RN_UpdateMode mode) {
    if (!_running) {
        return;
    }
    else if (_connector.getStatus() == RN_ConnectorStatus::Disconnected) {
        _running = false;
        return;
    }

    switch (mode) {
    case RN_UpdateMode::Receive:
        _updateReceive();
        break;

    case RN_UpdateMode::Send:
        _updateSend();
        break;

    default:
        HARD_ASSERT(false && "Unreachable");
        break;
    }
}

bool RN_UdpClientImpl::pollEvent(RN_Event& ev) {
    if (_eventQueue.empty()) {
        return false;
    }
    ev = _eventQueue.front();
    _eventQueue.pop_front();
    return true;
}

///////////////////////////////////////////////////////////////////////////
// STATE INSPECTION                                                      //
///////////////////////////////////////////////////////////////////////////

bool RN_UdpClientImpl::isRunning() const {
    return _running;
}

const std::string& RN_UdpClientImpl::getPassphrase() const {
    return _passphrase;
}

std::uint16_t RN_UdpClientImpl::getLocalPort() const {
    return _socket.getLocalPort();
}

std::chrono::microseconds RN_UdpClientImpl::getTimeoutLimit() const {
    return _timeoutLimit;
}

const RN_ConnectorInterface& RN_UdpClientImpl::getServerConnector() const {
    return _connector;
}
PZInteger RN_UdpClientImpl::getClientIndex() const {
    HARD_ASSERT(_running && _connector.getStatus() == RN_ConnectorStatus::Connected);
    return *_connector.getClientIndex();
}

bool RN_UdpClientImpl::isServer() const noexcept {
    return false;
}

RN_Protocol RN_UdpClientImpl::getProtocol() const noexcept {
    return _socket.getProtocol();
}

RN_NetworkingStack RN_UdpClientImpl::getNetworkingStack() const noexcept {
    return _socket.getNetworkingStack();
}

///////////////////////////////////////////////////////////////////////////
// PRIVATE IMPLEMENTATION                                                //
///////////////////////////////////////////////////////////////////////////

void RN_UdpClientImpl::_updateReceive() {
    util::Packet packet;
    sf::IpAddress senderIp;
    std::uint16_t senderPort;

    // When we connect the client locally, we don't initialize its 
    // socket so we must not try to use it
    bool keepReceiving = !_connector.isConnectedLocally();

    _connector.prepToReceive();
    while (keepReceiving) {
        switch (_socket.recv(packet, senderIp, senderPort)) {
        case decltype(_socket)::Status::OK:
            if (senderIp == _connector.getRemoteInfo().ipAddress
                && senderPort == _connector.getRemoteInfo().port) {
                _connector.receivedPacket(packet);
            }
            else {
                // handlePacketFromUnknownSender(senderIp, senderPort, packet); TODO
            }
            packet.clear();
            break;

        case decltype(_socket)::Status::NotReady:
            // Nothing left to receive for now
            keepReceiving = false;
            break;

        case decltype(_socket)::Status::Disconnected:
            // Normally we wouldn't expect to get this status from UDP sockets. However,
            // in case it does somehow occur, it should be safe to ignore.
            NO_OP();
            break;

        default:
            // Realistically these won't ever happen
            HARD_ASSERT(false && "Unreachable");
        }
    }

    if (_connector.getStatus() == RN_ConnectorStatus::Connected) {
        _connector.receivingFinished();
        _connector.sendAcks();
    }
    if (_connector.getStatus() != RN_ConnectorStatus::Disconnected) {
        _connector.handleDataMessages(SELF, /* reference to pointer -> */ _currentPacket);
    }
    if (_connector.getStatus() != RN_ConnectorStatus::Disconnected) {
        _connector.checkForTimeout();
    }
}

void RN_UdpClientImpl::_updateSend() {
    _connector.send();
}

void RN_UdpClientImpl::_compose(int receiver, const void* data, std::size_t sizeInBytes) {
    if (_connector.getStatus() != RN_ConnectorStatus::Connected) {
        throw TracedLogicError("Client is not connected; cannot compose messages");
    }
    _connector.appendToNextOutgoingPacket(data, sizeInBytes);
}

void RN_UdpClientImpl::_compose(RN_ComposeForAllType receiver, const void* data, std::size_t sizeInBytes) {
    if (_connector.getStatus() != RN_ConnectorStatus::Connected) {
        return;
    }
    _connector.appendToNextOutgoingPacket(data, sizeInBytes);
}

util::Packet* RN_UdpClientImpl::_getCurrentPacket() {
    return _currentPacket;
}

void RN_UdpClientImpl::_setUserData(util::AnyPtr userData) {
    _userData = userData;
}

util::AnyPtr RN_UdpClientImpl::_getUserData() const {
    return _userData;
}

} // namespace rn
HOBGOBLIN_NAMESPACE_END

#include <Hobgoblin/Private/Pmacro_undef.hpp>
