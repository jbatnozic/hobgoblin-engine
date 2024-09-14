// Copyright 2024 Jovan Batnozic. Released under MS-PL licence in Serbia.
// See https://github.com/jbatnozic/Hobgoblin?tab=readme-ov-file#licence

// clang-format off


#include "Udp_server_impl.hpp"

#include <Hobgoblin/Common.hpp>
#include <Hobgoblin/HGExcept.hpp>

#include <cassert>
#include <utility>

#include <Hobgoblin/Private/Pmacro_define.hpp>

HOBGOBLIN_NAMESPACE_BEGIN
namespace rn {

namespace {
constexpr auto UDP_HEADER_BYTE_COUNT = 8u;
} // namespace

RN_UdpServerImpl::RN_UdpServerImpl(std::string passphrase,
                                   PZInteger size,
                                   RN_NetworkingStack networkingStack,
                                   PZInteger aMaxPacketSize)
    : _socket{RN_Protocol::UDP, networkingStack}
    , _maxPacketSize{aMaxPacketSize}
    , _passphrase{std::move(passphrase)}
    , _retransmitPredicate{RN_DefaultRetransmitPredicate}
{
    _socket.init(_maxPacketSize);

    _clients.reserve(static_cast<std::size_t>(size));
    for (PZInteger i = 0; i < size; i += 1) {
        auto connector = std::make_unique<RN_UdpConnectorImpl>(
            _socket,
            _timeoutLimit,
            _passphrase,
            _retransmitPredicate,
            rn_detail::EventFactory{_eventListeners, i},
            _maxPacketSize);

        _clients.push_back(std::move(connector));
    }
}

RN_UdpServerImpl::~RN_UdpServerImpl() {
    stop();
}

///////////////////////////////////////////////////////////////////////////
// SERVER CONTROL                                                        //
///////////////////////////////////////////////////////////////////////////

void RN_UdpServerImpl::start(std::uint16_t localPort) {
    HG_VALIDATE_PRECONDITION(_running == false);

    _socket.bind(sf::IpAddress::Any, localPort);

    _running = true;
}

void RN_UdpServerImpl::stop(bool aNotifyClients) {
    for (auto& client : _clients) {
        if (client->getStatus() != RN_ConnectorStatus::Disconnected) {
            client->disconnect(aNotifyClients, "Server shutting down.");
        }
    }

    // Safe to call multiple times
    _socket.close();
}

void RN_UdpServerImpl::resize(PZInteger newSize) {
    if (newSize == getSize()) {
        return;
    }

    if (newSize < stopz(_clients.size())) {
        HG_NOT_IMPLEMENTED("Server downsizing not currently supported!");
    }

    PZInteger i = stopz(_clients.size());
    while (i < newSize) {
        auto connector = std::make_unique<RN_UdpConnectorImpl>(
            _socket,
            _timeoutLimit,
            _passphrase,
            _retransmitPredicate,
            rn_detail::EventFactory{_eventListeners, i},
            _maxPacketSize);

        _clients.push_back(std::move(connector));
        i += 1;
    }
}

void RN_UdpServerImpl::setTimeoutLimit(std::chrono::microseconds limit) {
    _timeoutLimit = limit;
}

void RN_UdpServerImpl::setRetransmitPredicate(RN_RetransmitPredicate pred) {
    _retransmitPredicate = pred;
}

RN_Telemetry RN_UdpServerImpl::update(RN_UpdateMode mode) {
    if (!_running) {
        return {};
    }

    switch (mode) {
    case RN_UpdateMode::Receive:
        return _updateReceive();

    case RN_UpdateMode::Send:
        return _updateSend();

    default:
        HG_UNREACHABLE();
        break;
    }
}

void RN_UdpServerImpl::addEventListener(NeverNull<RN_EventListener*> aEventListener) {
    _addEventListener(aEventListener);
}

void RN_UdpServerImpl::removeEventListener(NeverNull<RN_EventListener*> aEventListener) {
    _removeEventListener(aEventListener);
}

///////////////////////////////////////////////////////////////////////////
// CLIENT MANAGEMENT                                                     //
///////////////////////////////////////////////////////////////////////////

const RN_ConnectorInterface& RN_UdpServerImpl::getClientConnector(PZInteger clientIndex) const {
    return *(_clients.at(clientIndex));
}

RN_ConnectorInterface& RN_UdpServerImpl::getClientConnector(PZInteger clientIndex) {
    return *(_clients.at(clientIndex));
}

void RN_UdpServerImpl::kickClient(PZInteger          aClientIndex,
                                  bool               aNotifyRemote,
                                  const std::string& aMessage) {
    getClientConnector(aClientIndex).disconnect(aNotifyRemote, aMessage);
}

///////////////////////////////////////////////////////////////////////////
// STATE INSPECTION                                                      //
///////////////////////////////////////////////////////////////////////////

bool RN_UdpServerImpl::isRunning() const {
    return _running;
}

PZInteger RN_UdpServerImpl::getSize() const {
    return static_cast<PZInteger>(_clients.size());
}

const std::string& RN_UdpServerImpl::getPassphrase() const {
    return _passphrase;
}

std::chrono::microseconds RN_UdpServerImpl::getTimeoutLimit() const {
    return _timeoutLimit;
}

std::uint16_t RN_UdpServerImpl::getLocalPort() const {
    return _socket.getLocalPort();
}

int RN_UdpServerImpl::getSenderIndex() const {
    return _senderIndex;
}

bool RN_UdpServerImpl::isServer() const noexcept {
    return true;
}

RN_Protocol RN_UdpServerImpl::getProtocol() const noexcept {
    return _socket.getProtocol();
}

RN_NetworkingStack RN_UdpServerImpl::getNetworkingStack() const noexcept {
    return _socket.getNetworkingStack();
}

///////////////////////////////////////////////////////////////////////////
// NOT INHERITED                                                         //
///////////////////////////////////////////////////////////////////////////

int RN_UdpServerImpl::acceptLocalConnection(RN_UdpConnectorImpl& localPeer, 
                                            const std::string& passphrase) {
    HG_VALIDATE_PRECONDITION(isRunning());

    for (std::size_t i = 0; i < _clients.size(); i += 1) {
        if (_clients[i]->getStatus() == RN_ConnectorStatus::Disconnected) {
            if (_clients[i]->tryAcceptLocal(localPeer, passphrase)) {
                return static_cast<int>(i);
            }
        }
    }

    return -1;
}

///////////////////////////////////////////////////////////////////////////
// PRIVATE IMPLEMENTATION                                                //
///////////////////////////////////////////////////////////////////////////

RN_Telemetry RN_UdpServerImpl::_updateReceive() {
    RN_Telemetry telemetry;
    util::Packet packet;
    sf::IpAddress senderIp;
    std::uint16_t senderPort;

    for (auto& client : _clients) {
        client->prepToReceive();
    }

    bool keepReceiving = true;
    while (keepReceiving) {
        switch (_socket.recv(packet, senderIp, senderPort)) {
        case decltype(_socket)::Status::OK:
            {
                telemetry.downloadByteCount += stopz(packet.getDataSize() + UDP_HEADER_BYTE_COUNT);
                const int senderConnectorIndex = _findConnector(senderIp, senderPort);

                if (senderConnectorIndex != -1) {
                    _senderIndex = senderConnectorIndex;
                    _clients[senderConnectorIndex]->receivedPacket(packet);
                }
                else {
                    _handlePacketFromUnknownSender(senderIp, senderPort, packet);
                }

                packet.clear();
            }
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
            HG_UNREACHABLE();
        }
    }

    for (PZInteger i = 0; i < getSize(); i += 1) {
        auto& client = _clients[i];
        
        if (client->getStatus() == RN_ConnectorStatus::Connected) {
            client->receivingFinished();
            telemetry += client->sendAcks();
        }
        if (client->getStatus() != RN_ConnectorStatus::Disconnected) {
            _senderIndex = i;
            client->handleDataMessages(SELF, &_currentPacket);
        }
        if (client->getStatus() != RN_ConnectorStatus::Disconnected) {
            client->checkForTimeout();
        }
    }
    _senderIndex = -1;

    return telemetry;
}

RN_Telemetry RN_UdpServerImpl::_updateSend() {
    RN_Telemetry telemetry;
    for (auto& client : _clients) {
        if (client->getStatus() == RN_ConnectorStatus::Disconnected) {
            continue;
        }
        telemetry += client->send();
    }
    return telemetry;
}

int RN_UdpServerImpl::_findConnector(sf::IpAddress addr, std::uint16_t port) const {
    for (int i = 0; i < getSize(); i += 1) {
        const auto& remote = getClientConnector(i).getRemoteInfo();
        if (remote.port == port && remote.ipAddress == addr) {
            return i;
        }
    }
    return -1;
}

void RN_UdpServerImpl::_handlePacketFromUnknownSender(sf::IpAddress senderIp, 
                                                      std::uint16_t senderPort, 
                                                      util::Packet& packet) {
    for (PZInteger i = 0; i < getSize(); i += 1) {
        auto& connector = _clients[i];
        if (connector->getStatus() == RN_ConnectorStatus::Disconnected) {
            connector->setClientIndex(i);
            if (!connector->tryAccept(senderIp, senderPort, packet)) {
                // TODO Notify of error
            }
            return;
        }
    }
    // TODO Send disconnect message (no room left)
}

void RN_UdpServerImpl::_compose(RN_ComposeForAllType receiver, const void* data, std::size_t sizeInBytes) {
    for (auto& client : _clients) {
        if (client->getStatus() == RN_ConnectorStatus::Connected) {
            client->appendToNextOutgoingPacket(data, sizeInBytes);
        }
    }
}

void RN_UdpServerImpl::_compose(PZInteger receiver, const void* data, std::size_t sizeInBytes) {
    if (_clients[receiver]->getStatus() != RN_ConnectorStatus::Connected) {
        HG_THROW_TRACED(TracedLogicError, 0, "Cannot compose messages to clients that are not connected.");
    }
    _clients[receiver]->appendToNextOutgoingPacket(data, sizeInBytes);
}

util::Packet* RN_UdpServerImpl::_getCurrentPacket() {
    return _currentPacket;
}

void RN_UdpServerImpl::_setUserData(util::AnyPtr userData) {
    _userData = userData;
}

util::AnyPtr RN_UdpServerImpl::_getUserData() const {
    return _userData;
}

} // namespace rn
HOBGOBLIN_NAMESPACE_END

#include <Hobgoblin/Private/Pmacro_undef.hpp>

// clang-format on
