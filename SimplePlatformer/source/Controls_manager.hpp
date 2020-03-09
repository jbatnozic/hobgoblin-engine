#ifndef CONTROLS_MANAGER_HPP
#define CONTROLS_MANAGER_HPP

#include <Hobgoblin/Common.hpp>

#include <deque>
#include <vector>

#include "Object_framework.hpp"

struct PlayerControls {
    bool left = false;
    bool right = false;
    bool up = false;
};

class ControlsManager : public GameObject {
public:
    ControlsManager(hg::PZInteger size = 1, hg::PZInteger inputDelay = 0);

    void resize(hg::PZInteger newSize);
    void resetWithInputDelay(hg::PZInteger inputDelay);
    PlayerControls getCurrentControlsForPlayer(hg::PZInteger playerIndex);
    void overwriteControls(const PlayerControls& controls, hg::PZInteger delayBy);

protected:
    void eventPreUpdate() override;

private:
    std::vector<std::deque<PlayerControls>> _controls;
    hg::PZInteger _defaultInputDelay;
};

#endif // !CONTROLS_MANAGER_HPP

