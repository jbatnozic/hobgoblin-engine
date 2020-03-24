#ifndef CONTROLS_MANAGER_HPP
#define CONTROLS_MANAGER_HPP

#include <Hobgoblin/Common.hpp>
#include <Hobgoblin/Utility/Autopack.hpp>

#include <deque>
#include <vector>

#include "Game_object_framework.hpp"

struct PlayerControls {
    bool left = false;
    bool right = false;
    bool up = false;
    HG_ENABLE_AUTOPACK(PlayerControls, left, right, up);
};

class ControlsManager : public GOF_Base {
public:
    ControlsManager(QAO_Runtime* runtime, hg::PZInteger size = 1, hg::PZInteger inputDelay = 0);

    void resize(hg::PZInteger newSize);
    void resetWithInputDelay(hg::PZInteger inputDelay);
    PlayerControls getCurrentControlsForPlayer(hg::PZInteger playerIndex);
    void overwriteControls(hg::PZInteger playerIndex, const PlayerControls& controls, hg::PZInteger delayBy);

protected:
    void eventPreUpdate() override;
    void eventUpdate() override;

private:
    std::vector<std::deque<PlayerControls>> _controls;
    std::vector<int> _offsets;
    hg::PZInteger _defaultInputDelay;
};

#endif // !CONTROLS_MANAGER_HPP

