#ifndef SPEMPE_GAME_CONTEXT_GAME_CONTEXT_HPP
#define SPEMPE_GAME_CONTEXT_GAME_CONTEXT_HPP

#include <Hobgoblin/Common.hpp>
#include <Hobgoblin/QAO.hpp>

#include <SPeMPE/GameContext/Context_components.hpp>
#include <SPeMPE/GameObjectFramework/Synchronized_object_registry.hpp>

#include <atomic>
#include <chrono>
#include <memory>
#include <string>
#include <thread>
#include <vector>

namespace jbatnozic {
namespace spempe {

namespace hg = ::jbatnozic::hobgoblin;

namespace detail {
constexpr int GCMF_NONE = 0x0;
constexpr int GCMF_PRIV = 0x1;
constexpr int GCMF_HDLS = 0x2;
constexpr int GCMF_NETW = 0x4;

constexpr int GCM_INIT = GCMF_NONE;
constexpr int GCM_SERV = GCMF_PRIV | GCMF_HDLS | GCMF_NETW;
constexpr int GCM_CLNT = GCMF_NETW;
constexpr int GCM_SOLO = GCMF_PRIV;
constexpr int GCM_GMAS = GCMF_PRIV | GCMF_NETW;
} // namespace detail

//! TODO: Components included by default: WindowManager, LoggingManager, (NetworkingManager?)
class GameContext {
public:
    ///////////////////////////////////////////////////////////////////////////
    // CONFIGURATION                                                         //
    ///////////////////////////////////////////////////////////////////////////

    enum class Mode {
                                       // PRIVILEGED | HEADLESS | NETWORKING
        Initial    = detail::GCM_INIT, //            |          |           
        Server     = detail::GCM_SERV, //      +     |     +    |      +    
        Client     = detail::GCM_CLNT, //            |          |      +    
        Solo       = detail::GCM_SOLO, //      +     |          |           
        GameMaster = detail::GCM_GMAS, //      +     |          |      +    
    };

    struct RuntimeConfig {
        //! The amount of time one frame should last = 1.0 / framerate.
        std::chrono::duration<double> deltaTime{1.0 / 60};

        //! The maximum number of frames that can be simulated between two
        //! render/draw steps, in case the application isn't achieving the
        //! desired framerate. The recommended value is 2.
        hg::PZInteger maxFramesBetweenDisplays{2};
    };

    GameContext(const RuntimeConfig& aRuntimeConfig,
                hg::PZInteger aComponentTableSize = 32);
    ~GameContext();

    //! Only transitions between Initial and other modes (and the other way
    //! round) are possible.
    void setToMode(Mode aMode);

    bool isPrivileged() const;
    bool isHeadless() const;
    bool hasNetworking() const;

    const RuntimeConfig& getRuntimeConfig() const;

    ///////////////////////////////////////////////////////////////////////////
    // GAME OBJECT MANAGEMENT                                                //
    ///////////////////////////////////////////////////////////////////////////

    hg::QAO_Runtime& getQAORuntime();

    ///////////////////////////////////////////////////////////////////////////
    // COMPONENT MANAGEMENT                                                  //
    ///////////////////////////////////////////////////////////////////////////

    //! Throws hg::TracedLogicError in case of tagHash clash.
    template <class taComponent>
    void attachComponent(taComponent& aComponent);

    //! Throws hg::TracedLogicError in case of tagHash clash.
    template <class taComponent>
    void attachAndOwnComponent(std::unique_ptr<taComponent> aComponent);

    //! No-op if component isn't present.
    //! If the component was owned by the context, it is destroyed.
    void detachComponent(ContextComponent& aComponent);

    //! Throws hg::TracedLogicError if the component isn't present.
    template <class taComponent>
    taComponent& getComponent() const;

    std::string getComponentTableString(char aSeparator = '\n') const;

    ///////////////////////////////////////////////////////////////////////////
    // EXECUTION                                                             //
    ///////////////////////////////////////////////////////////////////////////

