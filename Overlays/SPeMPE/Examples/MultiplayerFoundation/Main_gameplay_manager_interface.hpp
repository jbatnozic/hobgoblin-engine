#pragma once

#include "Engine.h"

//! TODO(add description)
class MainGameplayManagerInterface : public spe::ContextComponent {
private:
    SPEMPE_CTXCOMP_TAG("MainGameplayManagerInterface");
};

using MMainGameplay = MainGameplayManagerInterface;
