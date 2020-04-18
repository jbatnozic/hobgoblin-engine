#ifndef CONTROLS_MANAGER_HPP
#define CONTROLS_MANAGER_HPP

#include <Hobgoblin/Common.hpp>
#include <Hobgoblin/Utility/Autopack.hpp>

#include <deque>

#include "Game_object_framework.hpp"
#include "State_scheduler.hpp"

struct PlayerControls {
    bool left = false;
    bool right = false;
    bool up = false;

    HG_ENABLE_AUTOPACK(PlayerControls, left, right, up);

    void integrate(const PlayerControls& other) {
        left = left || other.left;
        right = right || other.right;
        up = up || other.up;
    }

    void unstuck(const std::deque<PlayerControls>& history) {
        PlayerControls dummy{};

        for (const auto& controls : history) {
            dummy.integrate(controls);
        }

        left = left && dummy.left;
        right = right && dummy.right;
        up = up && dummy.up;
    }

    void debounce(const std::deque<PlayerControls>& q, std::size_t currentPos) {
        if (currentPos > 0) {
            auto& prev = q[currentPos - 1];
            auto& next = q[currentPos + 1];
            
            if (prev.left == next.left) left = prev.left;
            if (prev.right == next.right) right = prev.right;
            if (prev.up == next.up) up = prev.up;
        }
    }
};

using ControlsScheduler = StateScheduler<PlayerControls>;

class ControlsManager : public GOF_Base {
public:
    ControlsManager(QAO_RuntimeRef runtimeRef, hg::PZInteger playerCount, 
                    hg::PZInteger inputDelayInSteps, hg::PZInteger historySize);

    void setPlayerCount(hg::PZInteger playerCount);
    void setInputDelay(hg::PZInteger inputDelayInSteps, hg::PZInteger historySize); // TODO Rename
    PlayerControls getCurrentControlsForPlayer(hg::PZInteger playerIndex);

    void putNewControls(hg::PZInteger playerIndex, const PlayerControls& controls, std::chrono::microseconds delay);

protected:
    void eventPreUpdate() override;
    void eventUpdate() override;
    void eventPostUpdate() override;

private:
    std::vector<ControlsScheduler> _schedulers;
};

#endif // !CONTROLS_MANAGER_HPP

