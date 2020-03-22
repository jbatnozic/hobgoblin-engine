
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
            player->s = state;
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
            runtime.eraseObject(player);
        },
        [](NetworkingManager::ServerType& server) {
            // ERROR
        }
    );
}

Player::Player(QAO_Runtime* runtime, SynchronizedObjectManager& syncObjMapper, 
               float x, float y, hg::PZInteger playerIndex)
    : GOF_SynchronizedObject{runtime, TYPEID_SELF, 0, "Player", syncObjMapper}
{
    s.playerIndex = playerIndex;
    s.x = x;
    s.y = y;
}

Player::Player(QAO_Runtime* runtime, SynchronizedObjectManager& syncObjMapper, SyncId masterSyncId,
               float x, float y, hg::PZInteger playerIndex)
    : GOF_SynchronizedObject{runtime, TYPEID_SELF, 0, "Player", syncObjMapper, masterSyncId}
{
    s.playerIndex = playerIndex;
    s.x = x;
    s.y = y;
}

void Player::syncCreateImpl(RN_Node& node, const std::vector<hg::PZInteger>& rec) const {
    RN_Compose_CreatePlayer(node, rec, getSyncId(), s);
}

void Player::syncUpdateImpl(RN_Node& node, const std::vector<hg::PZInteger>& rec) const {
    RN_Compose_UpdatePlayer(node, rec, getSyncId(), s); // TODO
}

void Player::syncDestroyImpl(RN_Node& node, const std::vector<hg::PZInteger>& rec) const {
    RN_Compose_DestroyPlayer(node, rec, getSyncId()); // TODO
}

void Player::eventUpdate() {
    PlayerControls controls{};

    if (global().playerIndex == 0) {
        controls = global().controlsMgr.getCurrentControlsForPlayer(s.playerIndex);
    }

    if (s.y < static_cast<float>(800) - s.height) {
        s.yspeed += GRAVITY;
    }
    else {
        s.y = static_cast<float>(800) - s.height;
        s.yspeed = 0.f;
    }

    if (controls.up && !oldUp) {
        s.yspeed -= JUMP_POWER;
    }

    s.xspeed = (static_cast<float>(controls.right) - static_cast<float>(controls.left)) * MAX_SPEED;

    s.x += s.xspeed;
    s.y += s.yspeed;

    oldUp = controls.up;
}

void Player::eventDraw1() {
    sf::RectangleShape rect{{s.width, s.height}};
    rect.setFillColor(sf::Color{204, 0, 204, 255});
    rect.setPosition(s.x, s.y);

    global().windowMgr.appSurface.draw(rect);
}