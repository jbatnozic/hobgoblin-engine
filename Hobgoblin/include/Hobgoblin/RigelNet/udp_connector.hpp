#ifndef UHOBGOBLIN_RN_UDP_CONNECTOR_HPP
#define UHOBGOBLIN_RN_UDP_CONNECTOR_HPP

#include <Hobgoblin/common.hpp>
#include <Hobgoblin/RigelNet/node.hpp>
#include <Hobgoblin/RigelNet/packet.hpp>
#include <Hobgoblin/RigelNet/remote_info.hpp>

#include <Hobgoblin/Utility/stopwatch.hpp>

#include <SFML/System/Clock.hpp>
#include <SFML/Network.hpp>

#include <chrono>
#include <cstdint>

#include <Hobgoblin/Private/pmacro_define.hpp>

HOBGOBLIN_NAMESPACE_START
namespace rn {

enum class RN_ConnectorStatus {
    Disconnected,
    Accepting,
    Connecting,
    Connected
};

namespace detail {

struct TaggedPacket {
    enum Tag {
        DefaultTag,
    // SEND:
        ReadyForSending = DefaultTag,
        NotAcknowledged,
        Acknowledged,
    // RECV:
        WaitingForData = DefaultTag,
        ReadyForUnpacking,
        Unpacked,
    };

    RN_Packet packet;
    util::Stopwatch stopwatch;
    Tag tag = DefaultTag;
};

class RN_UdpConnector {
public:
    RN_UdpConnector(sf::UdpSocket& socket, const std::string& passphrase);

    void tryAccept(sf::IpAddress addr, std::uint16_t port, RN_Packet& packet);
    void connect(sf::IpAddress addr, std::uint16_t port);
    void disconnect(bool notfiyRemote);

    void update(RN_Node& node, PZInteger slotIndex, bool doUpload);
    void receivedPacket(RN_Packet& packet);
    void handleDataMessages(RN_Node& node);

    const RN_RemoteInfo& getRemoteInfo() const noexcept;
    RN_ConnectorStatus getStatus() const noexcept;

    void appendToNextOutgoingPacket(const void *data, std::size_t sizeInBytes);

private:
    RN_RemoteInfo _remoteInfo;
    std::chrono::microseconds _timeoutLimit = std::chrono::microseconds{0};
    sf::UdpSocket& _socket;
    const std::string& _passphrase;
    RN_ConnectorStatus _status;

    std::deque<TaggedPacket> _sendBuffer;
    std::deque<TaggedPacket> _recvBuffer;
    std::uint32_t _sendBufferHeadIndex;
    std::uint32_t _recvBufferHeadIndex;

    std::vector<std::uint32_t> _ackOrdinals;

    // TODO Ping stuff

    void cleanup();
    void reset();
    bool connectionTimedOut() const;
    void uploadAllData();
    void prepareAck(std::uint32_t ordinal);
    void receivedAck(std::uint32_t ordinal);
    void initializeSession();
    void prepareNextOutgoingPacket();
    void receiveDataMessage(RN_Packet& packet);
};

} // namespace detail
} // namespace rn
HOBGOBLIN_NAMESPACE_END

#include <Hobgoblin/Private/pmacro_undef.hpp>

#endif // !UHOBGOBLIN_RN_UDP_CONNECTOR_HPP