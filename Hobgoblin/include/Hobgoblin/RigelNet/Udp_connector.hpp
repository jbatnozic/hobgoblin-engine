#ifndef UHOBGOBLIN_RN_UDP_CONNECTOR_HPP
#define UHOBGOBLIN_RN_UDP_CONNECTOR_HPP

#include <Hobgoblin/Common.hpp>
#include <Hobgoblin/RigelNet/Connector.hpp>
#include <Hobgoblin/RigelNet/Node.hpp>
#include <Hobgoblin/RigelNet/Packet.hpp>
#include <Hobgoblin/RigelNet/Remote_info.hpp>

#include <Hobgoblin/Utility/Stopwatch.hpp>

#include <SFML/System/Clock.hpp>
#include <SFML/Network.hpp>

#include <chrono>
#include <cstdint>

#include <Hobgoblin/Private/Pmacro_define.hpp>

HOBGOBLIN_NAMESPACE_START
namespace rn {

namespace detail {

struct TaggedPacket {
    enum Tag {
        DefaultTag,
    // SEND:
        ReadyForSending = DefaultTag,
        NotAcknowledged,
        AcknowledgedWeakly,
        AcknowledgedStrongly,
    // RECV:
        WaitingForData = DefaultTag,
        ReadyForUnpacking,
        Unpacked,
    };

    RN_PacketWrapper packetWrap;
    util::Stopwatch stopwatch;
    Tag tag = DefaultTag;
};

class RN_UdpConnector : public RN_Connector<RN_UdpConnector> {
public:
    RN_UdpConnector(sf::UdpSocket& socket, const std::string& passphrase);

    void tryAccept(sf::IpAddress addr, std::uint16_t port, RN_PacketWrapper& packetWrap);
    void connect(sf::IpAddress addr, std::uint16_t port);
    void disconnect(bool notfiyRemote);

    void checkForTimeout();
    void send(RN_Node& node);
    void receivedPacket(RN_PacketWrapper& packetWrap);
    void handleDataMessages(RN_Node& node);
    void sendAcks();
    
    const RN_RemoteInfo& getRemoteInfo() const noexcept;
    RN_ConnectorStatus getStatus() const noexcept;
    PZInteger getSendBufferSize() const;
    PZInteger getRecvBufferSize() const;

    void appendToNextOutgoingPacket(const void *data, std::size_t sizeInBytes);

private:
    RN_RemoteInfo _remoteInfo;
    std::chrono::microseconds _timeoutLimit = std::chrono::microseconds{0};
    sf::UdpSocket& _socket;
    const std::string& _passphrase;
    RN_ConnectorStatus _status;

    std::deque<detail::TaggedPacket> _sendBuffer;
    std::deque<detail::TaggedPacket> _recvBuffer;
    std::uint32_t _sendBufferHeadIndex = 0;
    std::uint32_t _recvBufferHeadIndex = 0;

    std::vector<std::uint32_t> _ackOrdinals;

    void cleanUp();
    void reset();
    bool connectionTimedOut() const;
    void uploadAllData();
    void prepareAck(std::uint32_t ordinal);
    void receivedAck(std::uint32_t ordinal, bool strong);
    void initializeSession();
    void prepareNextOutgoingPacket();
    void receiveDataMessage(RN_PacketWrapper& packetWrapper);
    
    void processHelloPacket(RN_PacketWrapper& packpacketWrapperet);
    void processConnectPacket(RN_PacketWrapper& packetWrapper);
    void processDisconnectPacket(RN_PacketWrapper& packetWrapper);
    void processDataPacket(RN_PacketWrapper& packetWrapper);
    void processAcksPacket(RN_PacketWrapper& packetWrapper);
};

} // namespace detail
} // namespace rn
HOBGOBLIN_NAMESPACE_END

#include <Hobgoblin/Private/Pmacro_undef.hpp>

#endif // !UHOBGOBLIN_RN_UDP_CONNECTOR_HPP
