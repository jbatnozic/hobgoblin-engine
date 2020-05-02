
#include <Hobgoblin/ChipmunkPhysics.hpp>
#include <SFML/System.hpp>

#include "GameContext/Game_context.hpp"
#include "GameObjects/Framework/Execution_priorities.hpp"
#include "GameObjects/Gameplay/PhysicsPlayer.hpp"
#include "GameObjects/Managers/Main_game_controller.hpp"

MainGameController::MainGameController(QAO_RuntimeRef runtimeRef)
    : GOF_NonstateObject{runtimeRef, TYPEID_SELF, EXEPR_MAIN_GAME_CONTROLLER, "MainGameController"}
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
    auto& view = ctx().windowMgr.getView();

    const bool left  = kbi().keyPressed(KbKey::J);
    const bool right = kbi().keyPressed(KbKey::L);
    const bool up    = kbi().keyPressed(KbKey::I);
    const bool down  = kbi().keyPressed(KbKey::K);

    if (!left && !right && !up && !down) return;

    const float moveSpeed = 16.f;
    
    view.move({moveSpeed * static_cast<float>(right - left), moveSpeed * static_cast<float>(down - up)});
}

void MainGameController::eventPostUpdate() {
    cpSpaceStep(ctx().getPhysicsSpace(), 1.0 / 60.0); // TODO Temp. - Magic number
}

void MainGameController::onNetworkingEvent(const RN_Event& event_) {
    event_.visit(
        [](const RN_Event::BadPassphrase& ev) {
        },
        [](const RN_Event::ConnectAttemptFailed& ev) {
        },
        [this](const RN_Event::Connected& ev) {
            if (ctx().isPrivileged()) {
                PhysicsPlayer::ViState vs;
                vs.playerIndex = *ev.clientIndex + 1;
                vs.x = 70.f;
                vs.y = 70.f;
                QAO_PCreate<PhysicsPlayer>(getRuntime(), ctx().syncObjMgr, SYNC_ID_CREATE_MASTER, vs);
                //QAO_PCreate<Player>(getRuntime(), ctx().syncObjMgr, SYNC_ID_CREATE_MASTER,
                //                    200.f, 200.f, *ev.clientIndex + 1);
            }
        },
        [](const RN_Event::Disconnected& ev) {
        }
        );
}
