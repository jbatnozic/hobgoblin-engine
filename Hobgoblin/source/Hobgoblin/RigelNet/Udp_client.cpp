
#include <Hobgoblin/RigelNet/Udp_client.hpp>

#include <cassert>
#include <utility>

#include <Hobgoblin/Private/Pmacro_define.hpp>

HOBGOBLIN_NAMESPACE_START
namespace rn {

RN_UdpClient::RN_UdpClient()
    : _connector{_mySocket, _passphrase} 
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
    auto status = _mySocket.bind(localPort);
    assert(status == sf::Socket::Done); // TODO - Throw exception on failure
    _passphrase = std::move(passphrase);
    _connector.connect(serverIp, serverPort);
}

void RN_UdpClient::update() {
    update(true);
}

void RN_UdpClient::updateWithoutUpload() {
    update(false);
}

// Protected

void RN_UdpClient::compose(int receiver, const void* data, std::size_t sizeInBytes) {
    // TODO Temp.
    _connector.appendToNextOutgoingPacket(data, sizeInBytes);
}

// Private

void RN_UdpClient::update(bool doUpload) {
    /*if (!_running) {
        return; // TODO
    }*/

    if (!_eventQueue.empty()) {
        // TODO Error
    }

    if (true /*client->connected()*/) {
        // ping
        // send uord
    }
    _connector.update(Self, 0, true); // TODO
    
    download();
}

void RN_UdpClient::download() {
    RN_Packet packet;
    sf::IpAddress senderIp;
    std::uint16_t senderPort;

    while (_mySocket.receive(packet, senderIp, senderPort) == sf::Socket::Status::Done) {
        if (senderIp == _connector.getRemoteInfo().ipAddress
            && senderPort == _connector.getRemoteInfo().port) {
            _connector.receivedPacket(packet);
        }
        else {
            // handlePacketFromUnknownSender(senderIp, senderPort, packet); TODO
        }
        packet.clear();
    }

    _connector.handleDataMessages(Self);
}

} // namespace rn
HOBGOBLIN_NAMESPACE_END

#include <Hobgoblin/Private/Pmacro_undef.hpp>
