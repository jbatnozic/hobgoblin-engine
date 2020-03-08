
#include <iostream>

#include "Global_program_state.hpp"
#include "Player.hpp"

Player::Player(float x, float y, hg::PZInteger playerIndex)
    : GameObject{0, 0, "Player"}
    , x{x}
    , y{y}
{
}

struct PlayerInput {
    bool up = false;
    bool down = false;
    bool left = false;
    bool right = false;
};

void Player::eventUpdate() {
    PlayerInput input = {
            sf::Keyboard::isKeyPressed(sf::Keyboard::Up) || sf::Mouse::isButtonPressed(sf::Mouse::Left),
            sf::Keyboard::isKeyPressed(sf::Keyboard::Down),
            sf::Keyboard::isKeyPressed(sf::Keyboard::Left),
            sf::Keyboard::isKeyPressed(sf::Keyboard::Right)
        };

    if (y < static_cast<float>(800) - height) {
        yspeed += GRAVITY;
    }
    else {
        y = static_cast<float>(800) - height;
        yspeed = 0.f;
    }

    if (input.up && !oldUp) {
        yspeed -= JUMP_POWER;
    }

    xspeed = (static_cast<float>(input.right) - static_cast<float>(input.left)) * MAX_SPEED;

    x += xspeed;
    y += yspeed;

    oldUp = input.up;

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