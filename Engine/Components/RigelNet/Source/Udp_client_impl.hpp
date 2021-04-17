#ifndef UHOBGOBLIN_RN_UDP_CLIENT_HPP
#define UHOBGOBLIN_RN_UDP_CLIENT_HPP

#include <Hobgoblin/RigelNet/Client_interface.hpp>
#include <Hobgoblin/RigelNet/Remote_info.hpp>
#include <Hobgoblin/Utility/No_copy_no_move.hpp>

#include "Node_base.hpp"
#include "Socket_adapter.hpp"
#include "Udp_connector_impl.hpp"

#include <chrono>
#include <string>

#include <Hobgoblin/Private/Pmacro_define.hpp>

HOBGOBLIN_NAMESPACE_START
namespace rn {

class RN_UdpClientImpl : public RN_NodeBase, public RN_ClientInterface {
public:
    RN_UdpClientImpl(std::string aPassphrase,
                     RN_NetworkingStack aNetworkingStack);

    ~RN_UdpClientImpl();

    ///////////////////////////////////////////////////////////////////////////
    // CLIENT CONTROL                                                        //
    ///////////////////////////////////////////////////////////////////////////

    void connect(std::uint16_t localPort,
                 sf::IpAddress serverIp,
                 std::uint16_t serverPort) override;

    void disconnect(bool notifyRemote) override;

    void setTimeoutLimit(std::chrono::microseconds limit) override;

    // From RN_NodeInterface:

    void update(RN_UpdateMode mode) override;

    bool pollEvent(RN_Event& ev) override;

    ///////////////////////////////////////////////////////////////////////////
    // STATE INSPECTION                                                      //
    ///////////////////////////////////////////////////////////////////////////

    bool isRunning() const override;

    const std::string& getPassphrase() const override;

    std::chrono::microseconds getTimeoutLimit() const override;

    std::uint16_t getLocalPort() const override;

    const RN_ConnectorInterface& getServerConnector() const override;

    PZInteger getClientIndex() const override;

    // From RN_NodeInterface:

    bool isServer() const noexcept override;

    RN_Protocol getProtocol() const noexcept override;

    RN_NetworkingStack getNetworkingStack() const noexcept override;
      




    void setRetransmitPredicate(RetransmitPredicate pred);

private:
    RN_SocketAdapter _socket;

    RN_UdpConnectorImpl _connector;

    std::string _passphrase;
    std::chrono::microseconds _timeoutLimit = std::chrono::microseconds{0};
    RetransmitPredicate _retransmitPredicate;
    bool _running = false;

    detail::RN_PacketWrapper* _currentPacket = nullptr;

    void _updateReceive();
    void _updateSend();

    void _compose(int receiver, const void* data, std::size_t sizeInBytes) override;
    void _compose(RN_ComposeForAllType receiver, const void* data, std::size_t sizeInBytes) override;
    detail::RN_PacketWrapper* _getCurrentPacketWrapper() override;
};

} // namespace rn
HOBGOBLIN_NAMESPACE_END

#include <Hobgoblin/Private/Pmacro_undef.hpp>

#endif // !UHOBGOBLIN_RN_UDP_CLIENT_HPP
