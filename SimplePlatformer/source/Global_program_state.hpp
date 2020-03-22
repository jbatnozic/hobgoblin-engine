#ifndef GLOBAL_PROGRAM_STATE_HPP
#define GLOBAL_PROGRAM_STATE_HPP

#include <Hobgoblin/Common.hpp>

#include "Controls_manager.hpp"
#include "Game_object_framework.hpp"
#include "Networking_manager.hpp"
#include "Player.hpp"
#include "Window_manager.hpp"
#include "Isometric_tester.hpp"

struct GlobalProgramState {
    int playerIndex;
    bool quit = false;

    hg::QAO_Runtime qaoRuntime;
    WindowManager windowMgr;
    ControlsManager controlsMgr;
    NetworkingManager netMgr;
    SynchronizedObjectManager syncObjMgr;

    GlobalProgramState(bool isHost)
        : windowMgr{nullptr}
        , controlsMgr{nullptr, 4, 4} // runtime, size, inputDelay (in steps)
        , netMgr{nullptr, isHost}
        , syncObjMgr{netMgr.getNode()}
    {
        qaoRuntime.setUserData(this);
        netMgr.getNode().setUserData(this);

        qaoRuntime.addObjectNoOwn(windowMgr);
        qaoRuntime.addObjectNoOwn(controlsMgr);
        qaoRuntime.addObjectNoOwn(netMgr);

        if (isHost) {
            QAO_PCreate<Player>(&qaoRuntime, syncObjMgr, 200.f, 200.f, 0);
            playerIndex = 0;
        }
        else {
            playerIndex = -1;
        }
    }
};

#endif // !GLOBAL_PROGRAM_STATE_HPP
