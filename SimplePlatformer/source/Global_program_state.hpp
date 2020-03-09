#ifndef GLOBAL_PROGRAM_STATE_HPP
#define GLOBAL_PROGRAM_STATE_HPP

#include <Hobgoblin/Common.hpp>

#include "Controls_manager.hpp"
#include "Object_framework.hpp"
#include "Player.hpp"
#include "Window_manager.hpp"

struct GlobalProgramState : public QAO_UserData {
    hg::PZInteger playerIndex = 0;
    bool quit = false;

    QAO_Runtime qaoRuntime;
    WindowManager windowMgr;
    ControlsManager controlsMgr;

    GlobalProgramState()
        : controlsMgr{1, 3}
    {
        qaoRuntime.setUserData(this);
        qaoRuntime.addObjectNoOwn(windowMgr);
        qaoRuntime.addObjectNoOwn(controlsMgr);

        QAO_Create<Player>(qaoRuntime, 200.f, 200.f, 0);
    }
};

#endif // !GLOBAL_PROGRAM_STATE_HPP
