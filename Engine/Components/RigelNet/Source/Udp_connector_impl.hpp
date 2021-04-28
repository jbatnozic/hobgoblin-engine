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

    ///////////////////////////////////////////////////////////////////////////
    // PRIVATE METHODS                                                       //
    ///////////////////////////////////////////////////////////////////////////

    //! Returns true if the connector is configured for a local connection (even
    //! if the connection was terminated).
    bool _isConnectedLocally() const noexcept;

    //! Clears the send/receive buffers, sets the head indices back to 1, and
    //! also clears the ack buffer.
    void _resetBuffers();

    //! Clears all used data and reverts the connector into its original
    //! (unconnected) state.
    void _resetAll();

    //! Return true if the remote host hasn't reported back for longer than
    //! the maximum timeout period.
    bool _isConnectionTimedOut() const;

    //! Sends all prepared data to the remote host (that is actually remote).
    void _uploadAllData();

    //! Same as "_uploadAllData" but for a local connection.
    void _transferAllDataToLocalPeer();

    //! Saves the ack for the given packet ordinal into the ack buffer.
    void _prepareAck(std::uint32_t ordinal);

    //! Call when an ack is received to do the required book-keeping.
    //! Call with strong=true if it was received from a Data packet (false otherwise).
    void _receivedAck(std::uint32_t ordinal, bool strong);

    //! Sets the connector into the Connected state and resets the timeout timer.
    void _startSession();

    //! Appends a fresh packet to the send buffer and fills in its 
    //! header (packet type, ordinal, acks).
    void _prepareNextOutgoingPacket();

    //! Saves a received Data packet (without its headers and acks) into the
    //! receive buffer, unless it was received previously (Acks are sent in 
    //! either case).
    void _saveDataPacket(detail::RN_PacketWrapper& packetWrapper);
    
    //! Process a "Hello" packet.
    void _processHelloPacket(detail::RN_PacketWrapper& packpacketWrapperet);

    //! Process a "Connect" packet.
    void _processConnectPacket(detail::RN_PacketWrapper& packetWrapper);

    //! Process a "Disconnect" packet.
    void _processDisconnectPacket(detail::RN_PacketWrapper& packetWrapper);

    //! Process a "Data" packet.
    void _processDataPacket(detail::RN_PacketWrapper& packetWrapper);

    //! Process an "Acks" packet.
    void _processAcksPacket(detail::RN_PacketWrapper& packetWrapper);
};

} // namespace rn
HOBGOBLIN_NAMESPACE_END

#include <Hobgoblin/Private/Pmacro_undef.hpp>

#endif // !UHOBGOBLIN_RN_UDP_CONNECTOR_IMPL_HPP
