#ifndef GLOBAL_PROGRAM_STATE_HPP
#define GLOBAL_PROGRAM_STATE_HPP

#include <Hobgoblin/Common.hpp>

#include <chrono>
#include <fstream>
#include <memory>
#include <thread>

#include "Controls_manager.hpp"
#include "Game_object_framework.hpp"
#include "Isometric_tester.hpp"
#include "Main_game_controller.hpp"
#include "Networking_manager.hpp"
#include "Player.hpp"
#include "Window_manager.hpp"
#include "Lighting.hpp"

// Menu:
// (1) Host a game: Create a server, set self to Client, connect to server
// (2) Connect to a server: set to Client
// (3) Play solo: set to Solo

class GameContext {
public:
    static constexpr auto NETWORKING_PASSPHRASE = "beetlejuice";

    static constexpr int PLAYER_INDEX_UNKNOWN = -1;
    static constexpr int PLAYER_INDEX_NONE = -2;

    static constexpr int F_PRIVILEGED = 0x1;
    static constexpr int F_NETWORKING = 0x2;
    static constexpr int F_HEADLESS   = 0x4;

    enum class Mode : int {
        Initial    = 0,
        Server     = F_PRIVILEGED | F_NETWORKING | F_HEADLESS, 
        Client     = F_NETWORKING,
        Solo       = F_PRIVILEGED,
        GameMaster = F_PRIVILEGED | F_NETWORKING,
    };

private:
    GameContext* _parentContext;
    Mode _mode;

public:
    int playerIndex = PLAYER_INDEX_UNKNOWN;
    hg::PZInteger syncBufferLength = 6;
    hg::PZInteger syncBufferHistoryLength = 1;
    bool quit = false;

    hg::PZInteger networkingServerSize;
    std::uint16_t networkingLocalPort;
    std::uint16_t networkingServerPort;
    std::string networkingServerIp;

    hg::QAO_Runtime qaoRuntime;
    WindowManager windowMgr;
    NetworkingManager netMgr;
    SynchronizedObjectManager syncObjMgr;
    ControlsManager controlsMgr;
    MainGameController mainGameCtrl;
    //LightingManager lightMgr;

    GameContext(Mode mode)
        // Essential:
        : _parentContext{nullptr}
        , _mode{mode}
        , qaoRuntime{this}
        , windowMgr{qaoRuntime.nonOwning()}
        // Game-specific:
        , controlsMgr{qaoRuntime.nonOwning(), 4, syncBufferLength, syncBufferHistoryLength}
        , netMgr{qaoRuntime.nonOwning(), false}
        , syncObjMgr{netMgr.getNode()}
        , mainGameCtrl{qaoRuntime.nonOwning()}
        //, lightMgr{qaoRuntime.nonOwning()}
    {
        netMgr.getNode().setUserData(this);
    }

    ~GameContext() {
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

    void fullStart();

    bool isPrivileged() const {
        return ((static_cast<int>(_mode) & F_PRIVILEGED) != 0);
    }

    bool isHeadless() const {
        return ((static_cast<int>(_mode) & F_HEADLESS) != 0);
    }

    bool hasNetworking() const {
        return ((static_cast<int>(_mode) & F_NETWORKING) != 0);
    }

    int calcDelay(std::chrono::microseconds currentLatency) const {
        return static_cast<int>(currentLatency / std::chrono::microseconds{16'666});
    }

    void changeMode(Mode newMode);

    static void run(GameContext* context, int* retVal);

    void runChildContext(std::unique_ptr<GameContext> childContext) {
        // TODO Error if already running      
        _childContext = std::move(childContext);
        _childContext->_parentContext = this;
        _childContextThread = std::thread{run, _childContext.get(), &_childContextReturnValue};
    }

private:
    std::unique_ptr<GameContext> _childContext;
    std::thread _childContextThread;
    int _childContextReturnValue;
};

#endif // !GLOBAL_PROGRAM_STATE_HPP
