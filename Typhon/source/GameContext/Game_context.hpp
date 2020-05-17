#ifndef GLOBAL_PROGRAM_STATE_HPP
#define GLOBAL_PROGRAM_STATE_HPP

#include <Hobgoblin/ChipmunkPhysics.hpp>
#include <Hobgoblin/Common.hpp>
#include <Hobgoblin/Utility/stopwatch.hpp>

#include <chrono>
#include <iostream>
#include <fstream>
#include <memory>
#include <thread>

#include "GameObjects/Framework/Game_object_framework.hpp"

#include "GameObjects/Managers/Controls_manager.hpp"
#include "GameObjects/Managers/Environment_manager.hpp"
#include "GameObjects/Managers/Main_game_controller.hpp"
#include "GameObjects/Managers/Networking_manager.hpp"
#include "GameObjects/Managers/Window_manager.hpp"

#include "GameObjects/Gameplay/Collisions.hpp"
#include "GameObjects/Gameplay/Player.hpp"

#include "Graphics/Sprites.hpp"

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

    struct ResourceConfig {
        const hg::gr::SpriteLoader* spriteLoader;
    };

    static constexpr double MICROSECONDS_PER_SECOND = 1'000'000.0;

private:
    GameContext* _parentContext;
    Mode _mode;
    hg::cpSpaceUPtr _physicsSpace;
    std::chrono::duration<double> _deltaTime;
    hg::PZInteger _targetFps;

public:
    int playerIndex = PLAYER_INDEX_UNKNOWN;
    hg::PZInteger syncBufferLength = 2;
    hg::PZInteger syncBufferHistoryLength = 1;

    NetworkConfig networkConfig;

    QAO_Runtime qaoRuntime; // essential
    WindowManager windowMgr;  // essential
    NetworkingManager netMgr;  // essential
    MainGameController mainGameCtrl;
    ControlsManager controlsMgr;
    GOF_SynchronizedObjectRegistry syncObjReg;  // essential
    EnvironmentManager envMgr;

    GameContext(const ResourceConfig& resourceConfig, hg::PZInteger targetFps)
        // Essential:
        : _parentContext{nullptr}
        , _mode{Mode::Initial}
        , _physicsSpace{cpSpaceNew()}
        , _deltaTime{1.0 / static_cast<double>(targetFps)}
        , _targetFps{targetFps}
        , qaoRuntime{this}
        , windowMgr{qaoRuntime.nonOwning()}
        // Game-specific:
        , netMgr{qaoRuntime.nonOwning()}
        , mainGameCtrl{qaoRuntime.nonOwning()}
        , controlsMgr{qaoRuntime.nonOwning(), 4, syncBufferLength, syncBufferHistoryLength}
        , syncObjReg{netMgr.getNode()}
        , envMgr{qaoRuntime.nonOwning(), syncObjReg, GOF_SYNC_ID_CREATE_MASTER}
        // Other:
        , _resourceConfig{resourceConfig}
    {
        netMgr.getNode().setUserData(this);
        cpSpaceSetUserData(_physicsSpace.get(), this);
        cpSpaceSetDamping(_physicsSpace.get(), 0.1);
        Collideables::installCollisionHandlers(_physicsSpace.get());
    }

    ~GameContext() {
        qaoRuntime.eraseAllNonOwnedObjects();
    }

    void configure(Mode mode);

    bool isPrivileged() const;
    bool isHeadless() const;
    bool hasNetworking() const;

    const ResourceConfig& getResourceConfig() const {
        return _resourceConfig;
    }

    cpSpace* getPhysicsSpace() const {
        return _physicsSpace.get();
    }

    decltype(_deltaTime) getDeltaTime() const {
        return _deltaTime;
    }

    int calcDelay(std::chrono::microseconds currentLatency) const { // TODO Move to Utils class
        return static_cast<int>(currentLatency / _deltaTime);
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
    ResourceConfig _resourceConfig;
    bool _quit = false;

    static void runImpl(GameContext* context, int* retVal);
};

#endif // !GLOBAL_PROGRAM_STATE_HPP
