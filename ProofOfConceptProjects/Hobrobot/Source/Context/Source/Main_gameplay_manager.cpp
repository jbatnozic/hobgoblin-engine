
#include <HobRobot/Context/Managers/Main_gameplay_manager.hpp>

#include <HobRobot/Common/Player_controls.hpp>

#include <Hobgoblin/Logging.hpp>
#include <Hobgoblin/Graphics.hpp>

namespace hobrobot {

namespace {
constexpr auto LOG_ID = "Hobrobot";
} // namespace

RN_DEFINE_RPC(SetGlobalStateBufferingLength, RN_ARGS(unsigned, aNewLength)) {
    RN_NODE_IN_HANDLER().callIfClient(
        [=](RN_ClientInterface& aClient) {
            const auto rc = spe::RPCReceiverContext(aClient);
            rc.gameContext.getComponent<MNetworking>().setStateBufferingLength(aNewLength);
            rc.gameContext.getComponent<MInput>().setStateBufferingLength(aNewLength);
            HG_LOG_INFO(LOG_ID, "Global state buffering set to {} frames.", aNewLength);
        });
    RN_NODE_IN_HANDLER().callIfServer(
        [](RN_ServerInterface&) {
            throw RN_IllegalMessage();
        });
}

RN_DEFINE_RPC(TogglePause, RN_ARGS(spe::AuthToken&, aAuthToken)) {
    RN_NODE_IN_HANDLER().callIfClient(
        [](RN_ClientInterface&) {
            throw RN_IllegalMessage();
        });
    RN_NODE_IN_HANDLER().callIfServer(
        [&](RN_ServerInterface& aServer) {
            const spe::RPCReceiverContext rc{aServer};
            auto& authMgr = rc.gameContext.getComponent<spe::AuthorizationManagerInterface>();
            if (aAuthToken != *authMgr.getLocalAuthToken()) {
                throw RN_IllegalMessage();
            }
            rc.gameContext.getGameState().isPaused ^= true;
        });
}

MainGameplayManager::MainGameplayManager(QAO_RuntimeRef aRuntimeRef, int aExecutionPriority)
    : NonstateObject{aRuntimeRef, SPEMPE_TYPEID_SELF, aExecutionPriority, "GameplayManager"}
{
    auto& netMgr = ccomp<MNetworking>();
    netMgr.addEventListener(this);
    stateBufferingLength = netMgr.getStateBufferingLength();

    if (ctx().isPrivileged()) {
        ctx().getGameState().isPaused = true;
    }
}

MainGameplayManager::~MainGameplayManager() {
    ccomp<MNetworking>().removeEventListener(this);
}

void MainGameplayManager::onNetworkingEvent(const hg::RN_Event& aEvent) {
    if (ccomp<MNetworking>().isClient()) {
        // CLIENT
        aEvent.visit(
            [this](const RN_Event::Connected& ev) {
                HG_LOG_INFO(LOG_ID, "Client lobby uploading local info to server.");
                ccomp<MLobbyBackend>().uploadLocalInfo();
            }
        );
    }
    else {
        // HOST
        aEvent.visit(
            [this](const RN_Event::Connected& ev) {
            },
            [](const RN_Event::Disconnected& ev) {
                // TODO Remove player avatar
            }
        );
    }
}

void MainGameplayManager::_eventUpdate1() {
    if (ctx().isPrivileged()) {
        auto& winMgr = ccomp<MWindow>();

        const int MAX_BUFFERING_LENGTH = 10;
        bool sync = false;

        if (winMgr.getInput().checkPressed(hg::in::PK_NUMPAD_PLUS,
                                           spe::WindowFrameInputView::Mode::Direct)) {
            stateBufferingLength = (stateBufferingLength + 1) % (MAX_BUFFERING_LENGTH + 1);
            sync = true;
        }
        if (winMgr.getInput().checkPressed(hg::in::PK_NUMPAD_MINUS,
                                           spe::WindowFrameInputView::Mode::Direct)) {
            stateBufferingLength = (stateBufferingLength + MAX_BUFFERING_LENGTH) % (MAX_BUFFERING_LENGTH + 1);
            sync = true;
        }

        if (sync) {
            HG_LOG_INFO(LOG_ID, "Global state buffering set to {} frames.", stateBufferingLength);
            Compose_SetGlobalStateBufferingLength(
                ccomp<MNetworking>().getNode(),
                RN_COMPOSE_FOR_ALL,
                stateBufferingLength
            );
        }
        
        return;
    }

    if (!ctx().isPrivileged()) {
        auto& client = ccomp<MNetworking>().getClient();
        if (client.getServerConnector().getStatus() == RN_ConnectorStatus::Connected) {
            const auto& winMgr = ccomp<MWindow>();

            const auto pos = winMgr.getInput().getViewRelativeMousePos(0);
            const auto kbIn = winMgr.getInput();

            spe::InputSyncManagerWrapper wrapper{ccomp<MInput>()};
            wrapper.setSignalValue<float>(CTRLNAME_CURSOR_X, pos.x);
            wrapper.setSignalValue<float>(CTRLNAME_CURSOR_Y, pos.y);
            wrapper.setSignalValue<bool>(CTRLNAME_MIN_EXT, kbIn.checkPressed(hg::in::PK_Q));
            wrapper.setSignalValue<bool>(CTRLNAME_MAX_EXT, kbIn.checkPressed(hg::in::PK_E));
                                          
            auto& authMgr = ccomp<MAuth>();
            if (winMgr.getInput().checkPressed(hg::in::PK_P, spe::WindowFrameInputView::Mode::Direct) &&
                authMgr.getLocalAuthToken().has_value()) {

                Compose_TogglePause(client, RN_COMPOSE_FOR_ALL, *authMgr.getLocalAuthToken());
            }
        }
    }
}

void MainGameplayManager::_eventPostUpdate() {
    const auto input = ccomp<MWindow>().getInput();
    if (input.checkPressed(hg::in::PK_F9, spe::WindowFrameInputView::Mode::Direct)) {
        // Stopping the context will delete:
        // - All objects owned by the QAO runtime (in undefined order)
        // - Then, all ContextComponents owned by the context (in reverse order of insertion)
        ctx().stop();
    }
}

void MainGameplayManager::_eventDraw1() {
    if (!ctx().isHeadless()) {
        auto& winMgr = ccomp<MWindow>();
        auto& view = winMgr.getView();
        auto& canvas = winMgr.getCanvas();

        hg::gr::RectangleShape rect;
        rect.setSize({2000.f, 2000.f});
        rect.setOrigin({1000.f, 1000.f});
        rect.setPosition(view.getCenter());
        rect.setFillColor(hg::gr::Color(35, 26, 46));
        canvas.draw(rect);
    }
}

void MainGameplayManager::_eventDrawGUI() {
    // Do nothing
}

} // namespace hobrobot
