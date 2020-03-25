
#include <SFML/System.hpp>

#include "Controls_manager.hpp"
#include "Global_program_state.hpp"

#include <iostream>

RN_DEFINE_HANDLER(SetClientControls, RN_ARGS(PlayerControls&, controls)) {
    RN_NODE_IN_HANDLER().visit(
        [](NetworkingManager::ClientType& client) {
            // ERROR
        },
        [&](NetworkingManager::ServerType& server) {
            auto& global = *server.getUserData<GlobalProgramState>();
            auto& controlsMgr = global.controlsMgr;

            const auto clientIndex = server.getSenderIndex();
            const std::chrono::microseconds delay = server.getClient(clientIndex).getRemoteInfo().latency / 2LL;

            controlsMgr.putNewControls(server.getSenderIndex() + 1, controls, delay);
        }
    );
}

ControlsManager::ControlsManager(QAO_Runtime* runtime, hg::PZInteger playerCount, hg::PZInteger inputDelayInSteps)
    : GOF_Base{runtime, TYPEID_SELF, 30, "ControlsManager"} // Run after NetMgr
{
    _controls.reserve(playerCount);
    for (hg::PZInteger i = 0; i < playerCount; i += 1) {
        _controls.emplace_back(inputDelayInSteps);
    }
}

void ControlsManager::resetWithInputDelay(hg::PZInteger inputDelay) {
    // TODO (Can probably copy contructor code)
}

PlayerControls ControlsManager::getCurrentControlsForPlayer(hg::PZInteger playerIndex) {
    return _controls[static_cast<std::size_t>(playerIndex)].getCurrentControls();
}

void ControlsManager::putNewControls(hg::PZInteger playerIndex, const PlayerControls& controls,
                                     std::chrono::microseconds delay) {
    // TODO Temp. implementation
    _controls[playerIndex].putNewControls(controls, delay);
}

void ControlsManager::eventPreUpdate() {
    if (global().playerIndex == -1) {
        return;
    }

    // Local controls:
    auto& sch = _controls[global().playerIndex];
    bool focus = global().windowMgr.window.hasFocus();
    sch.putNewControls(PlayerControls{focus && sf::Keyboard::isKeyPressed(sf::Keyboard::A),
                                      focus && sf::Keyboard::isKeyPressed(sf::Keyboard::D),
                                      focus && sf::Keyboard::isKeyPressed(sf::Keyboard::W)},
                       std::chrono::microseconds{0});

    for (auto& sch : _controls) {
        sch.integrateNewControls();
    }
}

void ControlsManager::eventUpdate() {
    if (global().playerIndex > 0 && 
        global().netMgr.getClient().getServer().getStatus() == RN_ConnectorStatus::Connected) {
        auto& sch = _controls[global().playerIndex];
        RN_Compose_SetClientControls(global().netMgr.getClient(), 0, sch.getLatestControls());
    }
}

void ControlsManager::eventPostUpdate() {
    for (auto& sch : _controls) {
        sch.advanceStep();
    }
}