#ifndef GLOBAL_PROGRAM_STATE_HPP
#define GLOBAL_PROGRAM_STATE_HPP

#include <Hobgoblin/Common.hpp>

#include <fstream>

#include "Controls_manager.hpp"
#include "Game_object_framework.hpp"
#include "Isometric_tester.hpp"
#include "Main_game_controller.hpp"
#include "Networking_manager.hpp"
#include "Player.hpp"
#include "Window_manager.hpp"

struct GlobalProgramState {
    int playerIndex;
    bool quit = false;

    hg::QAO_Runtime qaoRuntime;
    WindowManager windowMgr;
    NetworkingManager netMgr;
    SynchronizedObjectManager syncObjMgr;
    ControlsManager controlsMgr;
    MainGameController mainGameCtrl;
    std::ofstream file;

    GlobalProgramState(bool isHost)
        : qaoRuntime{this}
        , windowMgr{qaoRuntime.nonOwning()}
        , controlsMgr{qaoRuntime.nonOwning(), 4, 4, 1} // runtime, playerCount, inputDelay (in steps), historySize
        , netMgr{qaoRuntime.nonOwning(), isHost}
        , syncObjMgr{netMgr.getNode()}
        , mainGameCtrl{qaoRuntime.nonOwning()}
        , file{"logs.txt", std::ostream::out}
    {
        netMgr.getNode().setUserData(this);

        if (isHost) {
            QAO_PCreate<Player>(&qaoRuntime, syncObjMgr, 200.f, 200.f, 0);
            playerIndex = 0;
        }
        else {
            playerIndex = -1;
        }
    }

    bool isHost() const {
        return (playerIndex == 0);
    }

};

#endif // !GLOBAL_PROGRAM_STATE_HPP
