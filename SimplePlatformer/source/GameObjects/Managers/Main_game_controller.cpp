
#include <SFML/System.hpp>

#include "GameContext/Game_context.hpp"
#include "GameObjects/Managers/Main_game_controller.hpp"

MainGameController::MainGameController(QAO_RuntimeRef runtimeRef)
    : GOF_NonstateObject{runtimeRef, TYPEID_SELF, 0, "MainGameController"}
{
    ctx().netMgr.addEventListener(this);
}

MainGameController::~MainGameController() {
    ctx().netMgr.removeEventListener(this);
}

void MainGameController::eventUpdate() {
    if (kbi().keyPressed(KbKey::Escape)) {
        ctx().stop();
    }

    // Camera movement
    const bool left  = kbi().keyPressed(KbKey::J);
    const bool right = kbi().keyPressed(KbKey::L);
    const bool up    = kbi().keyPressed(KbKey::I);
    const bool down  = kbi().keyPressed(KbKey::K);

    if (!left && !right && !up && !down) return;

    const float moveSpeed = 16.f;
    auto& view = ctx().windowMgr.getView();
    view.move({moveSpeed * static_cast<float>(right - left), moveSpeed * static_cast<float>(down - up)});
}

void MainGameController::onNetworkingEvent(const RN_Event& event_) {
    event_.visit(
        [](const RN_Event::BadPassphrase& ev) {
        },
        [](const RN_Event::ConnectAttemptFailed& ev) {
        },
        [this](const RN_Event::Connected& ev) {
            if (ctx().isPrivileged()) {
                QAO_PCreate<Player>(getRuntime(), ctx().syncObjMgr, SYNC_ID_CREATE_MASTER,
                                    200.f, 200.f, *ev.clientIndex + 1);
            }
        },
        [](const RN_Event::Disconnected& ev) {
        }
        );
}
