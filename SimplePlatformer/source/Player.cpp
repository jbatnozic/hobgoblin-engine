
#include <iostream>

#include "Global_program_state.hpp"
#include "Player.hpp"

RN_DEFINE_HANDLER(CreatePlayer, RN_ARGS(SyncId, syncId, Player::State&, state)) {
    RN_NODE_IN_HANDLER().visit(
        [=](NetworkingManager::ClientType& client) {
            auto& global = *client.getUserData<GlobalProgramState>();
            auto& runtime = global.qaoRuntime;
            auto& syncObjMapper = global.syncObjMgr;
            QAO_PCreate<Player>(&runtime, syncObjMapper, syncId, state.x, state.y, state.playerIndex);
        },
        [](NetworkingManager::ServerType& server) {
            // ERROR
        }
    );
}

RN_DEFINE_HANDLER(UpdatePlayer, RN_ARGS(SyncId, syncId, Player::State&, state)) {
    RN_NODE_IN_HANDLER().visit(
        [=](NetworkingManager::ClientType& client) {
            auto& global = *client.getUserData<GlobalProgramState>();
            auto& runtime = global.qaoRuntime;
            auto& syncObjMapper = global.syncObjMgr;
            auto* player = static_cast<Player*>(syncObjMapper.getMapping(syncId));

            const std::chrono::microseconds delay = client.getServer().getRemoteInfo().latency / 2LL;
            player->_ssch.putNewState(state, global.calcDelay(delay));
        },
        [](NetworkingManager::ServerType& server) {
            // ERROR
        }
    );
}

RN_DEFINE_HANDLER(DestroyPlayer, RN_ARGS(SyncId, syncId)) {
    RN_NODE_IN_HANDLER().visit(
        [=](NetworkingManager::ClientType& client) {
            auto& global = *client.getUserData<GlobalProgramState>();
            auto& runtime = global.qaoRuntime;
            auto& syncObjMapper = global.syncObjMgr;
            auto* player = static_cast<Player*>(syncObjMapper.getMapping(syncId));
            QAO_PDestroy(player);
        },
        [](NetworkingManager::ServerType& server) {
            // ERROR
        }
    );
}

Player::Player(QAO_Runtime* runtime, SynchronizedObjectManager& syncObjMapper, 
               float x, float y, hg::PZInteger playerIndex)
    : GOF_SynchronizedObject{runtime, TYPEID_SELF, 75, "Player", syncObjMapper} // TODO Unified constructor for SynchronizedObject
    , _ssch{global().syncBufferLength, global().syncBufferHistoryLength, false, false}
{
    for (auto& state : _ssch) {
        state.playerIndex = playerIndex;
        state.x = x;
        state.y = y;
    }
}

Player::Player(QAO_Runtime* runtime, SynchronizedObjectManager& syncObjMapper, SyncId masterSyncId,
               float x, float y, hg::PZInteger playerIndex)
    : GOF_SynchronizedObject{runtime, TYPEID_SELF, 75, "Player", syncObjMapper, masterSyncId} // TODO Unified constructor for SynchronizedObject
    , _ssch{global().syncBufferLength, global().syncBufferHistoryLength, false, false}
{
    for (auto& state : _ssch) {
        state.playerIndex = playerIndex;
        state.x = x;
        state.y = y;
    }
}

void Player::syncCreateImpl(RN_Node& node, const std::vector<hg::PZInteger>& rec) const {
    RN_Compose_CreatePlayer(node, rec, getSyncId(), _ssch.getCurrentState());
}

void Player::syncUpdateImpl(RN_Node& node, const std::vector<hg::PZInteger>& rec) const {
    RN_Compose_UpdatePlayer(node, rec, getSyncId(), _ssch.getCurrentState()); // TODO
}

void Player::syncDestroyImpl(RN_Node& node, const std::vector<hg::PZInteger>& rec) const {
    RN_Compose_DestroyPlayer(node, rec, getSyncId()); // TODO
}

void Player::eventPreUpdate() {
    if (!global().isHost()) {
        _ssch.advance();
    }
}

void Player::eventUpdate() {
    if (global().isHost()) {
        auto& self = _ssch.getCurrentState();

        PlayerControls controls = global().controlsMgr.getCurrentControlsForPlayer(self.playerIndex);
        /*if (self.playerIndex > 0) {
            printf("[%d, %d, %d]\n", (int)controls.left, (int)controls.right, (int)controls.up);
        }*/

        if (self.y < static_cast<float>(800) - self.height) {
            self.yspeed += GRAVITY;
        }
        else {
            self.y = static_cast<float>(800) - self.height;
            self.yspeed = 0.f;
        }

        if (controls.up && !oldUp) {
            self.yspeed -= JUMP_POWER;
        }

        self.xspeed = (static_cast<float>(controls.right) - static_cast<float>(controls.left)) * MAX_SPEED;

        self.x += self.xspeed;
        self.y += self.yspeed;

        oldUp = controls.up;
    }
    else {
        _ssch.scheduleNewStates();
    }
}

void Player::eventDraw1() {
    auto& self = _ssch.getCurrentState();

    sf::RectangleShape rect{{self.width, self.height}};
    if (self.playerIndex == 0) {
        rect.setFillColor(sf::Color::Blue);
    }
    else {
        rect.setFillColor(sf::Color{204, 0, 204, 255});
    }
    rect.setPosition(self.x, self.y);

    global().windowMgr.getMainRenderTexture().draw(rect);
}