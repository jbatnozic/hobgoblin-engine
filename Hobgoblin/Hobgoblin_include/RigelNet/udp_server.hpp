#ifndef UHOBGOBLIN_RN_UDP_SERVER_HPP
#define UHOBGOBLIN_RN_UDP_SERVER_HPP

#include <Hobgoblin_include/RigelNet/node.hpp>
#include <Hobgoblin_include/RigelNet/remote_info.hpp>
#include <Hobgoblin_include/RigelNet/udp_connector.hpp>
#include <Hobgoblin_include/Utility/NoCopyNoMove.hpp>

#include <SFML/Network.hpp>

#include <chrono>
#include <vector>

#include <Hobgoblin_include/Private/pmacro_define.hpp>

HOBGOBLIN_NAMESPACE_START
namespace rn {

class RN_UdpServer : public RN_Node, NO_COPY, NO_MOVE {
public:
    RN_UdpServer();
    RN_UdpServer(PZInteger size);
    RN_UdpServer(PZInteger size, std::uint16_t port, std::string passphrase);

    ~RN_UdpServer();

    void start(std::uint16_t localPort, std::string passphrase);
    void stop();
    bool isRunning() const;
    void update();
    void updateWithoutUpload();

    // Client management:
    const RN_RemoteInfo& getClientInfo(PZInteger index) const;
    void swapClients(PZInteger index1, PZInteger index2);

    // Utility:
    int getSenderIndex() const;
    std::uint16_t getLocalPort() const;
    PZInteger getSize() const;
    void resize(PZInteger newSize);

    std::chrono::microseconds getTimeoutLimit() const;
    void setTimeoutLimit(std::chrono::microseconds limit);

    const std::string& getPassphrase() const;

private:
    std::vector<detail::RN_UdpConnector> _clients;
    sf::UdpSocket _mySocket;
    std::string _passphrase;
    std::chrono::microseconds _timeoutLimit;
    int _senderIndex;
    std::uint32_t _updateOrdinal; // ???????
    bool _running;

    void update(bool doUpload);
    void download();
    void upload();
    int findConnector(sf::IpAddress addr, std::uint16_t port) const;
    void handlePacketFromUnknownSender(sf::IpAddress senderIp, std::uint16_t senderPort, RN_Packet& packet);
};

} // namespace rn
HOBGOBLIN_NAMESPACE_END

#include <Hobgoblin_include/Private/pmacro_undef.hpp>

#endif // !UHOBGOBLIN_RN_UDP_SERVER_HPP