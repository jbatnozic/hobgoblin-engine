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