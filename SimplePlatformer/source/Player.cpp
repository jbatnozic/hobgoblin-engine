
#include <iostream>

#include "Global_program_state.hpp"
#include "Player.hpp"

RN_DEFINE_HANDLER(CreatePlayer, RN_ARGS(float, x, float, y)) {
    RN_NODE_IN_HANDLER().visit(
        [](NetworkingManager::ClientType& client) {
            auto& global = *client.getUserData<GlobalProgramState>();

        },
        [](NetworkingManager::ServerType& server) {
            // ERROR
        }
    );
}

RN_DEFINE_HANDLER(UpdatePlayer, RN_ARGS()) {

}

RN_DEFINE_HANDLER(DestroyPlayer, RN_ARGS()) {

}

Player::Player(QAO_Runtime* runtime, float x, float y, hg::PZInteger playerIndex)
    : GOF_SynchronizedObject{runtime, TYPEID_SELF, 0, "Player"}
    , x{x}
    , y{y}
{
    // syncCreate
    if (RN_IsServer(global().netMgr.getNode().getType())) {
        RN_Compose_CreatePlayer(global().netMgr.getNode(), RN_COMPOSE_FOR_ALL, x, y);
    }
}

void Player::syncCreate(RN_Node& node, const std::vector<hg::PZInteger>& rec) {
    RN_Compose_CreatePlayer(node, rec, x, y);
}

void Player::syncUpdate(RN_Node& node, const std::vector<hg::PZInteger>& rec) {
    RN_Compose_UpdatePlayer(node, rec); // TODO
}

void Player::syncDestroy(RN_Node& node, const std::vector<hg::PZInteger>& rec) {
    RN_Compose_DestroyPlayer(node, rec); // TODO
}

void Player::eventUpdate() {
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