    //! Run for a number of steps. A negative value will be taken as infinity.
    //! Returns status code of the run (0 = success).
    int runFor(int aSteps);

    //! Can be called by an instance from 'within' the context (if runFor has
    //! been started) to stop the execution after the current step.
    void stop();

    struct PerformanceInfo {
        std::chrono::microseconds frameToFrameTime{0};
        std::chrono::microseconds updateAndDrawTime{0};
        std::chrono::microseconds finalizeTime{0};
        std::chrono::microseconds totalTime{0};
        hg::PZInteger consecutiveUpdateLoops{0};
    };

    //! Returns the performance measurements of the last executed step.
    const PerformanceInfo& getPerformanceInfo() const;

    //! Returns the ordinal number of the step currently being executed, or of
    //! the last step that was executed if execution was stopped or finished.
    hg::PZInteger getCurrentStepOrdinal() const;

    ///////////////////////////////////////////////////////////////////////////
    // CHILD CONTEXT SUPPORT                                                 //
    ///////////////////////////////////////////////////////////////////////////

    //! Attach a child context.
    //! Throws hg::TracedLogicError if a context is already attached.
    void attachChildContext(std::unique_ptr<GameContext> aChildContext);

    //! Detaches the currently attached child context (return value can be
    //! nullptr if none).
    //! Throws hg::TracedLogicError if the attached context is currently running
    //! (that is, if isChildContextJoinable() returns true).
    std::unique_ptr<GameContext> detachChildContext();

    //! Returns true if a child context is currently attached.
    bool hasChildContext() const;

    //! Returns true if the attached child context is currently running or has
    //! finished execution and is waiting to be joined. Also returns true if
    //! it hasn't been started yet.
    //! Throws hg::TracedLogicError if no child context is attached.
    bool isChildContextJoinable() const;

    //! Returns the currently attached child context (can be nullptr if none).
    GameContext* getChildContext() const;

    //! Starts executing the child context in a background thread.
    //! The argument aSteps is the same as for runFor().
    //! Throws hg::TracedLogicError if no child context is attached or it's
    //! joinable (that is, isChildContextJoinable() returns true).
    void startChildContext(int aSteps);

    //! If a child context is currently attached and is currently running, this
    //! method stops it and joins the background thread. Otherwise, it does 
    //! nothing. The return value is the status of the execution of the last
    //! stopped child context (0 = success), which will be undefined if no child
    //! contexts were ever started.
    int stopAndJoinChildContext();
    
private:
    // Configuration:
    Mode _mode = Mode::Initial;
    RuntimeConfig _runtimeConfig;

    // Game object management:
    hg::QAO_Runtime _qaoRuntime;

    // Context components:
    // TODO: Owned components should be destroyed in a defined order (opposite of insertion)
    std::vector<std::unique_ptr<ContextComponent>> _ownedComponents;
    detail::ComponentTable _components;

    // Execution:
    PerformanceInfo _performanceInfo;
    hg::PZInteger _stepOrdinal = 0;
    std::atomic<bool> _quit;

    // Child context support:
    GameContext* _parentContext = nullptr;
    std::unique_ptr<GameContext> _childContext = nullptr;
    std::thread _childContextThread;
    int _childContextReturnValue = 0;

    static void _runImpl(hg::not_null<GameContext*> aContext,
                         int aMaxSteps,
                         hg::not_null<int*> aReturnValue);
};

template <class taComponent>
void GameContext::attachComponent(taComponent& aComponent) {
    _components.attachComponent(aComponent);
}

template <class taComponent>
void GameContext::attachAndOwnComponent(std::unique_ptr<taComponent> aComponent) {
    if (!aComponent) {
        throw hg::TracedLogicError{"Cannot attach null component"};
    }
    _components.attachComponent(*aComponent);
    _ownedComponents.push_back(std::move(aComponent));
}

template <class taComponent>
taComponent& GameContext::getComponent() const {
    return _components.getComponent<taComponent>();
}

} // namespace spempe
} // namespace jbatnozic

#endif // !SPEMPE_GAME_CONTEXT_GAME_CONTEXT_HPP