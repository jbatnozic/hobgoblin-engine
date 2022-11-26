
#include "Main_gameplay_manager.hpp"

#include "Player_controls.hpp"

#include <Hobgoblin/Logging.hpp>

namespace {
constexpr auto LOG_ID = "MainGameplayManager";
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

MainGameplayManager::MainGameplayManager(QAO_RuntimeRef aRuntimeRef, int aExecutionPriority)
    : NonstateObject{aRuntimeRef, SPEMPE_TYPEID_SELF, aExecutionPriority, "GameplayManager"}
{
    auto& netMgr = ccomp<MNetworking>();
    netMgr.addEventListener(*this);
    stateBufferingLength = netMgr.getStateBufferingLength();
}

MainGameplayManager::~MainGameplayManager() {
    ccomp<MNetworking>().removeEventListener(*this);
}

void MainGameplayManager::_eventDrawGUI() {
    // Do nothing
}

void MainGameplayManager::_eventFinalizeFrame() {
    const auto kbInput = ccomp<MWindow>().getKeyboardInput();
    if (kbInput.checkPressed(spe::KbKey::F9, spe::KbInput::Mode::Direct)) {
        // Stopping the context will delete:
        // - All objects owned by the QAO runtime (in undefined order)
        // - Then, all ContextComponents owned by the context (in reverse order of insertion)
        ctx().stop();
    }
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

void MainGameplayManager::_eventUpdate() {
    if (ctx().isPrivileged()) {
        auto& winMgr = ccomp<MWindow>();

        const int MAX_BUFFERING_LENGTH = 10;
        bool sync = false;

        if (winMgr.getKeyboardInput().checkPressed(spe::KbKey::Add,
                                                   spe::KbInput::Mode::Direct)) {
            stateBufferingLength = (stateBufferingLength + 1) % (MAX_BUFFERING_LENGTH + 1);
            sync = true;
        }
        if (winMgr.getKeyboardInput().checkPressed(spe::KbKey::Subtract,
                                                   spe::KbInput::Mode::Direct)) {
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
            const auto kbInput = ccomp<MWindow>().getKeyboardInput();
            PlayerControls controls{
                kbInput.checkPressed(spe::KbKey::A),
                kbInput.checkPressed(spe::KbKey::D),
                kbInput.checkPressed(spe::KbKey::W),
                kbInput.checkPressed(spe::KbKey::S),
                kbInput.checkPressed(spe::KbKey::Space, spe::KbInput::Mode::Edge)
            };

            spe::InputSyncManagerWrapper wrapper{ccomp<MInput>()};
            wrapper.setSignalValue<bool>(CTRLNAME_LEFT,  controls.left);
            wrapper.setSignalValue<bool>(CTRLNAME_RIGHT, controls.right);
            wrapper.setSignalValue<bool>(CTRLNAME_UP,    controls.up);
            wrapper.setSignalValue<bool>(CTRLNAME_DOWN,  controls.down);
            wrapper.triggerEvent(CTRLNAME_JUMP, controls.jump);
        }
    }
}
