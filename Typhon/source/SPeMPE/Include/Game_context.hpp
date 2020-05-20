#ifndef SPEMPE_GAME_CONTEXT_HPP
#define SPEMPE_GAME_CONTEXT_HPP

#include <Hobgoblin/Common.hpp>
#include <Hobgoblin/Graphics.hpp>
#include <Hobgoblin/QAO.hpp>

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

class GameContextExtensionData {
public:
    virtual ~GameContextExtensionData() = 0 {};
    virtual const std::type_info& getTypeInfo() const = 0;
};

class GameContext {
public:
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

    struct ResourceConfig {
        const hg::gr::SpriteLoader* spriteLoader;
    };

    struct RuntimeConfig {
        // targetFramerate
        // deltaTime
    };

    // TODO Temp.
    int syncBufferLength = 2;
    int calcDelay(std::chrono::microseconds ms) { return 1; }

    GameContext(const ResourceConfig& resourceConfig, const RuntimeConfig& runtimeConfig);

    ~GameContext() {
        _qaoRuntime.eraseAllNonOwnedObjects();
        _extensionData.reset();
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

    void setLocalPlayerIndex(int index) {
        _localPlayerIndex = index;
    }

    int getLocalPlayerIndex() const {
        return _localPlayerIndex;
    }

    void setExtensionData(std::unique_ptr<GameContextExtensionData> extData) {
        _extensionData = std::move(extData);
    }

    GameContextExtensionData* getExtensionData() const {
        return _extensionData.get();
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
    std::unique_ptr<GameContextExtensionData> _extensionData;
    int _localPlayerIndex = PLAYER_INDEX_UNKNOWN;
    Mode _mode = Mode::Initial;
    bool _quit = false;

    static void runImpl(GameContext* context, int* retVal);
};

} // namespace spempe

#endif // !SPEMPE_GAME_CONTEXT_HPP
