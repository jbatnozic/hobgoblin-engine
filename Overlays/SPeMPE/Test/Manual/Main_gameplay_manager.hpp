#pragma once

#include "Config.hpp"
#include "Engine.hpp"

#include <Hobgoblin/Utility/Time_utils.hpp>

#include "Main_gameplay_manager_interface.hpp"

class MainGameplayManagerBase
    : public spe::NonstateObject {
public:
    MainGameplayManagerBase(QAO_RuntimeRef aRuntimeRef)
        : spe::NonstateObject{aRuntimeRef,
                              SPEMPE_TYPEID_SELF, 
                              PRIORITY_GAMEPLAYMGR,
                              "MainGameplayManager"}
    {
    }

private:
    hg::util::Stopwatch _periodicStopwatch;
    hg::util::Stopwatch _totalStopwatch;

    void _eventPreUpdate() override;
};

namespace singleplayer {

class MainGameplayManager
    : public MainGameplayManagerInterface
    , public MainGameplayManagerBase {
public:
    MainGameplayManager(QAO_RuntimeRef aRuntimeRef);

private:

};

} // namespace singleplayer

namespace multiplayer {

class MainGameplayManager
    : public MainGameplayManagerInterface
    , public MainGameplayManagerBase {
public:
    MainGameplayManager(QAO_RuntimeRef aRuntimeRef);

private:

};

} // namespace multiplayer