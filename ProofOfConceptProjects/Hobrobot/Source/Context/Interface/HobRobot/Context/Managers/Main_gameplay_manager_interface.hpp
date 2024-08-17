#pragma once

#include "Engine.hpp"

namespace hobrobot {

//! TODO(add description)
class MainGameplayManagerInterface : public spe::ContextComponent {
private:
    SPEMPE_CTXCOMP_TAG("hobrobot::MainGameplayManagerInterface");
};

using MMainGameplay = MainGameplayManagerInterface;

} // namespace hobrobot
