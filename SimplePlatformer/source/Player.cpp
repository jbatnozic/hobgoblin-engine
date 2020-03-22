
#include <iostream>

#include "Global_program_state.hpp"
#include "Player.hpp"

RN_DEFINE_HANDLER(CreatePlayer, RN_ARGS(SyncId, syncId, float, x, float, y, hg::PZInteger, playerIndex)) {
    RN_NODE_IN_HANDLER().visit(
        [=](NetworkingManager::ClientType& client) {
            auto& global = *client.getUserData<GlobalProgramState>();
            auto& runtime = global.qaoRuntime;
            auto& syncObjMapper = global.syncObjMgr;
            QAO_PCreate<Player>(&runtime, syncObjMapper, syncId, x, y, playerIndex);
        },
        [](NetworkingManager::ServerType& server) {
            // ERROR
        }
    );
}

RN_DEFINE_HANDLER(UpdatePlayer, RN_ARGS(SyncId, syncId, float, x, float, y)) {
    RN_NODE_IN_HANDLER().visit(
        [=](NetworkingManager::ClientType& client) {
            auto& global = *client.getUserData<GlobalProgramState>();
            auto& runtime = global.qaoRuntime;
            auto& syncObjMapper = global.syncObjMgr;
            auto* player = static_cast<Player*>(syncObjMapper.getMapping(syncId));
            player->x = x;
            player->y = y;
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
    , playerIndex{playerIndex}
    , x{x}
    , y{y}
{
}

Player::Player(QAO_Runtime* runtime, SynchronizedObjectManager& syncObjMapper, SyncId masterSyncId,
               float x, float y, hg::PZInteger playerIndex)
    : GOF_SynchronizedObject{runtime, TYPEID_SELF, 0, "Player", syncObjMapper, masterSyncId}
    , playerIndex{playerIndex}
    , x{x}
    , y{y}
{
}

void Player::syncCreateImpl(RN_Node& node, const std::vector<hg::PZInteger>& rec) const {
    RN_Compose_CreatePlayer(node, rec, getSyncId(), x, y, playerIndex);
}

void Player::syncUpdateImpl(RN_Node& node, const std::vector<hg::PZInteger>& rec) const {
    RN_Compose_UpdatePlayer(node, rec, getSyncId(), x, y); // TODO
}

void Player::syncDestroyImpl(RN_Node& node, const std::vector<hg::PZInteger>& rec) const {
    RN_Compose_DestroyPlayer(node, rec, getSyncId()); // TODO
}

void Player::eventUpdate() {
    if (playerIndex != global().playerIndex) {
        return;
    }

    PlayerControls controls = global().controlsMgr.getCurrentControlsForPlayer(global().playerIndex);

    if (y < static_cast<float>(800) - height) {
        yspeed += GRAVITY;
    }
    else {
        y = static_cast<float>(800) - height;
        yspeed = 0.f;
    }

    if (controls.up && !oldUp) {
        yspeed -= JUMP_POWER;
    }

    xspeed = (static_cast<float>(controls.right) - static_cast<float>(controls.left)) * MAX_SPEED;

    x += xspeed;
    y += yspeed;

    oldUp = controls.up;

    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Escape)) {
        global().quit = true;
    }
}

void Player::eventDraw1() {
    sf::RectangleShape rect{{width, height}};
    rect.setFillColor(sf::Color{204, 0, 204, 255});
    rect.setPosition(x, y);

    global().windowMgr.appSurface.draw(rect);
}