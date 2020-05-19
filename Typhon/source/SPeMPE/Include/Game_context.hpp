#ifndef SPEMPE_GAME_CONTEXT_HPP
#define SPEMPE_GAME_CONTEXT_HPP

#include <Hobgoblin/Common.hpp>
#include <Hobgoblin/Graphics.hpp>
#include <Hobgoblin/QAO.hpp>
#include <Hobgoblin/Utility/Any_ptr.hpp>

#include <SPeMPE/Include/Networking_manager.hpp>
#include <SPeMPE/Include/Synchronized_object_registry.hpp>
#include <SPeMPE/Include/Window_manager.hpp>

#include <chrono>
#include <cstdint>
#include <fstream>
#include <iostream>
#include <memory>
#include <string>
#include <thread>

namespace spempe {

constexpr int PLAYER_INDEX_UNKNOWN = -1;
constexpr int PLAYER_INDEX_NONE    = -2;

class GameContext {
public:
    static constexpr int F_PRIVILEGED = 0x1;
    static constexpr int F_NETWORKING = 0x2;
    static constexpr int F_HEADLESS   = 0x4;

    enum class Mode : int {
        Initial = 0,
        Server     = F_PRIVILEGED | F_NETWORKING | F_HEADLESS,
        Client     = F_NETWORKING,
        Solo       = F_PRIVILEGED,
        GameMaster = F_PRIVILEGED | F_NETWORKING,
    };

    struct ResourceConfig {
        const hg::gr::SpriteLoader* spriteLoader;
    };

    struct RuntimeConfig {

    };

    struct NetworkConfig {
        std::string passphrase;
        hg::PZInteger clientCount;
        sf::IpAddress serverIp;
        std::uint16_t serverPort;
        std::uint16_t localPort;
    };

    GameContext(const ResourceConfig& resourceConfig, const RuntimeConfig& runtimeConfig,
                const NetworkConfig& networkConfig);

    ~GameContext() {
        _qaoRuntime.eraseAllNonOwnedObjects();
    }

    void configure(Mode mode);

    bool isPrivileged() const;
    bool isHeadless() const;
    bool hasNetworking() const;

    const ResourceConfig& getResourceConfig() const {
        return _resourceConfig;
    }

    const RuntimeConfig& getRuntimeConfig() const {
        return _runtimeConfig;
    }

    const NetworkConfig& getNetworkConfig() const {
        return _networkConfig;
    }

    int getLocalPlayerIndex() const {
        return 0; // TODO
    }

    hg::QAO_Runtime& getQaoRuntime() {
        return _qaoRuntime;
    }

    WindowManager& getWindowManager() {
        return _windowManager;
    }

    NetworkingManager& getNetworkingManager() {
        return _networkingManager;
    }

    SynchronizedObjectRegistry& getSyncObjReg() {
        return _syncObjReg;
    }

    int run();
    void stop();

    bool hasChildContext();
    int stopChildContext();
    void runChildContext(std::unique_ptr<GameContext> childContext);

private:
    // Configuration:
    ResourceConfig _resourceConfig;
    RuntimeConfig _runtimeConfig;
    NetworkConfig _networkConfig;

    //std::chrono::duration<double> _deltaTime;
    //hg::PZInteger _targetFps;
    //hg::PZInteger syncBufferLength = 2;

    // Game object management:
    hg::QAO_Runtime _qaoRuntime;
    WindowManager _windowManager;
    NetworkingManager _networkingManager;
    SynchronizedObjectRegistry _syncObjReg;

    // Child context stuff:
    GameContext* _parentContext = nullptr;
    std::unique_ptr<GameContext> _childContext;
    std::thread _childContextThread;
    int _childContextReturnValue = 0;

    // State:
    hg::util::AnyPtr _extensionData;
    int _localPlayerIndex = PLAYER_INDEX_UNKNOWN;
    Mode _mode = Mode::Initial;
    bool _quit = false;

    static void runImpl(GameContext* context, int* retVal);
};

} // namespace spempe

#endif // !SPEMPE_GAME_CONTEXT_HPP
