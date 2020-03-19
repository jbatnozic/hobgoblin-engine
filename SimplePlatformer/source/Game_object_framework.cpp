
#include <cassert>

#include "Game_object_framework.hpp"
#include "Global_program_state.hpp"

GlobalProgramState& GOF_Base::global() const {
    assert(getRuntime() && "Game object not registered to any runtime!");
    return *(getRuntime()->getUserData<GlobalProgramState>());
}

hg::QAO_Runtime& GOF_Base::rt() const {
    assert(getRuntime() && "Game object not registered to any runtime!");
    return *getRuntime();
}