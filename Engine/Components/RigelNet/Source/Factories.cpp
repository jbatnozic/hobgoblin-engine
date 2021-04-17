
#include <Hobgoblin/RigelNet/Factories.hpp>
#include <Hobgoblin/Utility/Exceptions.hpp>

#include "Udp_client_impl.hpp"
#include "Udp_server_impl.hpp"

#include <cassert>

#include <Hobgoblin/Private/Pmacro_define.hpp>

HOBGOBLIN_NAMESPACE_START
namespace rn {

std::unique_ptr<RN_ServerInterface> RN_ServerFactory::createServer(RN_Protocol aProtocol,
                                                                   std::string aPassphrase,
                                                                   PZInteger aServerSize,
                                                                   RN_NetworkingStack aNetworkingStack) {
    switch (aProtocol) {
    case RN_Protocol::TCP:
        throw util::TracedException("TCP not implemented!");
        break;

    case RN_Protocol::UDP:
        return std::make_unique<RN_UdpServerImpl>(aPassphrase, aServerSize, aNetworkingStack);
        break;

    default:
        break;
    }

    assert(false && "Unreachable");
    return nullptr;
}

std::unique_ptr<RN_ServerInterface> RN_ServerFactory::createDummyServer() {
    return nullptr; // TODO
}





std::unique_ptr<RN_ClientInterface> RN_ClientFactory::createClient(RN_Protocol aProtocol,
                                                                   std::string aPassphrase,
                                                                   RN_NetworkingStack aNetworkingStack) {
    switch (aProtocol) {
    case RN_Protocol::TCP:
        throw util::TracedException("TCP not implemented!");
        break;

    case RN_Protocol::UDP:
        return std::make_unique<RN_UdpClientImpl>(aPassphrase, aNetworkingStack);
        break;

    default:
        break;
    }

    assert(false && "Unreachable");
    return nullptr;
}

std::unique_ptr<RN_ClientInterface> RN_ClientFactory::createDummyClient() {
    return nullptr; // TODO
}

} // namespace rn
HOBGOBLIN_NAMESPACE_END

#include <Hobgoblin/Private/Pmacro_undef.hpp>