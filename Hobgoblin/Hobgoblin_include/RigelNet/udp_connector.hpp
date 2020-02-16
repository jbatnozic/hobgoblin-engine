#ifndef UHOBGOBLIN_RN_UDP_CONNECTOR_HPP
#define UHOBGOBLIN_RN_UDP_CONNECTOR_HPP

#include <Hobgoblin_include/common.hpp>
#include <Hobgoblin_include/RigelNet/node.hpp>
#include <Hobgoblin_include/RigelNet/packet.hpp>
#include <Hobgoblin_include/RigelNet/remote_info.hpp>

#include <SFML/System/Clock.hpp>
#include <SFML/Network.hpp>

#include <chrono>
#include <cstdint>

#include <Hobgoblin_include/Private/pmacro_define.hpp>

HOBGOBLIN_NAMESPACE_START
namespace rn {
namespace detail {

struct TaggedPacket {
    RN_Packet packet;
    sf::Clock clock;
    bool acknowledged = false;
};

class RN_UdpConnector {
public:
    RN_UdpConnector(sf::UdpSocket& socket, const std::string& passphrase);

    void tryAccept(sf::IpAddress addr, std::uint16_t port, RN_Packet& packet);
    void connect(sf::IpAddress addr, std::uint16_t port);
    void reset(bool notifyRemoteOnDisconnect);


    void update(RN_Node& node, PZInteger slotIndex, bool doUpload);
    void receivedPacket(RN_Packet& packet);

    const RN_RemoteInfo& getRemoteInfo() const noexcept;

private:
    enum class State {
        Disconnected,
        Accepting,
        Connecting,
        Connected
    };

    RN_RemoteInfo _remoteInfo;
    std::chrono::microseconds _timeoutLimit = std::chrono::microseconds{0};
    sf::UdpSocket& _socket;
    const std::string& _passphrase;
    State _state;

    std::deque<TaggedPacket> _sendBuffer;
    std::deque<TaggedPacket> _recvBuffer;
    std::uint32_t _sendBufferHeadIndex;
    std::uint32_t _recvBufferHeadIndex;

    std::vector<std::uint32_t> _ackOrdinals;

    // TODO Ping stuff

    void execAccepting(RN_Node& node, PZInteger slotIndex, bool doUpload);
    void execConnecting(RN_Node& node, PZInteger slotIndex, bool doUpload);
    void execConnected(RN_Node& node, PZInteger slotIndex, bool doUpload);

    bool connectionTimedOut() const;
    void uploadAllData();
    void prepareAck(std::uint32_t ordinal);
    void receivedAck(std::uint32_t ordinal);
    void initializeSession();
};

} // namespace detail
} // namespace rn
HOBGOBLIN_NAMESPACE_END

#include <Hobgoblin_include/Private/pmacro_undef.hpp>

#endif // !UHOBGOBLIN_RN_UDP_CONNECTOR_HPP