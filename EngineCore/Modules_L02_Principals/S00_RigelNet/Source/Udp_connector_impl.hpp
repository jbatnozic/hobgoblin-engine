// Copyright 2024 Jovan Batnozic. Released under MS-PL licence in Serbia.
// See https://github.com/jbatnozic/Hobgoblin?tab=readme-ov-file#licence

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
#include "Udp_receive_buffer.hpp"
#include "Udp_send_buffer.hpp"

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

class RN_UdpConnectorImpl
    : public RN_ConnectorInterface
    , NO_COPY
    , NO_MOVE {
public:
    RN_UdpConnectorImpl(RN_SocketAdapter&                aSocket,
                        const std::chrono::microseconds& aTimeoutLimit,
                        const std::string&               aPassphrase,
                        const RN_RetransmitPredicate&    aRetransmitPredicate,
                        rn_detail::EventFactory          aEventFactory,
                        PZInteger                        aMaxPacketSize);

    // Accepting a connection from a client

    bool tryAccept(sf::IpAddress addr, std::uint16_t port, util::Packet& packet);
    bool tryAcceptLocal(RN_UdpConnectorImpl& localPeer, const std::string& passphrase);

    // Connecting to a server

    void connect(sf::IpAddress addr, std::uint16_t port);
    void connectLocal(RN_ServerInterface& server);

    // Receiving

    // NOTE: Happy flow of the receive step (called by the Server object
    //       or Client object) is as follows:
    //       - prepToReceive()
    //       - receivedPacket() - 0 or more times
    //       - receivingFinished()
    //       - sendWeakAcks()
    //       - handleDataMessages()
    //       - checkForTimeout()

    void prepToReceive();
    void receivedPacket(util::Packet& packet);
    void receivingFinished();
    auto sendWeakAcks() -> RN_Telemetry;
    void handleDataMessages(RN_NodeInterface& aNode, NeverNull<util::Packet**> aCurrentPacketPtr);
    void checkForTimeout();

    // Sending

    void appendDataForSending(NeverNull<const void*> aData, PZInteger aDataByteCount);
    auto sendData() -> RN_Telemetry;

    // Client index

    // NOTE: On the server side, the client index of the connector is set by the server object
    //       itself (based on the index of the index of the connector in the array). On the client
    //       side, it is received as a part of a CONNECT packet.

    void                     setClientIndex(std::optional<PZInteger> clientIndex);
    std::optional<PZInteger> getClientIndex() const;

    // Inherited from RN_ConnectorInterface

    const RN_RemoteInfo& getRemoteInfo() const noexcept override;
    RN_ConnectorStatus   getStatus() const noexcept override;
    bool                 isConnected() const noexcept override;
    bool                 isDisconnected() const noexcept override;
    void      disconnect(bool aNotfiyRemote = true, const std::string& aMessage = "") override;
    bool      isConnectedLocally() const noexcept override;
    PZInteger getSendBufferSize() const override;
    PZInteger getRecvBufferSize() const override;

private:
    // _socket, _timeoutLimit, _passphrase and _retransmitPredicate are references
    // to objects that live in the Server or Client object.
    RN_SocketAdapter&                _socket;
    const std::chrono::microseconds& _timeoutLimit;
    const std::string&               _passphrase;
    const RN_RetransmitPredicate&    _retransmitPredicate;

    rn_detail::EventFactory _eventFactory;

    PZInteger _maxPacketSize;

    RN_RemoteInfo                     _remoteInfo;
    decltype(_remoteInfo.meanLatency) _newMeanLatency;
    decltype(_remoteInfo.meanLatency) _newOptimisticLatency;
    decltype(_remoteInfo.meanLatency) _newPessimisticLatency;
    PZInteger                         _newLatencySampleSize = 0;
    RN_ConnectorStatus                _status;
    std::optional<PZInteger>          _clientIndex;

    UdpSendBuffer    _sendBuffer;
    UdpReceiveBuffer _recvBuffer;

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

    //! Saves a received Data packet (without its headers and acks) into the
    //! receive buffer, unless it was received previously (Acks are prepared in
    //! either case).
    void _saveDataPacket(util::Packet& packet, std::uint32_t packetType);

    // ===== PACKET PROCESSING ===== //

    void _processHelloPacket(util::Packet& packet);
    void _processConnectPacket(util::Packet& packet);
    void _processDisconnectPacket(util::Packet& packet);
    void _processDataPacket(util::Packet& packet);
    void _processDataMorePacket(util::Packet& packet);
    void _processDataTailPacket(util::Packet& packet);
    void _processAcksPacket(util::Packet& packet);
};

} // namespace rn
HOBGOBLIN_NAMESPACE_END

#include <Hobgoblin/Private/Pmacro_undef.hpp>

#endif // !UHOBGOBLIN_RN_UDP_CONNECTOR_IMPL_HPP
