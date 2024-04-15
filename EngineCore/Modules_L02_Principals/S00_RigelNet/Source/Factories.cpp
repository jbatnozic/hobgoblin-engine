// Copyright 2024 Jovan Batnozic. Released under MS-PL licence in Serbia.
// See https://github.com/jbatnozic/Hobgoblin?tab=readme-ov-file#licence

// clang-format off


#include <Hobgoblin/RigelNet/Factories.hpp>
#include <Hobgoblin/Common.hpp>
#include <Hobgoblin/HGExcept.hpp>

#include "Udp_client_impl.hpp"
#include "Udp_server_impl.hpp"

#include <cassert>

#include <Hobgoblin/Private/Pmacro_define.hpp>

HOBGOBLIN_NAMESPACE_BEGIN
namespace rn {

namespace {
class RN_DummyServer : public RN_ServerInterface {
public:
    ///////////////////////////////////////////////////////////////////////////
    // SERVER CONTROL                                                        //
    ///////////////////////////////////////////////////////////////////////////

    void start(std::uint16_t localPort) override {}

    void stop() override {}

    void resize(PZInteger newSize) override {}

    void setTimeoutLimit(std::chrono::microseconds limit) override {}

    void setRetransmitPredicate(RN_RetransmitPredicate pred) override {}

    // From RN_NodeInterface:

    RN_Telemetry update(RN_UpdateMode mode) override { return {}; }

    bool pollEvent(RN_Event& ev) override {
        return false;
    }

    ///////////////////////////////////////////////////////////////////////////
    // CLIENT MANAGEMENT                                                     //
    ///////////////////////////////////////////////////////////////////////////

    const RN_ConnectorInterface& getClientConnector(PZInteger clientIndex) const override {
        HG_UNREACHABLE("Dummy Server doesn't have any client connectors");
        return *reinterpret_cast<RN_ConnectorInterface*>(0x12345678);
    }

    void swapClients(PZInteger index1, PZInteger index2) override {}

    void kickClient(PZInteger index) override {}

    ///////////////////////////////////////////////////////////////////////////
    // STATE INSPECTION                                                      //
    ///////////////////////////////////////////////////////////////////////////

    bool isRunning() const override { 
        return false;
    }

    PZInteger getSize() const override {
        return 0;
    }

    const std::string& getPassphrase() const override {
        return _passphrase;
    }

    std::chrono::microseconds getTimeoutLimit() const override {
        return std::chrono::microseconds{0};
    }

    std::uint16_t getLocalPort() const override {
        return 0;
    }

    int getSenderIndex() const override { 
        return -1;
    }

    // From RN_NodeInterface:

    bool isServer() const noexcept override {
        return true;
    }

    RN_Protocol getProtocol() const noexcept override {
        return RN_Protocol::None;
    }

    RN_NetworkingStack getNetworkingStack() const noexcept override {
        return RN_NetworkingStack::Default;
    }

private:
    std::string _passphrase = "";

    void _compose(RN_ComposeForAllType receiver, const void* data, std::size_t sizeInBytes) override {}

    void _compose(PZInteger receiver, const void* data, std::size_t sizeInBytes) override {}

    util::Packet* _getCurrentPacket() override { return nullptr; }

    void _setUserData(util::AnyPtr userData) override {}

    util::AnyPtr _getUserData() const override {
        return nullptr;
    }
};
} // namespace

std::unique_ptr<RN_ServerInterface> RN_ServerFactory::createServer(RN_Protocol aProtocol,
                                                                   std::string aPassphrase,
                                                                   PZInteger aServerSize,
                                                                   PZInteger aMaxPacketSize,
                                                                   RN_NetworkingStack aNetworkingStack) {
    switch (aProtocol) {
    case RN_Protocol::TCP:
        HG_NOT_IMPLEMENTED("TCP is not currently supported by RigelNet.");
        break;

    case RN_Protocol::UDP:
        return std::make_unique<RN_UdpServerImpl>(aPassphrase, aServerSize, aNetworkingStack, aMaxPacketSize);
        break;

    default:
        break;
    }

    HG_UNREACHABLE();
    return nullptr;
}

std::unique_ptr<RN_ServerInterface> RN_ServerFactory::createDummyServer() {
    return std::make_unique<RN_DummyServer>();
}

namespace {
class RN_DummyClient : public RN_ClientInterface {
    // TODO
};
} // namespace

std::unique_ptr<RN_ClientInterface> RN_ClientFactory::createClient(RN_Protocol aProtocol,
                                                                   std::string aPassphrase,
                                                                   PZInteger aMaxPacketSize,
                                                                   RN_NetworkingStack aNetworkingStack) {
    switch (aProtocol) {
    case RN_Protocol::TCP:
        HG_NOT_IMPLEMENTED("TCP is not currently supported by RigelNet.");
        break;

    case RN_Protocol::UDP:
        return std::make_unique<RN_UdpClientImpl>(aPassphrase, aNetworkingStack, aMaxPacketSize);
        break;

    default:
        break;
    }

    HG_UNREACHABLE();
    return nullptr;
}

std::unique_ptr<RN_ClientInterface> RN_ClientFactory::createDummyClient() {
    HG_NOT_IMPLEMENTED("Creating cummy clients not currently supported by RigelNet.");
    return nullptr; // TODO
}

} // namespace rn
HOBGOBLIN_NAMESPACE_END

#include <Hobgoblin/Private/Pmacro_undef.hpp>

// clang-format on
