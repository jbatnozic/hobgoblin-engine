#ifndef GLOBAL_PROGRAM_STATE_HPP
#define GLOBAL_PROGRAM_STATE_HPP

#include <Hobgoblin/Common.hpp>

#include <chrono>
#include <fstream>

#include "Controls_manager.hpp"
#include "Game_object_framework.hpp"
#include "Isometric_tester.hpp"
#include "Main_game_controller.hpp"
#include "Networking_manager.hpp"
#include "Player.hpp"
#include "Window_manager.hpp"
#include "Lighting.hpp"

// TODO Rename to GameContext
struct GlobalProgramState {
    int playerIndex;
    hg::PZInteger syncBufferLength = 2;
    hg::PZInteger syncBufferHistoryLength = 1;
    bool quit = false;

    hg::QAO_Runtime qaoRuntime;
    WindowManager windowMgr;
    NetworkingManager netMgr;
    SynchronizedObjectManager syncObjMgr;
    ControlsManager controlsMgr;
    MainGameController mainGameCtrl;
    LightingManager lightMgr;

    GlobalProgramState(bool isHost)
        : qaoRuntime{this}
        , windowMgr{qaoRuntime.nonOwning()}
        , controlsMgr{qaoRuntime.nonOwning(), 4, syncBufferLength, syncBufferHistoryLength}
        , netMgr{qaoRuntime.nonOwning(), isHost}
        , syncObjMgr{netMgr.getNode()}
        , mainGameCtrl{qaoRuntime.nonOwning()}
        , lightMgr{qaoRuntime.nonOwning()}
    {
        netMgr.getNode().setUserData(this);

        if (isHost) {
            QAO_PCreate<Player>(&qaoRuntime, syncObjMgr, SYNC_ID_CREATE_MASTER, 200.f, 200.f, 0);
            playerIndex = 0;
        }
        else {
            playerIndex = -1;
        }
    }

    ~GlobalProgramState() {
        std::vector<QAO_Base*> objectsToDestroy;
        for (auto object : qaoRuntime) {
            if (qaoRuntime.ownsObject(object)) {
                objectsToDestroy.push_back(object);
            }
        }
        for (auto object : objectsToDestroy) {
            qaoRuntime.eraseObject(object);
        }
    }

    bool isHost() const {
        return (playerIndex == 0);
    }

    int calcDelay(std::chrono::microseconds currentLatency) const {
        return static_cast<int>(currentLatency / std::chrono::microseconds{16'666});
    }

};

#endif // !GLOBAL_PROGRAM_STATE_HPP
