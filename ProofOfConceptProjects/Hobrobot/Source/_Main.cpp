
#include "Engine.hpp"

#include <HobRobot/Context/Game_context_factory.hpp>

#include <Hobgoblin/Config.hpp>
#include <Hobgoblin/HGExcept.hpp>
#include <Hobgoblin/Logging.hpp>
#include <Hobgoblin/Utility/Randomization.hpp>

#include <ZTCpp.hpp>

#include <cstdint>
#include <cstring>
#include <fstream>
#include <thread>

static constexpr auto LOG_ID = "Hobrobot";

using namespace hobrobot;
namespace zt = jbatnozic::ztcpp;

///////////////////////////////////////////////////////////////////////////
// ZEROTIER                                                              //
///////////////////////////////////////////////////////////////////////////

namespace {
constexpr auto ZT_LOG_ID = "hobrobot::ZeroTier";

class NodeInfo {
public:
    NodeInfo() = default;

    bool online = false;
    int networksJoinedCount = 0;
    uint64_t id = 0;
    zt::IpAddress ip4 = zt::IpAddress::ipv4Unspecified();
    // zt::IpAddress ip6 = zt::IpAddress::ipv6Unspecified();
};

class ZeroTierEventHandler : public zt::EventHandlerInterface {
public:
    ZeroTierEventHandler(NodeInfo& aNodeInfo)
        : _nodeInfo{aNodeInfo}
    {
    }

    void onAddressEvent(zt::EventCode::Address aEventCode, const zt::AddressDetails* aDetails) noexcept override {
        HG_LOG_INFO(ZT_LOG_ID, "{}", zt::EventDescription(aEventCode, aDetails));

        if (aDetails && aEventCode == zt::EventCode::Address::AddedIPv4) {
            _nodeInfo.ip4 = aDetails->getIpAddress();
        }
    }

    void onNetworkEvent(zt::EventCode::Network aEventCode, const zt::NetworkDetails* aDetails) noexcept override {
        HG_LOG_INFO(ZT_LOG_ID, "{}", zt::EventDescription(aEventCode, aDetails));

        if (aEventCode == zt::EventCode::Network::ReadyIPv4 || 
            aEventCode == zt::EventCode::Network::ReadyIPv6 ||
            aEventCode == zt::EventCode::Network::ReadyIPv4_IPv6) {
            _nodeInfo.networksJoinedCount += 1;
        }
        else if (aEventCode == zt::EventCode::Network::Down) {
            _nodeInfo.networksJoinedCount -= 1;
        }
    }

    void onNetworkInterfaceEvent(zt::EventCode::NetworkInterface aEventCode,
                                 const zt::NetworkInterfaceDetails* aDetails) noexcept override{
        HG_LOG_INFO(ZT_LOG_ID, "{}", zt::EventDescription(aEventCode, aDetails));
    }

    void onNetworkStackEvent(zt::EventCode::NetworkStack aEventCode,
                             const zt::NetworkStackDetails* aDetails) noexcept override {
        HG_LOG_INFO(ZT_LOG_ID, "{}", zt::EventDescription(aEventCode, aDetails));
    }

    void onNodeEvent(zt::EventCode::Node aEventCode, const zt::NodeDetails* aDetails) noexcept override {
        HG_LOG_INFO(ZT_LOG_ID, "{}", zt::EventDescription(aEventCode, aDetails));

        if (aEventCode == zt::EventCode::Node::Online) {
            _nodeInfo.online = true;
        }
        else if (aEventCode == zt::EventCode::Node::Online) {
            _nodeInfo.online = false;
        }
    }

    void onPeerEvent(zt::EventCode::Peer aEventCode, const zt::PeerDetails* aDetails) noexcept override {
        HG_LOG_INFO(ZT_LOG_ID, "{}", zt::EventDescription(aEventCode, aDetails));
    }

    void onRouteEvent(zt::EventCode::Route aEventCode, const zt::RouteDetails* aDetails) noexcept override {
        HG_LOG_INFO(ZT_LOG_ID, "{}", zt::EventDescription(aEventCode, aDetails));
    }

    void onUnknownEvent(int16_t aRawZeroTierEventCode) noexcept override {
        HG_LOG_WARN(ZT_LOG_ID, "Unknown event dispatched!");
    }

private:
    NodeInfo& _nodeInfo;
};

NodeInfo g_ztLocalNode;
ZeroTierEventHandler g_ztEventHandler{g_ztLocalNode};

} // namespace

///////////////////////////////////////////////////////////////////////////
// GAME CONFIG                                                           //
///////////////////////////////////////////////////////////////////////////

HGConfig LoadConfig(const char* aConfigFilePath) {
    std::ifstream configFile{aConfigFilePath};
    return HGConfig{configFile};
}

