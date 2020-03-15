
#include <cassert>

#include "Global_program_state.hpp"
#include "Object_framework.hpp"

GlobalProgramState& GameObject::global() const {
    assert(getRuntime() && "Game object not registered to any runtime!");
    return *getRuntime()->getUserData<GlobalProgramState>();
}