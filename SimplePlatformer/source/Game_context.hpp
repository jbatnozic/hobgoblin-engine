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
#include "Terrain_manager.hpp"

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

    struct NetworkConfig {
        hg::PZInteger clientCount;
        sf::IpAddress serverIp;
        std::uint16_t serverPort;
        std::uint16_t localPort;
    };

private:
    GameContext* _parentContext;
    Mode _mode;

public:
    int playerIndex = PLAYER_INDEX_UNKNOWN;
    hg::PZInteger syncBufferLength = 2;
    hg::PZInteger syncBufferHistoryLength = 1;

    NetworkConfig networkConfig;

    QAO_Runtime qaoRuntime;
    WindowManager windowMgr;
    NetworkingManager netMgr;
    MainGameController mainGameCtrl;
    ControlsManager controlsMgr;
    //LightingManager lightMgr;
    SynchronizedObjectManager syncObjMgr; // TODO This object isn't really a "manager"
    TerrainManager terrMgr;

    GameContext()
        // Essential:
        : _parentContext{nullptr}
        , _mode{Mode::Initial}
        , qaoRuntime{this}
        , windowMgr{qaoRuntime.nonOwning()}
        // Game-specific:
        , netMgr{qaoRuntime.nonOwning()}
        , mainGameCtrl{qaoRuntime.nonOwning()}
        , controlsMgr{qaoRuntime.nonOwning(), 4, syncBufferLength, syncBufferHistoryLength}
        //, lightMgr{qaoRuntime.nonOwning()}
        , syncObjMgr{netMgr.getNode()}
        , terrMgr{qaoRuntime.nonOwning(), syncObjMgr, SYNC_ID_CREATE_MASTER}
    {
        netMgr.getNode().setUserData(this);
        // TODO Temp.
        terrMgr.generate(100, 100, 32.f);
    }

    ~GameContext() {
        qaoRuntime.eraseAllNonOwnedObjects();
    }

    void configure(Mode mode);

    bool isPrivileged() const;
    bool isHeadless() const;
    bool hasNetworking() const;

    int calcDelay(std::chrono::microseconds currentLatency) const {
        return static_cast<int>(currentLatency / std::chrono::microseconds{16'666});
    }

    int run();
    void stop();

    bool hasChildContext();
    int stopChildContext();
    void runChildContext(std::unique_ptr<GameContext> childContext);

private:
    // Child context stuff:
    std::unique_ptr<GameContext> _childContext;
    std::thread _childContextThread;
    int _childContextReturnValue;

    // Other:
    bool _quit = false;

    static void runImpl(GameContext* context, int* retVal);
};

#endif // !GLOBAL_PROGRAM_STATE_HPP
