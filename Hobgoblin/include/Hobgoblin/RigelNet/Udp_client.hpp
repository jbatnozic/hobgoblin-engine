#ifndef UHOBGOBLIN_RN_UDP_CLIENT_HPP
#define UHOBGOBLIN_RN_UDP_CLIENT_HPP

#include <Hobgoblin/RigelNet/Node.hpp>
#include <Hobgoblin/RigelNet/Remote_info.hpp>
#include <Hobgoblin/RigelNet/Udp_connector.hpp>
#include <Hobgoblin/Utility/NoCopyNoMove.hpp>

#include <SFML/Network.hpp>

#include <chrono>

#include <Hobgoblin/Private/Pmacro_define.hpp>

HOBGOBLIN_NAMESPACE_START
namespace rn {

class RN_UdpClient : public RN_Node {
public:
    RN_UdpClient();
    RN_UdpClient(std::uint16_t localPort, sf::IpAddress serverIp, std::uint16_t serverPort, std::string passphrase);

    ~RN_UdpClient();

    void connect(std::uint16_t localPort, sf::IpAddress serverIp, std::uint16_t serverPort, std::string passphrase);
    void disconnect();
    bool isConnected() const;
    void update();
    void updateWithoutUpload();

    // Utility:
    const RN_RemoteInfo& getServerInfo() const;
    RN_ConnectorStatus getConnectorStatus(PZInteger index) const;

    std::chrono::microseconds getTimeoutLimit() const;
    void setTimeoutLimit(std::chrono::microseconds limit);

    const std::string& getPassphrase() const;

protected:
    void compose(int receiver, const void* data, std::size_t sizeInBytes) override;

private:
    detail::RN_UdpConnector _connector;
    sf::UdpSocket _mySocket;
    std::string _passphrase;
    std::chrono::microseconds _timeoutLimit = std::chrono::microseconds{0};
    bool _running = false; // TODO Update this value

    void update(bool doUpload);
    void download();
};

} // namespace rn
HOBGOBLIN_NAMESPACE_END

#include <Hobgoblin/Private/Pmacro_undef.hpp>

#endif // !UHOBGOBLIN_RN_UDP_CLIENT_HPP
