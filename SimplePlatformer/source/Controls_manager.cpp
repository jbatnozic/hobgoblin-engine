
#include <SFML/System.hpp>

#include "Controls_manager.hpp"
#include "Game_context.hpp"

#include <iostream>

RN_DEFINE_HANDLER(SetClientControls, RN_ARGS(PlayerControls&, controls)) {
    RN_NODE_IN_HANDLER().visit(
        [](NetworkingManager::ClientType& client) {
            // ERROR
        },
        [&](NetworkingManager::ServerType& server) {
            auto& global = *server.getUserData<GameContext>();
            auto& controlsMgr = global.controlsMgr;

            const auto clientIndex = server.getSenderIndex();
            const std::chrono::microseconds delay = server.getClient(clientIndex).getRemoteInfo().latency / 2LL;

            controlsMgr.putNewControls(server.getSenderIndex() + 1, controls, delay);
        }
    );
}

ControlsManager::ControlsManager(QAO_RuntimeRef runtimeRef, hg::PZInteger playerCount, 
                                 hg::PZInteger inputDelayInSteps, hg::PZInteger historySize)
    : GOF_Base{runtimeRef, TYPEID_SELF, 30, "ControlsManager"} // Run after NetMgr
{
    _schedulers.reserve(playerCount);
    for (hg::PZInteger i = 0; i < playerCount; i += 1) {
        _schedulers.emplace_back(inputDelayInSteps, historySize);
    }
}

void ControlsManager::setInputDelay(hg::PZInteger inputDelayInSteps, hg::PZInteger historySize) {
    for (auto& scheduler : _schedulers) {
        scheduler.reset(inputDelayInSteps, historySize);
    }
}

PlayerControls ControlsManager::getCurrentControlsForPlayer(hg::PZInteger playerIndex) {
    return _schedulers[static_cast<std::size_t>(playerIndex)].getCurrentState();
}

void ControlsManager::putNewControls(hg::PZInteger playerIndex, const PlayerControls& controls,
                                     std::chrono::microseconds delay) {
    // TODO Temp. implementation
    _schedulers[playerIndex].putNewState(controls, (delay / std::chrono::microseconds{16'666}));
    //_schedulers[playerIndex].putNewState(controls, delay);
}

void ControlsManager::eventPreUpdate() {
    if (ctx().playerIndex == GameContext::PLAYER_INDEX_UNKNOWN ||
        ctx().playerIndex == GameContext::PLAYER_INDEX_NONE) {
        return;
    }

    // Local controls:
    auto& scheduler = _schedulers[ctx().playerIndex];
    bool focus = ctx().windowMgr.getWindow().hasFocus(); //TODO Temp.
    scheduler.putNewState(PlayerControls{focus && sf::Keyboard::isKeyPressed(sf::Keyboard::A),
                                         focus && sf::Keyboard::isKeyPressed(sf::Keyboard::D),
                                         focus && sf::Keyboard::isKeyPressed(sf::Keyboard::W)});

    for (auto& scheduler : _schedulers) {
        scheduler.scheduleNewStates();
    }
}

void ControlsManager::eventUpdate() {
    if (ctx().playerIndex > 0 && 
        ctx().netMgr.getClient().getServer().getStatus() == RN_ConnectorStatus::Connected) {
        auto& scheduler = _schedulers[ctx().playerIndex];
        Compose_SetClientControls(ctx().netMgr.getClient(), 0, scheduler.getLatestState());
    }
}

void ControlsManager::eventPostUpdate() {
    for (auto& scheduler : _schedulers) {
        scheduler.advance();
    }
}