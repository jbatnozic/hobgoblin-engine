#ifndef PLAYER_HPP
#define PLAYER_HPP

#include <Hobgoblin/Common.hpp>

#include "Game_object_framework.hpp"

class Player : public GOF_Base {
public:
    static constexpr float MAX_SPEED = 5.f;
    static constexpr float GRAVITY = 1.f;
    static constexpr float JUMP_POWER = 16.f;

    Player(float x, float y, hg::PZInteger playerIndex);

protected:
    void eventUpdate() override;
    void eventDraw1() override;

private:
    float x, y;
    float xspeed = 0.f, yspeed = 0.f;
    float width = 48.f, height = 64.f;
    bool oldUp = false;
};

#endif // !PLAYER_HPP
