#ifndef UHOBGOBLIN_RN_UDP_CONNECTOR_HPP
#define UHOBGOBLIN_RN_UDP_CONNECTOR_HPP

#include <Hobgoblin/Common.hpp>
#include <Hobgoblin/RigelNet/Connector.hpp>
#include <Hobgoblin/RigelNet/Event.hpp>
#include <Hobgoblin/RigelNet/Node.hpp>
#include <Hobgoblin/RigelNet/Packet_wrapper.hpp>
#include <Hobgoblin/RigelNet/Remote_info.hpp>

#include <Hobgoblin/Utility/Time_utils.hpp>

#include <SFML/System/Clock.hpp>
#include <SFML/Network.hpp>

#include <chrono>
#include <cstdint>
#include <functional>
#include <optional>

#include <Hobgoblin/Private/Pmacro_define.hpp>

HOBGOBLIN_NAMESPACE_START
namespace rn {

using RetransmitPredicate = std::function<bool(PZInteger, std::chrono::microseconds, std::chrono::microseconds)>;

inline
bool DefaultRetransmitPredicate(PZInteger cyclesSinceLastTransmit, std::chrono::microseconds timeSinceLastSend,
                                std::chrono::microseconds currentLatency) {
    return timeSinceLastSend > 2 * currentLatency;
}

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
    PZInteger cyclesSinceLastTransmit = 0;
    Tag tag = DefaultTag;
};

class RN_UdpConnector : public RN_Connector<RN_UdpConnector> {
public:
    RN_UdpConnector(sf::UdpSocket& socket, const std::chrono::microseconds& timeoutLimit, 
                    const std::string& passphrase, const RetransmitPredicate& retransmitPredicate,
                    EventFactory eventFactory);

    bool tryAccept(sf::IpAddress addr, std::uint16_t port, RN_PacketWrapper& packetWrap);
    void connect(sf::IpAddress addr, std::uint16_t port);
    void disconnect(bool notfiyRemote);

    void checkForTimeout();
    void send(RN_Node& node);
    void receivedPacket(RN_PacketWrapper& packetWrap, bool isClient);
    void handleDataMessages(RN_Node& node);
    void sendAcks();
    
    void setClientIndex(std::optional<PZInteger> clientIndex);
    std::optional<PZInteger> getClientIndex() const;
    const RN_RemoteInfo& getRemoteInfo() const noexcept;
    RN_ConnectorStatus getStatus() const noexcept;
    PZInteger getSendBufferSize() const;
    PZInteger getRecvBufferSize() const;

    void appendToNextOutgoingPacket(const void *data, std::size_t sizeInBytes);

    static constexpr PZInteger MAX_PACKET_SIZE = 65'000; // In bytes
    //static constexpr PZInteger MAX_PACKET_SIZE = 8'000; // In bytes

private:
    EventFactory _eventFactory;
    RN_RemoteInfo _remoteInfo;
    sf::UdpSocket& _socket;
    const std::string& _passphrase;
    const std::chrono::microseconds& _timeoutLimit;
    const RetransmitPredicate& _retransmitPredicate;
    RN_ConnectorStatus _status;
    std::optional<PZInteger> _clientIndex;

    std::deque<detail::TaggedPacket> _sendBuffer;
    std::deque<detail::TaggedPacket> _recvBuffer;
    std::uint32_t _sendBufferHeadIndex = 0;
    std::uint32_t _recvBufferHeadIndex = 0;

    std::vector<std::uint32_t> _ackOrdinals;

    void destroy();
    void reset();
    bool isConnectionTimedOut() const;
    void uploadAllData(bool isServer);
    void prepareAck(std::uint32_t ordinal);
    void receivedAck(std::uint32_t ordinal, bool strong);
    void initializeSession();
    void prepareNextOutgoingPacket();
    void receiveDataMessage(RN_PacketWrapper& packetWrapper, bool isClient);
    
    void processHelloPacket(RN_PacketWrapper& packpacketWrapperet);
    void processConnectPacket(RN_PacketWrapper& packetWrapper);
    void processDisconnectPacket(RN_PacketWrapper& packetWrapper);
    void processDataPacket(RN_PacketWrapper& packetWrapper, bool isClient);
    void processAcksPacket(RN_PacketWrapper& packetWrapper);
};

} // namespace detail
} // namespace rn
HOBGOBLIN_NAMESPACE_END

#include <Hobgoblin/Private/Pmacro_undef.hpp>
#include <Hobgoblin/Private/Short_namespace.hpp>

#endif // !UHOBGOBLIN_RN_UDP_CONNECTOR_HPP
