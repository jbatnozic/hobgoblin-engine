
#include "Lobby_frontend_manager.hpp"

#include <Hobgoblin/Logging.hpp>
#include <Hobgoblin/RigelNet_macros.hpp>
#include <Hobgoblin/Utility/No_copy_no_move.hpp>

namespace {
constexpr auto LOG_ID = "LobbyFrontendManager";

struct PlayerInfoModel {
    Rml::String name;
    Rml::String uuid;
    Rml::String ip;
};

struct DualPlayerInfoModel {
    PlayerInfoModel lockedIn;
    PlayerInfoModel pending;
    bool showPending = false;
};

struct LobbyModel {
    std::vector<DualPlayerInfoModel> players;
    Rml::String localName;
    bool isAuthorized = false;
};
} // namespace

#define COMMAND_LOCK_IN 0
#define COMMAND_RESET   1
#define COMMAND_MOVE_UP 2
#define COMMAND_MOVE_DN 3
#define COMMAND_KICK    4

void ActivateCommand(LobbyFrontendManager& aMgr, int aCommand, void* aArgs) {
    auto& lobbyBackendMgr = aMgr.ccomp<MLobbyBackend>();
    switch (aCommand) {
        case COMMAND_LOCK_IN:
            HG_LOG_INFO(LOG_ID, "Locking in lobby.");
            lobbyBackendMgr.lockInPendingChanges();
            break;

        case COMMAND_RESET:
            HG_LOG_INFO(LOG_ID, "Resetting lobby.");
            lobbyBackendMgr.resetPendingChanges();
            break;

        case COMMAND_MOVE_UP:
            {
                const auto size = lobbyBackendMgr.getSize();
                auto slotIndex1 = *static_cast<hg::PZInteger*>(aArgs);
                if (slotIndex1 == 0) break;
                auto slotIndex2 = (slotIndex1 + size - 1) % size;
                if (slotIndex2 == 0) slotIndex2 = (slotIndex1 + size - 2) % size;
                if (slotIndex2 == 0) break;

                HG_LOG_INFO(LOG_ID, "Swapping slots {} and {}.", slotIndex1, slotIndex2);
                lobbyBackendMgr.beginSwap(slotIndex1, slotIndex2);
            }
            break;

        case COMMAND_MOVE_DN:
            {
                const auto size = lobbyBackendMgr.getSize();
                auto slotIndex1 = *static_cast<hg::PZInteger*>(aArgs);
                if (slotIndex1 == 0) break;
                auto slotIndex2 = (slotIndex1 + 1) % size;
                if (slotIndex2 == 0) slotIndex2 = (slotIndex1 + 2) % size;
                if (slotIndex2 == 0) break;

                HG_LOG_INFO(LOG_ID, "Swapping slots {} and {}.", slotIndex1, slotIndex2);
                lobbyBackendMgr.beginSwap(slotIndex1, slotIndex2);
            }
            break;

        case COMMAND_KICK:
            {
                // 
            }
            break;

        default:
            break;
    }
}

