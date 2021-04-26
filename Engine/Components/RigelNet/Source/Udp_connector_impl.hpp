#ifndef UHOBGOBLIN_RN_UDP_CONNECTOR_IMPL_HPP
#define UHOBGOBLIN_RN_UDP_CONNECTOR_IMPL_HPP

#include <Hobgoblin/Common.hpp>
#include <Hobgoblin/RigelNet/Configuration.hpp>
#include <Hobgoblin/RigelNet/Connector_interface.hpp>
#include <Hobgoblin/RigelNet/Events.hpp>
#include <Hobgoblin/RigelNet/Node_interface.hpp>
#include <Hobgoblin/RigelNet/Packet_wrapper.hpp>
#include <Hobgoblin/RigelNet/Remote_info.hpp>
#include <Hobgoblin/Utility/No_copy_no_move.hpp>
#include <Hobgoblin/Utility/Time_utils.hpp>

#include "Socket_adapter.hpp"

#include <chrono>
#include <cstdint>
#include <deque>
#include <functional>
#include <memory>
#include <optional>

#include <Hobgoblin/Private/Pmacro_define.hpp>

HOBGOBLIN_NAMESPACE_START
namespace rn {

class RN_ServerInterface;

inline // TODO make this not-inline
bool DefaultRetransmitPredicate(PZInteger cyclesSinceLastTransmit, 
                                std::chrono::microseconds timeSinceLastSend,
                                std::chrono::microseconds currentLatency) {
    return timeSinceLastSend > 2 * currentLatency;
}

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

    detail::RN_PacketWrapper packetWrap;
    util::Stopwatch stopwatch;
    PZInteger cyclesSinceLastTransmit = 0;
    Tag tag = DefaultTag;
};

class RN_UdpConnectorImpl : public RN_ConnectorInterface, NO_COPY, NO_MOVE {
public:
    RN_UdpConnectorImpl(RN_SocketAdapter& socket,
                        const std::chrono::microseconds& timeoutLimit, 
                        const std::string& passphrase, 
                        const RN_RetransmitPredicate& retransmitPredicate,
                        detail::EventFactory eventFactory,
                        PZInteger aMaxPacketSize);

    bool tryAccept(sf::IpAddress addr, std::uint16_t port, detail::RN_PacketWrapper& packetWrap);
    bool tryAcceptLocal(RN_UdpConnectorImpl& localPeer, const std::string& passphrase);
    void connect(sf::IpAddress addr, std::uint16_t port);
    void connectLocal(RN_ServerInterface& server);
    void disconnect(bool notfiyRemote);

    void checkForTimeout();
    void send();
    void receivedPacket(detail::RN_PacketWrapper& packetWrap);
    void handleDataMessages(RN_NodeInterface& node, detail::RN_PacketWrapper*& pointerToCurrentPacket);
    void sendAcks();
    
    void setClientIndex(std::optional<PZInteger> clientIndex);
    std::optional<PZInteger> getClientIndex() const;

    const RN_RemoteInfo& getRemoteInfo() const noexcept override;
    RN_ConnectorStatus getStatus() const noexcept override;
    bool isConnectedLocally() const noexcept override;
    PZInteger getSendBufferSize() const override;
    PZInteger getRecvBufferSize() const override;

    void appendToNextOutgoingPacket(const void *data, std::size_t sizeInBytes);

private:
    RN_SocketAdapter& _socket;

    const std::chrono::microseconds& _timeoutLimit;
    const std::string& _passphrase;
    const RN_RetransmitPredicate& _retransmitPredicate;
    detail::EventFactory _eventFactory;

    PZInteger _maxPacketSize;

    RN_RemoteInfo _remoteInfo;
    RN_ConnectorStatus _status;
    std::optional<PZInteger> _clientIndex;

    std::deque<TaggedPacket> _sendBuffer;
    std::deque<TaggedPacket> _recvBuffer;
    std::uint32_t _sendBufferHeadIndex = 0;
    std::uint32_t _recvBufferHeadIndex = 0;

    std::vector<std::uint32_t> _ackOrdinals;

    class LocalConnectionSharedState;
    std::shared_ptr<LocalConnectionSharedState> _localSharedState = nullptr;

    bool _isConnectedLocally() const noexcept;
    void destroy();
    void reset();
    bool isConnectionTimedOut() const;
    void uploadAllData();
    void transferAllDataToLocalPeer();
    void prepareAck(std::uint32_t ordinal);
    void receivedAck(std::uint32_t ordinal, bool strong);
    void initializeSession();
    void prepareNextOutgoingPacket();
    void receiveDataMessage(detail::RN_PacketWrapper& packetWrapper);
    
    void processHelloPacket(detail::RN_PacketWrapper& packpacketWrapperet);
    void processConnectPacket(detail::RN_PacketWrapper& packetWrapper);
    void processDisconnectPacket(detail::RN_PacketWrapper& packetWrapper);
    void processDataPacket(detail::RN_PacketWrapper& packetWrapper);
    void processAcksPacket(detail::RN_PacketWrapper& packetWrapper);
};

} // namespace rn
HOBGOBLIN_NAMESPACE_END

#include <Hobgoblin/Private/Pmacro_undef.hpp>

#endif // !UHOBGOBLIN_RN_UDP_CONNECTOR_IMPL_HPP
