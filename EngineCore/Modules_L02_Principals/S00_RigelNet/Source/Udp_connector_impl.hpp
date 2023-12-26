#ifndef UHOBGOBLIN_RN_UDP_CONNECTOR_IMPL_HPP
#define UHOBGOBLIN_RN_UDP_CONNECTOR_IMPL_HPP

#include <Hobgoblin/Common.hpp>
#include <Hobgoblin/RigelNet/Configuration.hpp>
#include <Hobgoblin/RigelNet/Connector_interface.hpp>
#include <Hobgoblin/RigelNet/Events.hpp>
#include <Hobgoblin/RigelNet/Node_interface.hpp>
#include <Hobgoblin/RigelNet/Remote_info.hpp>
#include <Hobgoblin/RigelNet/Telemetry.hpp>
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

HOBGOBLIN_NAMESPACE_BEGIN
namespace rn {

class RN_ServerInterface;

struct TaggedPacket {
    enum Tag {
        DefaultTag = 0,
    // SEND:
        ReadyForSending      = DefaultTag,
        NotAcknowledged      = 1,
        AcknowledgedWeakly   = 2,
        AcknowledgedStrongly = 3,
    // RECV:
        WaitingForData      = DefaultTag,
        WaitingForMore      = 4,
        WaitingForMore_Tail = 5,
        ReadyForUnpacking   = 6,
        Unpacked            = 7,
    };

    util::Packet packet;
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
                        rn_detail::EventFactory eventFactory,
                        PZInteger aMaxPacketSize);

    bool tryAccept(sf::IpAddress addr, std::uint16_t port, util::Packet& packet);
    bool tryAcceptLocal(RN_UdpConnectorImpl& localPeer, const std::string& passphrase);
    void connect(sf::IpAddress addr, std::uint16_t port);
    void connectLocal(RN_ServerInterface& server);
    void disconnect(bool notfiyRemote);

    void checkForTimeout();
    auto send() -> RN_Telemetry;
    void prepToReceive();
    void receivedPacket(util::Packet& packet);
    void receivingFinished();
    void handleDataMessages(RN_NodeInterface& node, util::Packet*& pointerToCurrentPacket);
    auto sendAcks() -> RN_Telemetry;
    
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
    rn_detail::EventFactory _eventFactory;

    PZInteger _maxPacketSize;

    RN_RemoteInfo _remoteInfo;
    decltype(_remoteInfo.meanLatency) _newMeanLatency;
    decltype(_remoteInfo.meanLatency) _newOptimisticLatency;
    decltype(_remoteInfo.meanLatency) _newPessimisticLatency;
    PZInteger _newLatencySampleSize = 0;
    RN_ConnectorStatus _status;
    std::optional<PZInteger> _clientIndex;

    std::deque<TaggedPacket> _sendBuffer;
    std::deque<TaggedPacket> _recvBuffer;
    std::uint32_t _sendBufferHeadIndex = 0;
    std::uint32_t _recvBufferHeadIndex = 0;

    std::vector<std::uint32_t> _ackOrdinals;

    class LocalConnectionSharedState;
    std::shared_ptr<LocalConnectionSharedState> _localSharedState = nullptr;

    bool _skipNextDataPacketProcessing = false;

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
    //! Return estimated number of bytes uploaded.
    PZInteger _uploadAllData();

    //! Same as "_uploadAllData" but for a local connection.
    void _transferAllDataToLocalPeer();

    //! Saves the ack for the given packet ordinal into the ack buffer.
    void _prepareAck(std::uint32_t ordinal);

    //! Call when an ack is received to do the required book-keeping.
    //! Call with strong=true if it was received from a Data packet (false otherwise).
    void _receivedAck(std::uint32_t ordinal, bool strong);

    //! Sets the connector into the Connected state and resets the timeout timer.
    void _startSession();

    //! Appends a fresh data packet to the send buffer and fills in its 
    //! header (packet type, ordinal, acks).
    void _prepareNextOutgoingDataPacket(std::uint32_t packetType);

    //!
    void _tryToAssembleFragmentedPacketAtHead();

    //! Saves a received Data packet (without its headers and acks) into the
    //! receive buffer, unless it was received previously (Acks are prepared in 
    //! either case).
    void _saveDataPacket(util::Packet& packet,
                         std::uint32_t packetType);
    
    //! Process a "Hello" packet.
    void _processHelloPacket(util::Packet& packet);

    //! Process a "Connect" packet.
    void _processConnectPacket(util::Packet& packet);

    //! Process a "Disconnect" packet.
    void _processDisconnectPacket(util::Packet& packet);

    //! Process a "Data" packet.
    void _processDataPacket(util::Packet& packet);

    //! Process a "DataMore" packet.
    void _processDataMorePacket(util::Packet& packet);

    //! Process a "DataTail" packet.
    void _processDataTailPacket(util::Packet& packet);

    //! Process an "Acks" packet.
    void _processAcksPacket(util::Packet& packet);
};

} // namespace rn
HOBGOBLIN_NAMESPACE_END

#include <Hobgoblin/Private/Pmacro_undef.hpp>

#endif // !UHOBGOBLIN_RN_UDP_CONNECTOR_IMPL_HPP