namespace {
RN_DEFINE_RPC(LobbyFrontendManager_LockInLobby, RN_ARGS(std::string&, aAuthToken)) {
    RN_NODE_IN_HANDLER().callIfServer(
        [&](RN_ServerInterface& aServer) {
        const spe::RPCReceiverContext rc{aServer};
        auto& authMgr = rc.gameContext.getComponent<spe::AuthorizationManagerInterface>();
        if (aAuthToken != *authMgr.getLocalAuthToken()) {
            throw RN_IllegalMessage();
        }
        ActivateCommand(
            dynamic_cast<LobbyFrontendManager&>(
                rc.gameContext.getComponent<LobbyFrontendManagerInterface>()),
            COMMAND_LOCK_IN,
            nullptr
        );
    });
    RN_NODE_IN_HANDLER().callIfClient(
        [](RN_ClientInterface&) {
        throw RN_IllegalMessage();
    });
}

RN_DEFINE_RPC(LobbyFrontendManager_ResetLobby, RN_ARGS(std::string&, aAuthToken)) {
    RN_NODE_IN_HANDLER().callIfServer(
        [&](RN_ServerInterface& aServer) {
        const spe::RPCReceiverContext rc{aServer};
        auto& authMgr = rc.gameContext.getComponent<spe::AuthorizationManagerInterface>();
        if (aAuthToken != *authMgr.getLocalAuthToken()) {
            throw RN_IllegalMessage();
        }
        ActivateCommand(
            dynamic_cast<LobbyFrontendManager&>(
                rc.gameContext.getComponent<LobbyFrontendManagerInterface>()),
            COMMAND_RESET,
            nullptr
        );
    });
    RN_NODE_IN_HANDLER().callIfClient(
        [](RN_ClientInterface&) {
        throw RN_IllegalMessage();
    });
}

RN_DEFINE_RPC(LobbyFrontendManager_MoveUp, RN_ARGS(std::string&, aAuthToken, hg::PZInteger, aSlotIndex)) {
    RN_NODE_IN_HANDLER().callIfServer(
        [&](RN_ServerInterface& aServer) {
        const spe::RPCReceiverContext rc{aServer};
        auto& authMgr = rc.gameContext.getComponent<spe::AuthorizationManagerInterface>();
        if (aAuthToken != *authMgr.getLocalAuthToken()) {
            throw RN_IllegalMessage();
        }
        ActivateCommand(
            dynamic_cast<LobbyFrontendManager&>(
                rc.gameContext.getComponent<LobbyFrontendManagerInterface>()),
            COMMAND_MOVE_UP,
            &aSlotIndex
        );
    });
    RN_NODE_IN_HANDLER().callIfClient(
        [](RN_ClientInterface&) {
        throw RN_IllegalMessage();
    });
}

RN_DEFINE_RPC(LobbyFrontendManager_MoveDown, RN_ARGS(std::string&, aAuthToken, hg::PZInteger, aSlotIndex)) {
    RN_NODE_IN_HANDLER().callIfServer(
        [&](RN_ServerInterface& aServer) {
        const spe::RPCReceiverContext rc{aServer};
        auto& authMgr = rc.gameContext.getComponent<spe::AuthorizationManagerInterface>();
        if (aAuthToken != *authMgr.getLocalAuthToken()) {
            throw RN_IllegalMessage();
        }
        ActivateCommand(
            dynamic_cast<LobbyFrontendManager&>(
                rc.gameContext.getComponent<LobbyFrontendManagerInterface>()),
            COMMAND_MOVE_DN,
            &aSlotIndex
        );
    });
    RN_NODE_IN_HANDLER().callIfClient(
        [](RN_ClientInterface&) {
        throw RN_IllegalMessage();
    });
}

RN_DEFINE_RPC(LobbyFrontendManager_Kick, RN_ARGS(std::string&, aAuthToken, hg::PZInteger, aSlotIndex)) {
    RN_NODE_IN_HANDLER().callIfServer(
        [&](RN_ServerInterface& aServer) {
        const spe::RPCReceiverContext rc{aServer};
        auto& authMgr = rc.gameContext.getComponent<spe::AuthorizationManagerInterface>();
        if (aAuthToken != *authMgr.getLocalAuthToken()) {
            throw RN_IllegalMessage();
        }
        ActivateCommand(
            dynamic_cast<LobbyFrontendManager&>(
                rc.gameContext.getComponent<LobbyFrontendManagerInterface>()),
            COMMAND_KICK,
            &aSlotIndex
        );
    });
    RN_NODE_IN_HANDLER().callIfClient(
        [](RN_ClientInterface&) {
        throw RN_IllegalMessage();
    });
}
} // namespace

///////////////////////////////////////////////////////////////////////////
// IMPL                                                                  //
///////////////////////////////////////////////////////////////////////////

class LobbyFrontendManager::Impl: hg::util::NonCopyable, hg::util::NonMoveable {
public:
#define CTX   _super.ctx
#define CCOMP _super.ccomp

    explicit Impl(LobbyFrontendManager& aLobbyFrontendManager)
        : _super{aLobbyFrontendManager}
    {
    }

    ~Impl() {
        if (_document) {
            CCOMP<MWindow>().getGUIContext().UnloadDocument(_document);
            _document = nullptr;
        }
    }

    void setToHeadlessHostMode() {
        SPEMPE_VALIDATE_GAME_CONTEXT_FLAGS(CTX(), headless==true);
        _mode = Mode::HeadlessHost;
    }

    void setToClientMode(const std::string& aName, const std::string& aUniqueId) {
        SPEMPE_VALIDATE_GAME_CONTEXT_FLAGS(CTX(), headless==false);
        _mode = Mode::Client;

        {
            auto& lobbyBackendMgr = CCOMP<MLobbyBackend>();
            lobbyBackendMgr.setLocalName(aName);
            lobbyBackendMgr.setLocalUniqueId(aUniqueId);
        }

        auto& guiContext = CCOMP<MWindow>().getGUIContext();
        auto handle = _setUpDataBinding(guiContext);
        _dataModelHandle = *handle; // TODO

        hg::rml::PreprocessRcssFile("assets/lobby.rcss.fp");
        _document = guiContext.LoadDocument("assets/lobby.rml");
        if (_document) {
            _document->Show();
            _documentVisible = true;
            HG_LOG_INFO(LOG_ID, "RMLUI Document loaded successfully.");
        }
        else {
            HG_LOG_ERROR(LOG_ID, "RMLUI Document could not be loaded.");
        }
    }