bool ConfigureZeroTier(const HGConfig& aConfig) {
    const bool ztEnabled = (aConfig.getStringValue("User_General", "ZeroTier_Enabled") == "YES");
    if (!ztEnabled) {
        HG_LOG_INFO(ZT_LOG_ID, "ZeroTier is disabled, skipping initialization.");
        return false;
    }

    HG_LOG_INFO(ZT_LOG_ID, "ZeroTier is enabled: Configuring service...");
    {
        const auto res = zt::Config::setIdentityFromStorage(aConfig.getStringValue("User_General", "ZeroTier_IdentityPath"));
        ZTCPP_THROW_ON_ERROR(res, std::runtime_error);
    }
    {
        const auto res = zt::Config::allowNetworkCaching(true);
        ZTCPP_THROW_ON_ERROR(res, std::runtime_error);
    }
    {
        const auto res = zt::Config::allowPeerCaching(true);
        ZTCPP_THROW_ON_ERROR(res, std::runtime_error);
    }
    {
        const auto res = zt::Config::allowIdentityCaching(true);
        ZTCPP_THROW_ON_ERROR(res, std::runtime_error);
    }
    {
        const auto res = zt::Config::setPort(aConfig.getIntegerValue<std::uint16_t>("User_General", "ZeroTier_ServicePort"));
        ZTCPP_THROW_ON_ERROR(res, std::runtime_error);
    }

    HG_LOG_INFO(ZT_LOG_ID, "Starting service...");
    {
        zt::LocalNode::setEventHandler(&g_ztEventHandler);
        const auto res = zt::LocalNode::start();
        ZTCPP_THROW_ON_ERROR(res, std::runtime_error);
    }

    while (!g_ztLocalNode.online) {
        HG_LOG_INFO(ZT_LOG_ID, "Waiting for local node to come online...");
        std::this_thread::sleep_for(std::chrono::milliseconds{500});
    }

    const auto networkID_str = aConfig.getRawValue("User_General", "ZeroTier_NetworkID");
    const auto networkID = std::strtoull(networkID_str.c_str(), nullptr, 16);
    HG_LOG_INFO(ZT_LOG_ID, "Joining network {:#x}", networkID);
    HG_LOG_INFO(ZT_LOG_ID, "Don't forget to authorize this device in my.zerotier.com or the web API!");
    {
        const auto res = zt::Network::join(networkID);
        ZTCPP_THROW_ON_ERROR(res, std::runtime_error);
    }

    while (g_ztLocalNode.networksJoinedCount <= 0) {
        HG_LOG_INFO(ZT_LOG_ID, "Waiting to join network...");
        std::this_thread::sleep_for(std::chrono::milliseconds{500});
    }

    const std::string banner = "!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!";

    HG_LOG_INFO(ZT_LOG_ID,
                "\n{}\n" // banner
                "* The app has been cussessfully configured to communicate via ZeroTier!\n"
                "* IP address of the local node on the ZT network is displayed below:\n"
                "* {}\n"
                "{}\n", // banner
                banner,
                g_ztLocalNode.ip4.toString(),
                banner
    );

    return true;
}

#define CATCH_EXCEPTIONS_IN_MAIN 1

/* SERVER:
 *   ./Hobrobot host
 *
 * CLIENT:
 *   ./Hobrobot client
 *
 */
int main(int argc, char* argv[])
#if CATCH_EXCEPTIONS_IN_MAIN
try
#endif
{
    // Set logging severity:
    hg::log::SetMinimalLogSeverity(hg::log::Severity::Info);

    // Seed pseudorandom number generators:
    hg::util::DoWith32bitRNG([](std::mt19937& aRNG) {
        aRNG.seed(hg::util::Generate32bitSeed());
    });
    hg::util::DoWith64bitRNG([](std::mt19937_64& aRNG) {
        aRNG.seed(hg::util::Generate64bitSeed());
    });
    
    // Initialize RigelNet:
    RN_IndexHandlers();

    // Parse command line arguments & load config:
    if (argc != 2) {
        HG_LOG_INFO(LOG_ID, 
                    "Hobrobot requires exactly one command line argument to set the mode. "
                    "Possible values are 'host' and 'client'.");
        return EXIT_FAILURE;
    }
    const std::string mode = argv[1];

    const auto config = LoadConfig("assets/SETTINGS.hgconf");
    const bool ztEnabled = ConfigureZeroTier(config);

    // Start the game:
    const std::unique_ptr<spe::GameContext> context = (mode == "host") ? CreateHostContext(config)
                                                                       : CreateClientContext(config);
    const int status = context->runFor(-1);
    HG_LOG_INFO(LOG_ID, "Main program loop exited with status code: {}.", status);

    if (ztEnabled) {
        zt::LocalNode::stop();
        std::this_thread::sleep_for(std::chrono::milliseconds{1000});
        zt::LocalNode::setEventHandler(nullptr);
    }

    return status;
}
#if CATCH_EXCEPTIONS_IN_MAIN
catch (const hg::TracedException& ex) {
    HG_LOG_FATAL(LOG_ID, "Traced exception caught: {}", ex.getFullFormattedDescription());
}
catch (const std::exception& ex) {
    HG_LOG_FATAL(LOG_ID, "Exception caught: {}", ex.what());
    return EXIT_FAILURE;
}
catch (...) {
    HG_LOG_FATAL(LOG_ID, "Exception caught: UNKNOWN");
    return EXIT_FAILURE;
}
#endif
