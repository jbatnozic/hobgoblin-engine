
#include <iostream>

#include "Global_program_state.hpp"
#include "Player.hpp"

Player::Player(float x, float y, hg::PZInteger playerIndex)
    : GOF_Base{0, 0, "Player"}
    , x{x}
    , y{y}
{
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