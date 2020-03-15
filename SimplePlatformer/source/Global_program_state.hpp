#ifndef GLOBAL_PROGRAM_STATE_HPP
#define GLOBAL_PROGRAM_STATE_HPP

#include <Hobgoblin/Common.hpp>

#include "Controls_manager.hpp"
#include "Networking_manager.hpp"
#include "Object_framework.hpp"
#include "Player.hpp"
#include "Window_manager.hpp"

struct GlobalProgramState {
    hg::PZInteger playerIndex = 0;
    bool quit = false;

    QAO_Runtime qaoRuntime;
    WindowManager windowMgr;
    ControlsManager controlsMgr;
    NetworkingManager netMgr;

    Player* player1;
    Player* player2;

    GlobalProgramState(bool isHost)
        : controlsMgr{1, 3}
        , netMgr{isHost}
    {
        qaoRuntime.setUserData(this);
        qaoRuntime.addObjectNoOwn(windowMgr);
        qaoRuntime.addObjectNoOwn(controlsMgr);
        qaoRuntime.addObjectNoOwn(netMgr);

        player1 = QAO_Create<Player>(qaoRuntime, 200.f, 200.f, 0);
        player2 = QAO_Create<Player>(qaoRuntime, 300.f, 200.f, 1);
    }
};

#endif // !GLOBAL_PROGRAM_STATE_HPP
