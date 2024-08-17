
#include <HobRobot/Context/Game_context_factory.hpp>

#include <HobRobot/Context/Managers/Environment_manager.hpp>
#include <HobRobot/Context/Managers/Lobby_frontend_manager.hpp>
#include <HobRobot/Context/Managers/Main_gameplay_manager.hpp>

#include <HobRobot/Common/Player_controls.hpp>
#include <HobRobot/GameObjects/Player_robot.hpp>

namespace hobrobot {

using hg::HGConfig;
using hg::PZInteger;

std::unique_ptr<spe::GameContext> CreateHostContext(const HGConfig& aConfig) {
    const auto tickRate = aConfig.getIntegerValue<PZInteger>("Engine", "TickRate");
    // From the user perspective, this does not include the server, so it's equivalent to client count
    const auto playerCount = aConfig.getIntegerValue<PZInteger>("User_Host", "PlayerCount");

    auto context = std::make_unique<spe::GameContext>(
        spe::GameContext::RuntimeConfig{});
    context->setToMode(spe::GameContext::Mode::Server);

    // WINDOW MANAGER

    auto winMgr = std::make_unique<spe::DefaultWindowManager>(context->getQAORuntime().nonOwning(),
                                                              PRIORITY_WINDOWMGR);

    const auto timingConfig =
        spe::WindowManagerInterface::TimingConfig(60,
                                                  spe::PREVENT_BUSY_WAIT_OFF,
                                                  spe::VSYNC_OFF);
    winMgr->setToHeadlessMode(timingConfig);

    context->attachAndOwnComponent(std::move(winMgr));

    // NETWORKING MANAGER

    auto netMgr = std::make_unique<spe::DefaultNetworkingManager>(context->getQAORuntime().nonOwning(),
                                                                  PRIORITY_NETWORKMGR,
                                                                  STATE_BUFFERING_LENGTH);

    netMgr->setToServerMode(
        RN_Protocol::UDP,
        aConfig.getStringValue("Engine", "RigelNet_Passphrase"),
        playerCount,
        aConfig.getIntegerValue<PZInteger>("Engine", "RigelNet_MaxPacketSize"),
        (aConfig.getStringValue("User_General", "ZeroTier_Enabled") != "YES") ? RN_NetworkingStack::Default 
                                                                              : RN_NetworkingStack::ZeroTier
    );
    {
        auto& server = netMgr->getServer();
        server.setTimeoutLimit(std::chrono::seconds{5});
        server.start(aConfig.getIntegerValue<std::uint16_t>("User_Host", "LocalPort"));
        std::printf("Server started on port %d for up to %d clients.\n", (int)server.getLocalPort(), playerCount);
    }
    
    context->attachAndOwnComponent(std::move(netMgr));

    // INPUT SYNC MANAGER

    auto insMgr = std::make_unique<spe::DefaultInputSyncManager>(context->getQAORuntime().nonOwning(),
                                                                 PRIORITY_INPUTMGR);

    insMgr->setToHostMode(playerCount, STATE_BUFFERING_LENGTH);
    SetUpPlayerControlsDefinitions(*insMgr);

    context->attachAndOwnComponent(std::move(insMgr));

    // SYNCED VARMAN MANAGER

    auto svmMgr = std::make_unique<spe::DefaultSyncedVarmapManager>(context->getQAORuntime().nonOwning(),
                                                                    PRIORITY_VARMAPMGR);

    svmMgr->setToMode(spe::SyncedVarmapManagerInterface::Mode::Host);

    context->attachAndOwnComponent(std::move(svmMgr));

    // LOBBY BACKEND MANAGER

    auto lobbyMgr = std::make_unique<spe::DefaultLobbyBackendManager>(context->getQAORuntime().nonOwning(),
                                                                      PRIORITY_LOBBYBACKMGR);

    lobbyMgr->setToHostMode(playerCount + 1);

    context->attachAndOwnComponent(std::move(lobbyMgr));

    // LOBBY FRONTEND MANAGER

    auto lobbyFrontendMgr = std::make_unique<LobbyFrontendManager>(context->getQAORuntime().nonOwning(),
                                                                   PRIORITY_LOBBYFRONTMGR);

    lobbyFrontendMgr->setToHeadlessHostMode();

    context->attachAndOwnComponent(std::move(lobbyFrontendMgr));

    // AUTHORIZATION MANAGER

    auto authMgr = std::make_unique<spe::DefaultAuthorizationManager>(context->getQAORuntime().nonOwning(),
                                                                      PRIORITY_AUTHMGR);

    authMgr->setToHostMode();

    context->attachAndOwnComponent(std::move(authMgr));

    // MAIN GAMEPLAY MANAGER

    auto gpMgr = std::make_unique<MainGameplayManager>(context->getQAORuntime().nonOwning(),
                                                       PRIORITY_GAMEPLAYMGR);
    context->attachAndOwnComponent(std::move(gpMgr));

    // ENVIRONMENT MANAGER

    auto envMgr = std::make_unique<EnvironmentManager>(context->getQAORuntime().nonOwning(),
                                                       PRIORITY_ENVIRONMENTMGR);
    envMgr->setToHostMode();
    context->attachAndOwnComponent(std::move(envMgr));

    // Create player "characters"
    auto* p = QAO_PCreate<PlayerRobot>(context->getQAORuntime(),
                                       context->getComponent<MNetworking>().getRegistryId(),
                                       spe::SYNC_ID_NEW);
    p->init(300.0, 300.0);

    return context;
}

std::unique_ptr<spe::GameContext> CreateClientContext(const HGConfig& aConfig) {
    const auto tickRate = aConfig.getIntegerValue<PZInteger>("Engine", "TickRate");

    auto context = std::make_unique<spe::GameContext>(
        spe::GameContext::RuntimeConfig{});
    context->setToMode(spe::GameContext::Mode::Client);

    // WINDOW MANAGER

    auto winMgr = std::make_unique<spe::DefaultWindowManager>(context->getQAORuntime().nonOwning(),
                                                              PRIORITY_WINDOWMGR);

    const auto windowWidth = aConfig.getIntegerValue<int>("User_Client", "WindowPixelWidth");
    const auto windowHeight = aConfig.getIntegerValue<int>("User_Client", "WindowPixelHeight");

    winMgr->setToNormalMode(
        spe::WindowManagerInterface::WindowConfig{
            hg::win::VideoMode{windowWidth, windowHeight},
            "SPeMPE Minimal Multiplayer",
            hg::win::WindowStyle::Default
        },
        spe::WindowManagerInterface::MainRenderTextureConfig{{854, 480}}, // TODO
        spe::WindowManagerInterface::TimingConfig{
            60,
            (aConfig.getStringValue("User_Client", "PreciseTiming") == "YES"),
            (aConfig.getStringValue("User_Client", "V-Sync") == "YES")
        }
    );

    struct FontFace {
        Rml::String filename;
        bool fallback_face;
    };
    FontFace font_faces[] = {
        { "LatoLatin-Regular.ttf",    false },
        { "LatoLatin-Italic.ttf",     false },
        { "LatoLatin-Bold.ttf",       false },
        { "LatoLatin-BoldItalic.ttf", false },
    };
    for (const FontFace& face : font_faces) {
        Rml::LoadFontFace("assets/fonts/" + face.filename, face.fallback_face);
    }

    Rml::Debugger::Initialise(&(winMgr->getGUIContext()));
    Rml::Debugger::SetVisible(aConfig.getStringValue("User_Client", "V-Sync") == "RmlUi_DebuggerEnabled");
    

    context->attachAndOwnComponent(std::move(winMgr));

    // NETWORKING MANAGER

    auto netMgr = std::make_unique<spe::DefaultNetworkingManager>(context->getQAORuntime().nonOwning(),
                                                                  PRIORITY_NETWORKMGR,
                                                                  STATE_BUFFERING_LENGTH);
    netMgr->setToClientMode(
        RN_Protocol::UDP,
        aConfig.getStringValue("Engine", "RigelNet_Passphrase"),
        aConfig.getIntegerValue<PZInteger>("Engine", "RigelNet_MaxPacketSize"),
        (aConfig.getStringValue("User_General", "ZeroTier_Enabled") != "YES") ? RN_NetworkingStack::Default 
                                                                              : RN_NetworkingStack::ZeroTier
    );
    {
        auto& client = netMgr->getClient();
        client.setTimeoutLimit(std::chrono::seconds{5});
        client.connect(
            aConfig.getIntegerValue<std::uint16_t>("User_Client", "LocalPort"),
            aConfig.getStringValue("User_Client", "ServerIP"),
            aConfig.getIntegerValue<std::uint16_t>("User_Client", "ServerPort")
        );

        //std::printf("Client started on port %d (connecting to %s:%d)\n",
        //            (int)client.getLocalPort(), aRemoteIp.c_str(), (int)aRemotePort);
    }
    
    context->attachAndOwnComponent(std::move(netMgr));

    // INPUT SYNC MANAGER

    auto insMgr = std::make_unique<spe::DefaultInputSyncManager>(context->getQAORuntime().nonOwning(),
                                                                 PRIORITY_INPUTMGR);

    insMgr->setToClientMode();
    SetUpPlayerControlsDefinitions(*insMgr);

    context->attachAndOwnComponent(std::move(insMgr));

    // SYNCED VARMAP MANAGER

    auto svmMgr = std::make_unique<spe::DefaultSyncedVarmapManager>(context->getQAORuntime().nonOwning(),
                                                                    PRIORITY_VARMAPMGR);
    
    svmMgr->setToMode(spe::SyncedVarmapManagerInterface::Mode::Client);

    context->attachAndOwnComponent(std::move(svmMgr));

    // LOBBY BACKEND MANAGER

    auto lobbyMgr = std::make_unique<spe::DefaultLobbyBackendManager>(context->getQAORuntime().nonOwning(),
                                                                      PRIORITY_LOBBYBACKMGR);

    lobbyMgr->setToClientMode(1);

    context->attachAndOwnComponent(std::move(lobbyMgr));

    // LOBBY FRONTEND MANAGER

    auto lobbyFrontendMgr = std::make_unique<LobbyFrontendManager>(context->getQAORuntime().nonOwning(),
                                                                   PRIORITY_LOBBYFRONTMGR);

    const auto nameInLobby = aConfig.getStringValue("User_Client", "Nickname");
    const auto uniqueId = std::string{"TODO"}; //"id_" + std::to_string(hg::util::GetRandomNumber<int>(10'000, 99'999));
    lobbyFrontendMgr->setToClientMode(nameInLobby, uniqueId);

    context->attachAndOwnComponent(std::move(lobbyFrontendMgr));

    // AUTHORIZATION MANAGER

    auto authMgr = std::make_unique<spe::DefaultAuthorizationManager>(context->getQAORuntime().nonOwning(),
                                                                      PRIORITY_AUTHMGR);

    authMgr->setToClientMode();

    context->attachAndOwnComponent(std::move(authMgr));

    // MAIN GAMEPLAY MANAGER

    auto gpMgr = std::make_unique<MainGameplayManager>(context->getQAORuntime().nonOwning(),
                                                       PRIORITY_GAMEPLAYMGR);
    context->attachAndOwnComponent(std::move(gpMgr));

    // ENVIRONMENT MANAGER

    auto envMgr = std::make_unique<EnvironmentManager>(context->getQAORuntime().nonOwning(),
                                                       PRIORITY_ENVIRONMENTMGR);
    envMgr->setToClientMode();
    context->attachAndOwnComponent(std::move(envMgr));

    // FINISH

    return context;
}

} // namespace hobrobot