    Mode getMode() const {
        return _mode;
    }

    void eventPreUpdate() {
        auto& lobbyBackendMgr = CCOMP<MLobbyBackend>();
        spe::LobbyBackendEvent ev;
        while (lobbyBackendMgr.pollEvent(ev)) {
            ev.strictVisit(
                [this](spe::LobbyBackendEvent::LobbyLockedIn& aEvData) {
                    HG_LOG_INFO(LOG_ID, "(event) Lobby locked in.");
                    if (_mode == Mode::Client && aEvData.somethingDidChange) {
                        _document->Show();
                        _documentVisible = true;
                    }
                },
                [this](spe::LobbyBackendEvent::LobbyChanged&) {
                    HG_LOG_INFO(LOG_ID, "(event) Lobby changed.");
                    if (_mode == Mode::Client) {
                        _document->Show();
                        _documentVisible = true;
                    }
                }
            );
        }
    }

    void eventUpdate() {
        if (_mode != Mode::Client) {
            return;
        }

        auto& winMgr = CCOMP<MWindow>();
        if (winMgr.getInput().checkPressed(hg::in::PK_L, spe::WindowFrameInputView::Mode::Edge)) {
            _documentVisible = !_documentVisible;
            if (_documentVisible) {
                _document->Show();
            }
            else {
                _document->Hide();
            }
        }
    }

    void eventDrawGUI() {
        if (_mode != Mode::Client) {
            return;
        }

        const auto& lobbyBackendMgr = CCOMP<MLobbyBackend>();

        _lobbyModel.players.resize(hg::pztos(lobbyBackendMgr.getSize()));
        for (hg::PZInteger i = 0; i < lobbyBackendMgr.getSize(); i += 1) {
            const auto& lockedIn = lobbyBackendMgr.getLockedInPlayerInfo(i);
            _lobbyModel.players[hg::pztos(i)].lockedIn = 
                PlayerInfoModel{
                (!lockedIn.name.empty()) ? lockedIn.name : "<empty>",
                lockedIn.uniqueId,
                lockedIn.ipAddress
            };

            if (lobbyBackendMgr.areChangesPending(i)) {
                _lobbyModel.players[hg::pztos(i)].showPending = true;
                const auto& pending = lobbyBackendMgr.getPendingPlayerInfo(i);
                _lobbyModel.players[hg::pztos(i)].pending = 
                    PlayerInfoModel{
                    (!pending.name.empty()) ? pending.name : "<empty>",
                    pending.uniqueId,
                    pending.ipAddress
                };
            } else {
                _lobbyModel.players[hg::pztos(i)].showPending = false;
            }
        }

        _lobbyModel.localName = lobbyBackendMgr.getLocalName();
        _lobbyModel.isAuthorized = CCOMP<spe::AuthorizationManagerInterface>().getLocalAuthToken().has_value();

        _dataModelHandle.DirtyAllVariables();
    }

private:
    LobbyFrontendManager& _super;

    Mode _mode = Mode::Uninitialized;

    Rml::ElementDocument* _document = nullptr;
    LobbyModel _lobbyModel;
    Rml::DataModelHandle _dataModelHandle;

    bool _documentVisible = false;

    void _onLockInClicked(Rml::DataModelHandle, Rml::Event&, const Rml::VariantList&) {
        if (auto authToken = CCOMP<spe::AuthorizationManagerInterface>().getLocalAuthToken()) {
            Compose_LobbyFrontendManager_LockInLobby(
                CCOMP<MNetworking>().getNode(),
                RN_COMPOSE_FOR_ALL,
                *authToken
            );
        }        
    }

    void _onResetClicked(Rml::DataModelHandle, Rml::Event&, const Rml::VariantList&) {
        if (auto authToken = CCOMP<spe::AuthorizationManagerInterface>().getLocalAuthToken()) {
            Compose_LobbyFrontendManager_ResetLobby(
                CCOMP<MNetworking>().getNode(),
                RN_COMPOSE_FOR_ALL,
                *authToken
            );
        }        
    }

    void _onMoveUpClicked(Rml::DataModelHandle, Rml::Event&, const Rml::VariantList& aArguments) {
        if (auto authToken = CCOMP<spe::AuthorizationManagerInterface>().getLocalAuthToken()) {
            Compose_LobbyFrontendManager_MoveUp(
                CCOMP<MNetworking>().getNode(),
                RN_COMPOSE_FOR_ALL,
                *authToken,
                aArguments.at(0).Get<int>(-1)
            );
        }  
    }

