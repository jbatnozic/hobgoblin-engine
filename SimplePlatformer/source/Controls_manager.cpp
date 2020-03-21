
#include <SFML/System.hpp>

#include "Controls_manager.hpp"
#include "Global_program_state.hpp"

ControlsManager::ControlsManager(QAO_Runtime* runtime, hg::PZInteger size, hg::PZInteger inputDelay)
    : GOF_Base{runtime, TYPEID_SELF, 50, "ControlsManager"}
    , _defaultInputDelay{inputDelay}
{
    _controls.resize(static_cast<std::size_t>(size));
    for (auto& queue : _controls) {
        queue.resize(inputDelay + 1);
    }
}

void ControlsManager::resetWithInputDelay(hg::PZInteger inputDelay) {
    // TODO
}

PlayerControls ControlsManager::getCurrentControlsForPlayer(hg::PZInteger playerIndex) {
    return _controls[static_cast<std::size_t>(playerIndex)][0];
}

void ControlsManager::overwriteControls(const PlayerControls& controls, hg::PZInteger delayBy) {
    // TODO
}

void ControlsManager::eventPreUpdate() {
    auto& queue = _controls[global().playerIndex];
    bool focus = global().windowMgr.window.hasFocus();
    queue.pop_front();
    queue.push_back(PlayerControls{
        focus && sf::Keyboard::isKeyPressed(sf::Keyboard::A),
        focus && sf::Keyboard::isKeyPressed(sf::Keyboard::D),
        focus && sf::Keyboard::isKeyPressed(sf::Keyboard::W)
    });
    
}