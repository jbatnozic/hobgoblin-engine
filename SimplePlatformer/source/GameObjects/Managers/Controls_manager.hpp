#ifndef CONTROLS_MANAGER_HPP
#define CONTROLS_MANAGER_HPP

#include <Hobgoblin/Common.hpp>
#include <Hobgoblin/Utility/Autopack.hpp>
#include <Hobgoblin/Utility/State_scheduler.hpp>

#include <deque>

#include "GameObjects/Framework/Game_object_framework.hpp"

struct PlayerControls {
    float mouseX = 0.0;
    float mouseY = 0.0;
    bool fire = false;

    bool left = false;
    bool right = false;
    bool up = false;
    bool down = false;

    HG_ENABLE_AUTOPACK(PlayerControls, mouseX, mouseY, fire, left, right, up, down);
};

using ControlsScheduler = hg::util::StateScheduler<PlayerControls>;

class ControlsManager : public GOF_Base {
public:
    ControlsManager(QAO_RuntimeRef runtimeRef, hg::PZInteger playerCount, 
                    hg::PZInteger inputDelayInSteps, hg::PZInteger historySize);

    void setPlayerCount(hg::PZInteger playerCount);
    void setInputDelay(hg::PZInteger inputDelayInSteps, hg::PZInteger historySize); // TODO Rename
    PlayerControls getCurrentControlsForPlayer(hg::PZInteger playerIndex);

    void putNewControls(hg::PZInteger playerIndex, const PlayerControls& controls, std::chrono::microseconds delay);

protected:
    void eventPreUpdate() override; // TODO To frameStart
    void eventUpdate() override;
    void eventPostUpdate() override;

private:
    std::vector<ControlsScheduler> _schedulers;
};

#endif // !CONTROLS_MANAGER_HPP

