
#include <Hobgoblin_include/RigelNet/packet.hpp>
#include <Hobgoblin_include/RigelNet/udp_server.hpp>

#include <cassert>
#include <utility>

#include <Hobgoblin_include/Private/pmacro_define.hpp>

HOBGOBLIN_NAMESPACE_START
namespace rn {

RN_UdpServer::RN_UdpServer(PZInteger size) {
    _clients.reserve(static_cast<std::size_t>(size));
    for (PZInteger i = 0; i < size; i += 1) {
        _clients.emplace_back(_mySocket, _passphrase);
    }

    _mySocket.setBlocking(false);
}

RN_UdpServer::RN_UdpServer()
    : RN_UdpServer{1}
{
}

RN_UdpServer::RN_UdpServer(PZInteger size, std::uint16_t port, std::string passphrase)
    : RN_UdpServer{size} 
{
    start(port, std::move(passphrase));
}

RN_UdpServer::~RN_UdpServer() {
    // TODO
}

void RN_UdpServer::start(std::uint16_t localPort, std::string passphrase) {
    auto status = _mySocket.bind(localPort);
    assert(status == sf::Socket::Done); // TODO - Throw exception on failure
    _passphrase = std::move(passphrase);
    _running = true;
}

void RN_UdpServer::stop() {
    // TODO
}

bool RN_UdpServer::isRunning() const {
    return _running;
}

void RN_UdpServer::update() {
    update(true);
}

void RN_UdpServer::updateWithoutUpload() {
    update(false);
}

// Client management:
const RN_RemoteInfo& RN_UdpServer::getClientInfo(PZInteger index) const {
    return _clients[index].getRemoteInfo();
}

void RN_UdpServer::swapClients(PZInteger index1, PZInteger index2) {
    // TODO
}

// Utility:
int RN_UdpServer::getSenderIndex() const {
    return _senderIndex;
}

std::uint16_t RN_UdpServer::getLocalPort() const {
    return _mySocket.getLocalPort();
}

PZInteger RN_UdpServer::getSize() const {
    return static_cast<PZInteger>(_clients.size());
}

void RN_UdpServer::resize(PZInteger newSize) {

}

std::chrono::microseconds RN_UdpServer::getTimeoutLimit() const {
    return _timeoutLimit;
}

void RN_UdpServer::setTimeoutLimit(std::chrono::microseconds limit) {
    _timeoutLimit = limit;
}

const std::string& RN_UdpServer::getPassphrase() const {
    return _passphrase;
}

// Private:
void RN_UdpServer::update(bool doUpload) {
    if (!_running) {
        return;
    }

    if (!_eventQueue.empty()) {
        // TODO Error
    }

    for (auto& client : _clients) {
        if (true /*client->connected()*/) {
            // ping
            // send uord
        }
        client.update(Self, 0, true); // TODO
    }

    download();
}

void RN_UdpServer::download() {
    RN_Packet packet;
    sf::IpAddress senderIp;
    std::uint16_t senderPort;

    while (_mySocket.receive(packet, senderIp, senderPort) == sf::Socket::Status::Done) {
        const int senderConnectorIndex = findConnector(senderIp, senderPort);

        if (senderConnectorIndex != -1) {
            _senderIndex = senderConnectorIndex;
            _clients[senderConnectorIndex].receivedPacket(packet);
        }
        else {
            handlePacketFromUnknownSender(senderIp, senderPort, packet);
        }

        packet.clear();
    }

    _senderIndex = -1;
}

int RN_UdpServer::findConnector(sf::IpAddress addr, std::uint16_t port) const {
    for (int i = 0; i < getSize(); i += 1) {
        const auto& conn = getClientInfo(i);
        if (conn.port == port && conn.ipAddress == addr) {
            return i;
        }
    }
    return -1;
}

void RN_UdpServer::handlePacketFromUnknownSender(sf::IpAddress senderIp, std::uint16_t senderPort, RN_Packet& packet) {
    for (auto& connector : _clients) {
        if (connector.getRemoteInfo().status == RN_RemoteStatus::Disconnected) {
            connector.tryAccept(senderIp, senderPort, packet);
        }
        break;
    }
    // TODO Send disconnect message (no room left)
}

} // namespace rn
HOBGOBLIN_NAMESPACE_END

#include <Hobgoblin_include/Private/pmacro_undef.hpp>