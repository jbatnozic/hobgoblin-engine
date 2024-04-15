// Copyright 2024 Jovan Batnozic. Released under MS-PL licence in Serbia.
// See https://github.com/jbatnozic/Hobgoblin?tab=readme-ov-file#licence

// clang-format off

#pragma once

#include "Engine.hpp"

namespace singleplayer {

class MainGameplayManagerInterface : public spe::ContextComponent {
private:
    SPEMPE_CTXCOMP_TAG("SPMainGameplayManagerInterface");
};

} // namespace singleplayer

namespace multiplayer {

class MainGameplayManagerInterface : public spe::ContextComponent {
private:
    SPEMPE_CTXCOMP_TAG("MPMainGameplayManagerInterface");
};

} // namespace multiplayer

// clang-format on
