#pragma once

#include "Engine.hpp"

#include <HobRobot/Context/Managers/Main_gameplay_manager_interface.hpp>

namespace hobrobot {

class MainGameplayManager
    : public  MainGameplayManagerInterface
    , public  spe::NonstateObject
    , private spe::NetworkingEventListener {
public:
    explicit MainGameplayManager(QAO_RuntimeRef aRuntimeRef, int aExecutionPriority);

    ~MainGameplayManager() override;

private:
    hg::PZInteger stateBufferingLength = 0;

    void _eventUpdate1() override;
    void _eventPostUpdate() override;
    void _eventDraw1() override;
    void _eventDrawGUI() override;

    void onNetworkingEvent(const RN_Event& aEvent) override;
};

} // namespace hobrobot