    void _onMoveDownClicked(Rml::DataModelHandle, Rml::Event&, const Rml::VariantList& aArguments) {
        if (auto authToken = CCOMP<spe::AuthorizationManagerInterface>().getLocalAuthToken()) {
            Compose_LobbyFrontendManager_MoveDown(
                CCOMP<MNetworking>().getNode(),
                RN_COMPOSE_FOR_ALL,
                *authToken,
                aArguments.at(0).Get<int>(-1)
            );
        }  
    }

    void _onKickClicked(Rml::DataModelHandle, Rml::Event&, const Rml::VariantList& aArguments) {
        if (auto authToken = CCOMP<spe::AuthorizationManagerInterface>().getLocalAuthToken()) {

        }  
    }

    std::optional<Rml::DataModelHandle> _setUpDataBinding(Rml::Context& aContext) {
        Rml::DataModelConstructor constructor = aContext.CreateDataModel("lobby-model");
        if (!constructor) {
            return {};
        }

        if (auto handle = constructor.RegisterStruct<PlayerInfoModel>()) {
            handle.RegisterMember("name", &PlayerInfoModel::name);
            handle.RegisterMember("uuid", &PlayerInfoModel::uuid);
            handle.RegisterMember("ip",   &PlayerInfoModel::ip);
        }
        else {
            return {};
        }

        if (auto handle = constructor.RegisterStruct<DualPlayerInfoModel>()) {
            handle.RegisterMember("lockedIn",    &DualPlayerInfoModel::lockedIn);
            handle.RegisterMember("pending",     &DualPlayerInfoModel::pending);
            handle.RegisterMember("showPending", &DualPlayerInfoModel::showPending);
        }
        else {
            return {};
        }

        constructor.RegisterArray<std::vector<DualPlayerInfoModel>>();

        if (auto handle = constructor.RegisterStruct<LobbyModel>()) {
            handle.RegisterMember("players", &LobbyModel::players);
        }
        else {
            return {};
        }

        constructor.Bind("players", &_lobbyModel.players);
        constructor.Bind("localName", &_lobbyModel.localName);
        constructor.Bind("isAuthorized", &_lobbyModel.isAuthorized);

        constructor.BindEventCallback(
            "LockIn",
            &Impl::_onLockInClicked,
            this
        );
        constructor.BindEventCallback(
            "Reset",
            &Impl::_onResetClicked,
            this
        );
        constructor.BindEventCallback(
            "MoveUp",
            &Impl::_onMoveUpClicked,
            this
        );
        constructor.BindEventCallback(
            "MoveDown",
            &Impl::_onMoveDownClicked,
            this
        );
        constructor.BindEventCallback(
            "Kick",
            &Impl::_onKickClicked,
            this
        );

        return constructor.GetModelHandle();
    }

#undef CCOMP
#undef CTX
};

///////////////////////////////////////////////////////////////////////////
// LOBBY FRONTEND MANAGER                                                //
///////////////////////////////////////////////////////////////////////////

LobbyFrontendManager::LobbyFrontendManager(QAO_RuntimeRef aRuntimeRef, int aExecutionPriority)
    : NonstateObject(aRuntimeRef, SPEMPE_TYPEID_SELF, aExecutionPriority, "LobbyFrontendManager")
    , _impl{std::make_unique<Impl>(*this)}
{
}

LobbyFrontendManager::~LobbyFrontendManager() = default;

void LobbyFrontendManager::setToHeadlessHostMode() {
    _impl->setToHeadlessHostMode();
}

void LobbyFrontendManager::setToClientMode(const std::string& aName, const std::string& aUniqueId) {
    _impl->setToClientMode(aName, aUniqueId);
}

LobbyFrontendManager::Mode LobbyFrontendManager::getMode() const {
    return _impl->getMode();
}

void LobbyFrontendManager::_eventPreUpdate() {
    _impl->eventPreUpdate();
}

void LobbyFrontendManager::_eventUpdate() {
    _impl->eventUpdate();
}

void LobbyFrontendManager:: _eventDrawGUI() {
#if 0
    if (auto* winMgr = ctx().getComponentPtr<MWindow>()) {
        auto& lobbyBackendMgr = ccomp<MLobby>();

        // Old
        auto& canvas = winMgr->getCanvas();
        sf::Text text;
        text.setPosition(32.f, 32.f);
        text.setFont(hg::gr::BuiltInFonts::getFont(hg::gr::BuiltInFonts::TitilliumRegular));
        text.setFillColor(sf::Color::Yellow);
        text.setString(lobbyBackendMgr.getEntireStateString());
        canvas.draw(text);
    }
#endif
    _impl->eventDrawGUI();
}
