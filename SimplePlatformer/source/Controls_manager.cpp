
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

            const hg::PZInteger clientIndex = server.getSenderIndex();
            const hg::PZInteger delay =
                (server.getClient(clientIndex).getRemoteInfo().latency / std::chrono::microseconds{2 * 16'666}); // TODO Hardcoded value
            controlsMgr.overwriteControls(server.getSenderIndex() + 1, controls, 4 - delay);
            //std::cout << "c " << delay << '\n';
        }
    );
}

ControlsManager::ControlsManager(QAO_Runtime* runtime, hg::PZInteger size, hg::PZInteger inputDelay)
    : GOF_Base{runtime, TYPEID_SELF, 60, "ControlsManager"}
    , _defaultInputDelay{inputDelay}
{
    _controls.resize(static_cast<std::size_t>(size));
    for (auto& queue : _controls) {
        queue.resize(inputDelay + 1);
    }
    _offsets.resize(static_cast<std::size_t>(size));
}

void ControlsManager::resetWithInputDelay(hg::PZInteger inputDelay) {
    // TODO
}

PlayerControls ControlsManager::getCurrentControlsForPlayer(hg::PZInteger playerIndex) {
    return _controls[static_cast<std::size_t>(playerIndex)][0];
}

void ControlsManager::overwriteControls(hg::PZInteger playerIndex, const PlayerControls& controls,
                                        hg::PZInteger delayBy) {
    // TODO Temp. implementation
    auto& queue = _controls[playerIndex];

    //int i = delayBy + _offsets[playerIndex];
    //if (i < 0 || i >= queue.size()) {
    //    // Ignore
    //}
    //else {
    //    queue[i] = controls;
    //}

    //_offsets[playerIndex] += 1;

    queue.push_back(controls);
}

void ControlsManager::eventPreUpdate() {
    if (global().playerIndex == -1) {
        return;
    }

    for (auto& i : _offsets) {
        i = 0;
    }

    if (global().playerIndex == 0) {
        // SERVER
        int i = 0;
        for (auto& queue : _controls) {
            if (i == global().playerIndex) {
                // Local
                bool focus = global().windowMgr.window.hasFocus();
                queue.pop_front();
                queue.push_back(PlayerControls{
                    focus && sf::Keyboard::isKeyPressed(sf::Keyboard::A),
                    focus && sf::Keyboard::isKeyPressed(sf::Keyboard::D),
                    focus && sf::Keyboard::isKeyPressed(sf::Keyboard::W)
                });
            }
            else {
                // Remote
                if (queue.size() > 3) {
                    queue.resize(1);
                    queue[0] = PlayerControls{};
                }
                else {
                    if (queue.size() == 1) {
                        queue.push_back(queue[queue.size() - 1]);
                    }
                    queue.pop_front();
                }
            }
            i += 1;
        }
    }
    else {
        // CLIENT
        auto& queue = _controls[global().playerIndex];
        bool focus = global().windowMgr.window.hasFocus();
        queue.pop_front();
        queue.push_back(PlayerControls{
            focus && sf::Keyboard::isKeyPressed(sf::Keyboard::A),
            focus && sf::Keyboard::isKeyPressed(sf::Keyboard::D),
            focus && sf::Keyboard::isKeyPressed(sf::Keyboard::W)
        });
    }
}

void ControlsManager::eventUpdate() {
    if (global().playerIndex > 0) {
        auto& queue = _controls[global().playerIndex];
        RN_Compose_SetClientControls(global().netMgr.getNode(), 0, queue[queue.size() - 1]);
    }
}