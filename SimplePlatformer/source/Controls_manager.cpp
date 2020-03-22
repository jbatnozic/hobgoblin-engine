
#include <SFML/System.hpp>

#include "Controls_manager.hpp"
#include "Global_program_state.hpp"

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
                (server.getClient(clientIndex).getRemoteInfo().latency / std::chrono::microseconds{16'666}); // TODO Hardcoded value
            controlsMgr.overwriteControls(server.getSenderIndex() + 1, controls, delay);
        }
    );
}

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

void ControlsManager::overwriteControls(hg::PZInteger playerIndex, const PlayerControls& controls,
                                        hg::PZInteger delayBy) {
    // TODO Temp. implementation
    auto& queue = _controls[playerIndex];

    queue.pop_front();
    queue.push_back(PlayerControls{});

    queue[std::min(delayBy, 3)] = controls;
}

void ControlsManager::eventPreUpdate() {
    if (global().playerIndex == -1) {
        return;
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
                //queue.pop_front();
                //queue.push_back(PlayerControls{});
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