#ifndef GAME_CONFIG_HPP
#define GAME_CONFIG_HPP

#include <Hobgoblin/Common.hpp>

class GameConfig {
public:
    static constexpr hg::PZInteger TARGET_FRAMERATE = 60;

    static constexpr int PLAYER_INDEX_UNKNOWN = -1;
    static constexpr int PLAYER_INDEX_NONE = -2;
};

#endif // !GAME_CONFIG_HPP

