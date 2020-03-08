#ifndef GLOBAL_PROGRAM_STATE_HPP
#define GLOBAL_PROGRAM_STATE_HPP

#include "Object_framework.hpp"
#include "Player.hpp"
#include "Window_manager.hpp"

struct GlobalProgramState : public QAO_UserData {
    bool quit = false;

    QAO_Runtime qaoRuntime;
    WindowManager windowMgr;

    GlobalProgramState() {
        qaoRuntime.addObjectNoOwn(windowMgr);

        QAO_Create<Player>(qaoRuntime, 200.f, 200.f, 0);
    }
};

#endif // !GLOBAL_PROGRAM_STATE_HPP
