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
#include <deque>
#include <fstream>
#include <functional>
#include <iostream>
#include <list>
#include <memory>
#include <string>
#include <thread>

namespace spempe {

constexpr int PLAYER_INDEX_UNKNOWN = -2;
constexpr int PLAYER_INDEX_NONE = -1;
constexpr int PLAYER_INDEX_LOCAL_PLAYER = 0;

constexpr class GameContext_AllowOnHost_Type {} ALLOW_ON_HOST;

class GameContextExtensionData {
public:
    virtual ~GameContextExtensionData() = 0;
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
        RuntimeConfig(hg::PZInteger targetFramerate = 60, hg::PZInteger maxFramesBetweenDisplays = 2);

        hg::PZInteger getTargetFramerate() const noexcept;
        std::chrono::duration<double> getDeltaTime() const noexcept;
        hg::PZInteger getMaxFramesBetweenDisplays() const noexcept;

    private:
        hg::PZInteger _targetFramerate;
        std::chrono::duration<double> _deltaTime;
        hg::PZInteger _maxFramesBetweenDisplays;
    };

    struct PerformanceInfo {
        PerformanceInfo() = default;
        PerformanceInfo(const PerformanceInfo&) = default;
        PerformanceInfo& operator=(const PerformanceInfo&) = default;

        std::chrono::microseconds frameToFrameTime{0};
        std::chrono::microseconds updateAndDrawTime{0};
        std::chrono::microseconds finalizeTime{0};
        std::chrono::microseconds totalTime{0};
        hg::PZInteger consecutiveUpdateLoops{0};
    };

    // TODO Temp.
    int syncBufferLength = 2;

    GameContext(const ResourceConfig& resourceConfig, const RuntimeConfig& runtimeConfig);
    ~GameContext();

    // Configuration
    void configure(Mode mode);

    bool isPrivileged() const;
    bool isHeadless() const;
    bool hasNetworking() const;

    const ResourceConfig& getResourceConfig() const;
    const RuntimeConfig& getRuntimeConfig() const;

    void setLocalPlayerIndex(int index);
    int getLocalPlayerIndex() const;

    void setExtensionData(std::unique_ptr<GameContextExtensionData> extData);
    GameContextExtensionData* getExtensionData() const;

    // Get Controllers & Managers:
    hg::QAO_Runtime& getQaoRuntime();
    WindowManager& getWindowManager();
    NetworkingManager& getNetworkingManager();
    SynchronizedObjectRegistry& getSyncObjReg();

    // Execution:
    int run();
    void stop();

    const PerformanceInfo& getPerformanceInfo() const;
    int getCurrentStepOrdinal() const;

    void addPostStepAction(hg::PZInteger delay, std::function<void(GameContext&)> action);
    void addPostStepAction(hg::PZInteger delay, GameContext_AllowOnHost_Type,
                           std::function<void(GameContext&)> action);

    // Child context stuff:
    bool hasChildContext();
    GameContext* getChildContext() const;
    int stopChildContext();
    void runChildContext(std::unique_ptr<GameContext> childContext);

private:
    // Configuration:
    ResourceConfig _resourceConfig;
    RuntimeConfig _runtimeConfig;

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
    std::deque<std::list<std::function<void(GameContext&)>>> _postStepActions;
    PerformanceInfo _performanceInfo;
    std::unique_ptr<GameContextExtensionData> _extensionData;
    int _stepOrdinal = 0;
    int _localPlayerIndex = PLAYER_INDEX_UNKNOWN;
    Mode _mode = Mode::Initial;
    bool _quit = false;

    static void _runImpl(hg::not_null<GameContext*> context, hg::not_null<int*> retVal);

    void _insertPostStepAction(std::function<void(GameContext&)> action, hg::PZInteger delay);
    void _pollPostStepActions();
};

} // namespace spempe

#endif // !SPEMPE_GAME_CONTEXT_HPP
