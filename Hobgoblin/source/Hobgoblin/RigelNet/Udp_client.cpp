
#include <Hobgoblin/RigelNet/Udp_client.hpp>

#include <cassert>
#include <utility>

#include <Hobgoblin/Private/Pmacro_define.hpp>

HOBGOBLIN_NAMESPACE_START
namespace rn {

RN_UdpClient::RN_UdpClient()
    : RN_Client<RN_UdpClient, detail::RN_UdpConnector>{RN_NodeType::UdpClient}
    , _connector{_mySocket, _timeoutLimit, _passphrase, _retransmitPredicate, detail::EventFactory{SELF}}
    , _retransmitPredicate{DefaultRetransmitPredicate}
{
    _mySocket.setBlocking(false);
}

RN_UdpClient::RN_UdpClient(std::uint16_t localPort, sf::IpAddress serverIp, std::uint16_t serverPort, std::string passphrase)
    : RN_UdpClient()
{
    connect(localPort, serverIp, serverPort, std::move(passphrase));
}

RN_UdpClient::~RN_UdpClient() {
    // TODO
}

void RN_UdpClient::connect(std::uint16_t localPort, sf::IpAddress serverIp, std::uint16_t serverPort, std::string passphrase) {
    assert(!_running || _connector.getStatus() == RN_ConnectorStatus::Disconnected);
    
    auto status = _mySocket.bind(localPort);
    assert(status == sf::Socket::Done); // TODO - Throw exception on failure
    _passphrase = std::move(passphrase);
    _connector.connect(serverIp, serverPort);
    _running = true;
}

void RN_UdpClient::disconnect(bool notifyRemote) {
    _connector.disconnect(notifyRemote);
    _running = false;
}

void RN_UdpClient::update(RN_UpdateMode mode) {
    if (!_running) {
        return;
    }
    else if (_connector.getStatus() == RN_ConnectorStatus::Disconnected) {
        _running = false;
        return;
    }

    switch (mode) {
    case RN_UpdateMode::Receive:
        updateReceive();
        break;

    case RN_UpdateMode::Send:
        updateSend();
        break;

    default:
        assert(0 && "Unreachable");
        break;
    }
}

const detail::RN_UdpConnector& RN_UdpClient::getServer() const {
    return _connector;
}

PZInteger RN_UdpClient::getClientIndex() const {
    assert(_running && _connector.getStatus() == RN_ConnectorStatus::Connected);
    return *_connector.getClientIndex();
}

std::chrono::microseconds RN_UdpClient::getTimeoutLimit() const {
    return _timeoutLimit;
}

void RN_UdpClient::setTimeoutLimit(std::chrono::microseconds limit) {
    _timeoutLimit = limit;
}

void RN_UdpClient::setRetransmitPredicate(RetransmitPredicate pred) {
    _retransmitPredicate = pred;
}

const std::string& RN_UdpClient::getPassphrase() const {
    return _passphrase;
}

// Protected

void RN_UdpClient::compose(int receiver, const void* data, std::size_t sizeInBytes) {
    _connector.appendToNextOutgoingPacket(data, sizeInBytes);
}

void RN_UdpClient::compose(RN_ComposeForAllType receiver, const void* data, std::size_t sizeInBytes) {
    _connector.appendToNextOutgoingPacket(data, sizeInBytes);
}

// Private

void RN_UdpClient::updateReceive() {
    detail::RN_PacketWrapper packetWrap;
    sf::IpAddress senderIp;
    std::uint16_t senderPort;

    while (_mySocket.receive(packetWrap.packet, senderIp, senderPort) == sf::Socket::Status::Done) {
        if (senderIp == _connector.getRemoteInfo().ipAddress
            && senderPort == _connector.getRemoteInfo().port) {
            _connector.receivedPacket(packetWrap, true);
        }
        else {
            // handlePacketFromUnknownSender(senderIp, senderPort, packet); TODO
        }
        packetWrap.packet.clear();
    }

    if (_connector.getStatus() == RN_ConnectorStatus::Connected) {
        _connector.sendAcks();
    }
    if (_connector.getStatus() != RN_ConnectorStatus::Disconnected) {
        _connector.handleDataMessages(SELF);
    }
    if (_connector.getStatus() != RN_ConnectorStatus::Disconnected) {
        _connector.checkForTimeout();
    }
}

void RN_UdpClient::updateSend() {
    _connector.send(SELF);
}

} // namespace rn
HOBGOBLIN_NAMESPACE_END

#include <Hobgoblin/Private/Pmacro_undef.hpp>
