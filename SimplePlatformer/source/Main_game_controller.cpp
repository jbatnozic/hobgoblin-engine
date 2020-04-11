
#include "Global_program_state.hpp"
#include "Main_game_controller.hpp"

MainGameController::MainGameController(QAO_RuntimeRef runtimeRef)
    : GOF_NonstateObject{runtimeRef, TYPEID_SELF, 0, "MainGameController"}
{
    global().netMgr.addEventListener(this);
}

MainGameController::~MainGameController() {
    global().netMgr.removeEventListener(this);
}

void MainGameController::onNetworkingEvent(const RN_Event& event_) {
    event_.visit(
        [](const RN_Event::BadPassphrase& ev) {
        },
        [](const RN_Event::ConnectAttemptFailed& ev) {
        },
        [this](const RN_Event::Connected& ev) {
            if (global().isHost()) {
                QAO_PCreate<Player>(getRuntime(), global().syncObjMgr, 200.f, 200.f, *ev.clientIndex + 1);
            }
        },
        [](const RN_Event::Disconnected& ev) {
        }
        );
}