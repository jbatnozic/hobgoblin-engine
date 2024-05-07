// Copyright 2024 Jovan Batnozic. Released under MS-PL licence in Serbia.
// See https://github.com/jbatnozic/Hobgoblin?tab=readme-ov-file#licence

// clang-format off

#ifndef TYPHON_FRAMEWORK_GAME_CONFIG_HPP
#define TYPHON_FRAMEWORK_GAME_CONFIG_HPP

#include <Hobgoblin/Common.hpp>

#define GAME_NAME_STRING "Typhon"
#define GAME_VERS_STRING "0.0.1"

class GameConfig {
public:
    constexpr static hg::PZInteger TARGET_FRAMERATE = 60;
    constexpr static const char* GAME_NAME = GAME_NAME_STRING;
    constexpr static const char* GAME_VERSION = GAME_VERS_STRING;
    constexpr static const char* NETWORKING_PASSPHRASE = ("NetPassphrase_" GAME_NAME_STRING "_" GAME_VERS_STRING);
    constexpr static hg::PZInteger STATE_BUFFERING_LENGTH = 2;
};

#undef GAME_NAME_STRING
#undef GAME_VERS_STRING

#endif // !TYPHON_FRAMEWORK_GAME_CONFIG_HPP

// clang-format on
