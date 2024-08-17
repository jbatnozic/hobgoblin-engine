#pragma once

#include "Engine.hpp"

namespace hobrobot {

struct PlayerControls {
    float cursorX = 0.f;
    float cursorY = 0.f;
    bool minExtend = false;
    bool maxExtend = false;
};

constexpr auto CTRLNAME_CURSOR_X = "cur_x";
constexpr auto CTRLNAME_CURSOR_Y = "cur_y";
constexpr auto CTRLNAME_MIN_EXT  = "min_ex";
constexpr auto CTRLNAME_MAX_EXT  = "max_ex";

inline
void SetUpPlayerControlsDefinitions(spe::InputSyncManagerInterface& aInputSyncManager) {
    spe::InputSyncManagerWrapper wrapper{aInputSyncManager};
    wrapper.defineSignal<float>(CTRLNAME_CURSOR_X, 0.f);
    wrapper.defineSignal<float>(CTRLNAME_CURSOR_Y, 0.f);
    wrapper.defineSignal<bool>(CTRLNAME_MIN_EXT, false);
    wrapper.defineSignal<bool>(CTRLNAME_MAX_EXT, false);
}

} // namespace hobrobot
