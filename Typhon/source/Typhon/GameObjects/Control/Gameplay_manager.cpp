
#include <Hobgoblin/ChipmunkPhysics.hpp>
#include <Hobgoblin/Graphics.hpp>
#include <SFML/System.hpp>

#include <Typhon/GameObjects/Gameplay/PhysicsPlayer.hpp>

#include <sstream>

#include "Gameplay_manager.hpp"

GameplayManager::GameplayManager(QAO_RuntimeRef runtimeRef)
    : NonstateObject{runtimeRef, TYPEID_SELF, *PEXEPR_GAMEPLAY_MGR, "GameplayManager"}
{
    ctx(MNetworking).addEventListener(this);
}

GameplayManager::~GameplayManager() {
    ctx(MNetworking).removeEventListener(this);
}

void GameplayManager::eventUpdate() {
    if (ctx(DKeyboard).keyPressed(KbKey::Escape)) {
        ctx().stop();
    }

    // Camera movement
    auto& view = ctx(MWindow).getView();

    const bool left  = ctx(DKeyboard).keyPressed(KbKey::J);
    const bool right = ctx(DKeyboard).keyPressed(KbKey::L);
    const bool up    = ctx(DKeyboard).keyPressed(KbKey::I);
    const bool down  = ctx(DKeyboard).keyPressed(KbKey::K);

    if (!left && !right && !up && !down) return;

    const float moveSpeed = 16.f;
    
    view.move({moveSpeed * static_cast<float>(right - left), moveSpeed * static_cast<float>(down - up)});
}

void GameplayManager::eventPostUpdate() {
    cpSpaceStep(ctx(DPhysicsSpace), ctx().getRuntimeConfig().getDeltaTime().count());
}

void GameplayManager::eventDrawGUI() {
    sf::Text text;
    text.setFont(hg::gr::BuiltInFonts::getFont(hg::gr::BuiltInFonts::TitilliumRegular));
    text.setPosition({16.f, 16.f});
    text.setFillColor(hg::gr::Color::White);
    text.setOutlineColor(hg::gr::Color::Black);
    text.setOutlineThickness(1.f);
    text.setCharacterSize(16);
    
    auto& perfInfo = ctx().getPerformanceInfo();
    std::stringstream sstream;
    sstream 
        << "Local context performance (microsec):\n"
        << "    tUpdate: "   << perfInfo.updateAndDrawTime.count() << " (x" << perfInfo.consecutiveUpdateLoops << ")\n"
        << "    tFinalize: " << perfInfo.finalizeTime.count() << '\n'
        << "    tTotal: "    << perfInfo.totalTime.count() << '\n'
        << "    tFTF: "      << perfInfo.frameToFrameTime.count() << "\n\n";

    if (ctx().hasChildContext()) {
        auto& perfInfo = ctx().getChildContext()->getPerformanceInfo();
        sstream
            << "Child context performance (microsec):\n"
            << "    tUpdate: "   << perfInfo.updateAndDrawTime.count() << " (x" << perfInfo.consecutiveUpdateLoops << ")\n"
            << "    tFinalize: " << perfInfo.finalizeTime.count() << '\n'
            << "    tTotal: "    << perfInfo.totalTime.count() << '\n'
            << "    tFTF: "      << perfInfo.frameToFrameTime.count() << "\n\n";
    }

    text.setString(sstream.str());

    ctx(MWindow).getCanvas().draw(text);
}

void GameplayManager::onNetworkingEvent(const RN_Event& event_) {
    event_.visit(
        [](const RN_Event::BadPassphrase& ev) {
        },
        [](const RN_Event::ConnectAttemptFailed& ev) {
        },
        [this](const RN_Event::Connected& ev) {
            if (ctx().isPrivileged()) {
                PhysicsPlayer::VisibleState vs;
                vs.playerIndex = *ev.clientIndex + 1;
                vs.x = 70.f;
                vs.y = 70.f;
                QAO_PCreate<PhysicsPlayer>(getRuntime(), ctx().getSyncObjReg(), SYNC_ID_NEW, vs);
            }
        },
        [](const RN_Event::Disconnected& ev) {
        }
    );
}
