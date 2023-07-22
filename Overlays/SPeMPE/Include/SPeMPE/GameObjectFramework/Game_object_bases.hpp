#ifndef SPEMPE_GAME_OBJECT_FRAMEWORK_GAME_OBJECT_BASES_HPP
#define SPEMPE_GAME_OBJECT_FRAMEWORK_GAME_OBJECT_BASES_HPP

#include <Hobgoblin/QAO.hpp>
#include <Hobgoblin/RigelNet.hpp>
#include <Hobgoblin/RigelNet_macros.hpp>
#include <Hobgoblin/Utility/Dynamic_bitset.hpp>
//#include <Hobgoblin/Utility/State_scheduler_simple.hpp>
#include <Hobgoblin/Utility/State_scheduler_verbose.hpp>
#include <SPeMPE/GameContext/Game_context.hpp>
#include <SPeMPE/GameObjectFramework/Autodiff_state.hpp>
#include <SPeMPE/GameObjectFramework/Synchronized_object_registry.hpp>

#include <algorithm>
#include <cassert>
#include <cmath>
#include <cstdint>
#include <optional>
#include <typeinfo>
#include <type_traits>
#include <utility>
#include <vector>

#include <Hobgoblin/Logging.hpp>
#include <iostream>

#define SPEMPE_TYPEID_SELF (typeid(decltype(*this)))

namespace jbatnozic {
namespace spempe {

namespace hg = ::jbatnozic::hobgoblin;

//! Not to be inherited from directly. Use one of the classes below.
class GameObjectBase : public hg::QAO_Base {
public:
    using QAO_Base::QAO_Base;

    //! Shorthand to get the GameContext in which this object lives.
    GameContext& ctx() const {
        // TODO Temporary implementation
        return *(getRuntime()->getUserData<GameContext>());
    }

    //! Shorthand to get one of the context components of the context 
    //! in which this object lives.
    template <class taComponent>
    taComponent& ccomp() const {
        return ctx().getComponent<taComponent>();
    }
};

///////////////////////////////////////////////////////////////////////////
// NONSTATE OBJECTS                                                      //
///////////////////////////////////////////////////////////////////////////

//! I:
//! Objects which are not essential to the game's state and thus not saved (when
//! writing game state) nor synchronized with clients (in multiplayer sessions).
//! For example, particle effects and such.
//! II:
//! Controllers which are always created when the game starts and thus always
//! implicitly present, so we don't need to save them.
class NonstateObject : public GameObjectBase {
public:
    using GameObjectBase::GameObjectBase;
};

///////////////////////////////////////////////////////////////////////////
// STATE OBJECTS                                                         //
///////////////////////////////////////////////////////////////////////////

//! Objects which are essential for the game's state, but will not be synchronized
//! with clients. For use with singleplayer games, or for server-side controller
//! objects in multiplayer games.
class StateObject : public GameObjectBase {
public:
    using GameObjectBase::GameObjectBase;
};

///////////////////////////////////////////////////////////////////////////
// SYNCHRONIZED OBJECTS                                                  //
///////////////////////////////////////////////////////////////////////////

enum class SyncUpdateStatus {
    Normal,    //! TODO (desc)
    Skip,      //! TODO (desc)
    Deactivate //! TODO (desc)
};

struct IfMaster {};
struct IfDummy  {};

class SynchronizedObjectBase : public StateObject {
public:
    //! Big scary constructor with way too many arguments.
    SynchronizedObjectBase(hg::QAO_RuntimeRef aRuntimeRef,
                           const std::type_info& aTypeInfo,
                           int aExecutionPriority,
                           std::string aName,
                           RegistryId aRegId,
                           SyncId aSyncId);

    ~SynchronizedObjectBase() override;

    SyncId getSyncId() const noexcept;
    bool isMasterObject() const noexcept;
    bool isUsingAlternatingUpdates() const noexcept;

    //! Internal implementation, do not call manually!
    void __spempeimpl_destroySelfIn(int aStepCount);

    virtual bool isDeactivated() const = 0;

protected:
    // Call the following to sync this object's creation/update/destruction right away.

    void doSyncCreate() const;
    void doSyncUpdate() const;
    void doSyncDestroy() const;

    // These overloads will be called if the object is a Master object 
    // (that is, executing in a Privileged context).

    virtual void _eventStartFrame(IfMaster)    {}
    virtual void _eventPreUpdate(IfMaster)     {}
    virtual void _eventUpdate(IfMaster)        {}
    virtual void _eventPostUpdate(IfMaster)    {}
    virtual void _eventDraw1(IfMaster)         {}
    virtual void _eventDraw2(IfMaster)         {}
    virtual void _eventDrawGUI(IfMaster)       {}
    virtual void _eventFinalizeFrame(IfMaster) {}

    // These overloads will be called if the object is a Dummy object 
    // (that is, executing in a non-Privileged context).

    virtual void _eventStartFrame(IfDummy)    {}
    virtual void _eventPreUpdate(IfDummy)     {}
    virtual void _eventUpdate(IfDummy)         ; // This one is special!
    virtual void _eventPostUpdate(IfDummy)    {}
    virtual void _eventDraw1(IfDummy)         {}
    virtual void _eventDraw2(IfDummy)         {}
    virtual void _eventDrawGUI(IfDummy)       {}
    virtual void _eventFinalizeFrame(IfDummy) {}

    // If you override any of the below, the overloads above will not be used.
    // The same code will be executed on both ends.

    void _eventStartFrame() override;
    void _eventPreUpdate() override;
    void _eventUpdate() override;
    void _eventPostUpdate() override;
    void _eventDraw1() override;
    void _eventDraw2() override;
    void _eventDrawGUI() override;
    void _eventFinalizeFrame() override;

    // Misc.
    bool _willDieAfterUpdate() const;

    void _enableAlternatingUpdates();

    //! This method can be called only during _eventFinalizeStep().
    //! It will return true if objects with alternating updates enabled have
    //! synced during this cycle.
    //! Usually this is only useful for objects that use both autodiff states
    //! and alternating updates, as they will want to commit their autodiff
    //! states only after a sync.
    bool _didAlternatingUpdatesSync() const;

private:
    friend class detail::SynchronizedObjectRegistry;

    detail::SynchronizedObjectRegistry& _syncObjReg;
    const SyncId _syncId;

    hg::util::DynamicBitset _remoteStatuses;

    int _deathCounter = -1;

    bool _alternatingUpdatesEnabled = false;

    //! Called when it's needed to sync this object's creation to one or more recepeints.
    virtual void _syncCreateImpl(SyncDetails& aSyncDetails) const = 0;

    //! Called when it's needed to sync this object's update to one or more recepeints.
    virtual void _syncUpdateImpl(SyncDetails& aSyncDetails) const = 0;

    //! Called when it's needed to sync this object's destruction to one or more recepeints.
    virtual void _syncDestroyImpl(SyncDetails& aSyncDetails) const = 0;

    virtual void _advanceDummyAndScheduleNewStates() = 0;

    virtual void _setStateSchedulerDefaultDelay(hg::PZInteger aNewDefaultDelaySteps) = 0;

    virtual void _deactivateSelfIn(hg::PZInteger aDelaySteps) = 0;
};

/*
Note:
    If an object drived from SynchronizedObject (below), and transitively from
    SynchronizedObjectBase overrides either _eventUpdate() or eventUpdate(IfDummy),
    note that the dummy object won't behave properly unless you call the following
    code at the start of its eventUpdate:
    if (!isMasterObject()) {
        const bool endOfLifetime = _willDieAfterUpdate();
        SynchronizedObjectBase::eventUpdate(IfDummy);
        if (endOfLifetime) return;
    }

    This can be expressed with the macro SPEMPE_SYNCOBJ_BEGIN_EVENT_UPDATE_OVERRIDE().
*/

#define SPEMPE_SYNCOBJ_BEGIN_EVENT_UPDATE_OVERRIDE() \
    do { if (!SynchronizedObjectBase::isMasterObject()) { \
        const bool endOfLifetime_ = SynchronizedObjectBase::_willDieAfterUpdate(); \
        SynchronizedObjectBase::_eventUpdate(::jbatnozic::spempe::IfDummy{}); \
        if (endOfLifetime_) { return; } \
    } } while (false)

//! Objects which are essential to the game's state, so they are both saved when
//! writing game state, and synchronized with clients in multiplayer sessions.
//! For example, units, terrain, interactible items (and, basically, most other 
//! game objects).
template <class taVisibleState>
class SynchronizedObject : public SynchronizedObjectBase {
public:
    using VisibleState = taVisibleState;

    bool isDeactivated() const final override {
        return _ssch.getCurrentState().status.isDeactivated;
    }

protected:
    using SyncObjSuper = SynchronizedObject;

    SynchronizedObject(hg::QAO_RuntimeRef aRuntimeRef,
                       const std::type_info& aTypeInfo,
                       int aExecutionPriority,
                       std::string aName,
                       RegistryId aRegId,
                       SyncId aSyncId = SYNC_ID_NEW)
        : SynchronizedObjectBase{ aRuntimeRef
                                , aTypeInfo
                                , aExecutionPriority
                                , std::move(aName)
                                , aRegId
                                , aSyncId
                                }
        , _ssch{
            isMasterObject() ? 0 // Masters don't need state scheduling
                             : reinterpret_cast<detail::SynchronizedObjectRegistry*>(
                                   aRegId.address
                               )->getDefaultDelay()
        }
    {
    }

    taVisibleState& _getCurrentState() {
        assert(isMasterObject() || !isDeactivated());
        return _ssch.getCurrentState().visibleState;
    }

    taVisibleState& _getFollowingState() {
        assert(isMasterObject() || !isDeactivated());
        auto& followingState = _ssch.getFollowingState();
        if (followingState.status.isDeactivated) {
            return _ssch.getCurrentState().visibleState;
        }
        return followingState.visibleState;
    }

    const taVisibleState& _getCurrentState() const {
        assert(isMasterObject() || !isDeactivated());
        return _ssch.getCurrentState().visibleState;
    }

    const taVisibleState& _getFollowingState() const {
        return const_cast<SynchronizedObject*>(this)->_getFollowingState();
    }

//private:
protected:
    struct DummyStatus {
        bool isDeactivated = true;

        inline constexpr static DummyStatus active() {
            return DummyStatus{false};
        }

        inline constexpr static DummyStatus deactivated() {
            return DummyStatus{true};
        }
    };

    struct SchedulerPair {
        taVisibleState visibleState;
        DummyStatus status;

        friend
        std::ostream& operator<<(std::ostream& aOstream, const SchedulerPair& aSPair) {
            return (aOstream << aSPair.visibleState << (aSPair.status.isDeactivated ? " [deactivated]" : " [active]"));
        }
    };

    struct DeferredState {
        taVisibleState state;
        hg::PZInteger delay;
    };

    struct PacemakerPulse {
        bool happened = false;
        hg::PZInteger delay;
    };

    hg::util::SimpleStateScheduler<SchedulerPair> _ssch;

    std::optional<DeferredState> _deferredState; // Used with alternating updates

    PacemakerPulse _pacemakerPulse;

    void _advanceDummyAndScheduleNewStates() override final {
        _ssch.advance();

        if (_pacemakerPulse.happened) {
            _ssch.alignToDelay(_pacemakerPulse.delay);
            _pacemakerPulse.happened = false;
        }
        else if (isUsingAlternatingUpdates() && !_ssch.isCurrentStateFresh()) {
            _ssch.alignToDelay(_pacemakerPulse.delay);
        }

        if (_deferredState.has_value()) {
            _ssch.putNewState(SchedulerPair{_deferredState->state, DummyStatus::active()}, _deferredState->delay);
            _deferredState.reset();
        }

        _ssch.scheduleNewStates();
    }

    void _setStateSchedulerDefaultDelay(hg::PZInteger aNewDefaultDelaySteps) override final {
        if (!isMasterObject()) {
            if (isUsingAlternatingUpdates()) {
                aNewDefaultDelaySteps++;
            }
            _ssch.setDefaultDelay(aNewDefaultDelaySteps);
        }
    }

    void _deactivateSelfIn(hg::PZInteger aDelaySteps) override final {
        _ssch.putNewState(SchedulerPair{{}, DummyStatus::deactivated()}, aDelaySteps);
    }

    const taVisibleState& _getLatestState() const {
        return _ssch.getLatestState().visibleState;
    }

public:
    //! Internal implementation, do not call manually!
    void __spempeimpl_applyUpdate(const VisibleState& aNewState, hg::PZInteger aDelaySteps, SyncFlags aFlags) {
        VisibleState stateToSchedule = [this, &aNewState, aFlags]() {
            if constexpr (std::is_base_of<detail::AutodiffStateTag, VisibleState>::value) {
                if (!HasFullState(aFlags)) {
                    VisibleState state = _getLatestState();
                    state.applyDiff(aNewState);
                    return state;
                }
            }
            return aNewState;
        }();

        // std::cout << "@@@ " << _getLatestState() << " + " << aNewState << " = " << stateToSchedule << '\n';

        switch (HasPacemakerPulse(aFlags)) {
        // NORMAL UPDATE
        case false:
            if (!isUsingAlternatingUpdates()) {
                _ssch.putNewState(SchedulerPair{stateToSchedule, DummyStatus::active()}, aDelaySteps);
            }
            else {
                if (_deferredState.has_value()) {
                    _ssch.putNewState(SchedulerPair{(*_deferredState).state,
                                      DummyStatus::active()},
                                      (*_deferredState).delay);
                    _deferredState.reset();
                }

                _ssch.putNewState(SchedulerPair{stateToSchedule, DummyStatus::active()}, aDelaySteps);

                _deferredState = {stateToSchedule, aDelaySteps};
            }
            //if (_pacemakerPulse.happened) {
                // This is in the correct place because the pacemaker 
                // pulse actually affects the *following* update.
                _pacemakerPulse.delay = aDelaySteps;
            //}
            break;

        // PACEMAKER UPDATE
        case true:
            if (!isUsingAlternatingUpdates()) {
                _ssch.putNewState(SchedulerPair{stateToSchedule, DummyStatus::active()}, aDelaySteps);
            }
            else {
                // When using alternating updates, a pacemaker pulse happens inbetween actual updates
            }
            _pacemakerPulse.happened = true;
            _pacemakerPulse.delay = aDelaySteps;
            break;

        default:
            break;
        }
    }
};

///////////////////////////////////////////////////////////////////////////
// SYNCHRONIZATION HELPERS                                               //
///////////////////////////////////////////////////////////////////////////

namespace detail {

//! This class can be instantiated within the body of a RigelNet RPC (=within a handler)
//! to get easy access to the GameContext of the receiver and other relevant important info.
template <class taGameContext, class taNetwMgr>
struct RPCReceiverContext {
    //! Reference to game context (spempe::GameContext).
    taGameContext& gameContext;

    //! Reference to instance of spempe::NetworkingManagerInterface.
    taNetwMgr& netwMgr;

    //! Index of the sender (always -1000 on client).
    int senderIndex;

    //! Latency to the sender (single direction, not round-trip).
    //! In case of a lag spike after which many packets arrive at once,
    //! this value should give you roughly something between the
    //! 'optimistic' and 'pessimistic' latencies (see below).
    //! During normal operation with a stable connection, all 3 latencies
    //! should have similar values.
    std::chrono::microseconds meanLatency;

    //! Latency to the sender (single direction, not round-trip).
    //! In case of a lag spike after which many packets arrive at once,
    //! the 'optimistic' latency will take into account only the most
    //! recent packet, so it shouldn't spike much (if at all).
    //! During normal operation with a stable connection, all 3 latencies
    //! should have similar values.
    std::chrono::microseconds optimisticLatency;

    //! Latency to the sender (single direction, not round-trip).
    //! In case of a lag spike after which many packets arrive at once,
    //! the 'pessimistic' latency will also briefly spike.
    //! During normal operation with a stable connection, all 3 latencies
    //! should have similar values.
    std::chrono::microseconds pessimisticLatency;

    //! Mean latency to the sender (single direction, not round-trip)
    //! expressed in steps (approximated using the desired framerate
    //! in the game context's runtime configuration).
    hg::PZInteger meanLatencyInSteps;

    //! Optimistic latency to the sender (single direction, not round-trip)
    //! expressed in steps (approximated using the desired framerate
    //! in the game context's runtime configuration).
    hg::PZInteger optimisticLatencyInSteps;

    //! Pessimistic latency to the sender (single direction, not round-trip)
    //! expressed in steps (approximated using the desired framerate
    //! in the game context's runtime configuration).
    hg::PZInteger pessimisticLatencyInSteps;

#define ROUNDTOI(_x_) (static_cast<int>(std::round(_x_)))
    explicit RPCReceiverContext(const hg::RN_ClientInterface& aClient)
        : gameContext{*aClient.getUserDataOrThrow<taGameContext>()}
        , netwMgr{gameContext.template getComponent<taNetwMgr>()}
        , senderIndex{-1000}
        , meanLatency{aClient.getServerConnector().getRemoteInfo().meanLatency / 2}
        , optimisticLatency{aClient.getServerConnector().getRemoteInfo().optimisticLatency / 2}
        , pessimisticLatency{aClient.getServerConnector().getRemoteInfo().pessimisticLatency / 2}
        , meanLatencyInSteps{ROUNDTOI(meanLatency / gameContext.getRuntimeConfig().deltaTime)}
        , optimisticLatencyInSteps{ROUNDTOI(optimisticLatency / gameContext.getRuntimeConfig().deltaTime)}
        , pessimisticLatencyInSteps{ROUNDTOI(pessimisticLatency / gameContext.getRuntimeConfig().deltaTime)}
    {
    }

    explicit RPCReceiverContext(const hg::RN_ServerInterface& aServer)
        : gameContext{*aServer.getUserDataOrThrow<taGameContext>()}
        , netwMgr{gameContext.template getComponent<taNetwMgr>()}
        , senderIndex{aServer.getSenderIndex()}
        , meanLatency{aServer.getClientConnector(senderIndex).getRemoteInfo().meanLatency / 2}
        , optimisticLatency{aServer.getClientConnector(senderIndex).getRemoteInfo().optimisticLatency / 2}
        , pessimisticLatency{aServer.getClientConnector(senderIndex).getRemoteInfo().pessimisticLatency / 2}
        , meanLatencyInSteps{ROUNDTOI(meanLatency / gameContext.getRuntimeConfig().deltaTime)}
        , optimisticLatencyInSteps{ROUNDTOI(optimisticLatency / gameContext.getRuntimeConfig().deltaTime)}
        , pessimisticLatencyInSteps{ROUNDTOI(pessimisticLatency / gameContext.getRuntimeConfig().deltaTime)}
    {
    }
#undef ROUNDTOI
};

template <class taGameContext, class taNetwMgr>
RPCReceiverContext<taGameContext, taNetwMgr> GetRPCReceiverContext(hg::RN_ClientInterface& aClient) {
    return RPCReceiverContext<taGameContext, taNetwMgr>{aClient};
}

template <class taGameContext, class taNetwMgr>
RPCReceiverContext<taGameContext, taNetwMgr> GetRPCReceiverContext(hg::RN_ServerInterface& aServer) {
    return RPCReceiverContext<taGameContext, taNetwMgr>{aServer};
}

} // namespace detail

template <class taSyncObj, class taGameContext, class taNetwMgr>
void DefaultSyncCreateHandler(hg::RN_NodeInterface& node, 
                              SyncId syncId) {
    node.callIfClient(
        [&](hg::RN_ClientInterface& client) {
            auto& ctx     = *client.getUserDataOrThrow<taGameContext>();
            auto& runtime = ctx.getQAORuntime();
            auto  regId   = ctx.getComponent<NetworkingManagerInterface>().getRegistryId();
            auto& syncObjReg = *reinterpret_cast<detail::SynchronizedObjectRegistry*>(regId.address);

            if (syncObjReg.getMapping(syncId) == nullptr) {
                hg::QAO_PCreate<taSyncObj>(&runtime, regId, syncId);
            }
        });

    node.callIfServer(
        [](hg::RN_ServerInterface&) {
            throw hg::RN_IllegalMessage("Server received a sync message");
        });
}

template <class taSyncObj, class taGameContext, class taNetwMgr>
void DefaultSyncUpdateHandler(hg::RN_NodeInterface& node,
                              SyncId syncId,
                              SyncFlags flags,
                              typename taSyncObj::VisibleState& state) {
    node.callIfClient(
        [&](hg::RN_ClientInterface& client) {
            detail::RPCReceiverContext<taGameContext, taNetwMgr> rc{client};
            auto  regId      = rc.netwMgr.getRegistryId();
            auto& syncObjReg = *reinterpret_cast<detail::SynchronizedObjectRegistry*>(regId.address);
            auto* object     = static_cast<taSyncObj*>(syncObjReg.getMapping(syncId));

            if (object) {
                object->__spempeimpl_applyUpdate(
                    state,
                    syncObjReg.adjustDelayForLag(rc.pessimisticLatencyInSteps),
                    flags
                );
            }
        });

    node.callIfServer(
        [](hg::RN_ServerInterface&) {
            throw hg::RN_IllegalMessage("Server received a sync message");
        });
}

template <class taSyncObj, class taGameContext, class taNetwMgr>
void DefaultSyncDestroyHandler(hg::RN_NodeInterface& node, 
                               SyncId syncId) {
    node.callIfClient(
        [&](hg::RN_ClientInterface& client) {
            detail::RPCReceiverContext<taGameContext, taNetwMgr> rc{client};
            auto  regId      = rc.gameContext.template getComponent<taNetwMgr>().getRegistryId();
            auto& syncObjReg = *reinterpret_cast<detail::SynchronizedObjectRegistry*>(regId.address);
            auto* object     = static_cast<taSyncObj*>(syncObjReg.getMapping(syncId));

            if (object) {
                object->__spempeimpl_destroySelfIn(
                    static_cast<int>(syncObjReg.getDefaultDelay())
                    - (syncObjReg.adjustDelayForLag(rc.meanLatencyInSteps) + 1)
                );
            }
        });

    node.callIfServer(
        [](hg::RN_ServerInterface&) {
            throw hg::RN_IllegalMessage("Server received a sync message");
        });
}

namespace detail {
//! TODO: explanation/justification
template <class taAutodiffState>
AutodiffPackMode AlignAutodiffState_PreCompose(taAutodiffState& aAutodiffState, SyncDetails& aSyncDetails) {
    if constexpr (std::is_base_of<AutodiffStateTag, taAutodiffState>::value) {
        const auto previousPackMode = aAutodiffState.getPackMode();

        // If a full state sync was requested by the engine, we must not
        // filter out any recepients nor allow only the diff to be sent.
        const bool diffAllowed = !HasFullState(aSyncDetails.getFlags());
        if (diffAllowed) {
            aAutodiffState.setPackMode(AutodiffPackMode::PackDiff);

            if (aAutodiffState.cmp() == AUTODIFF_STATE_NO_CHANGE) {
                if (aAutodiffState.getNoChangeStreakCount() >= 1) {
                    aSyncDetails.filterSyncs([](hg::PZInteger /* aClientIndex */) -> SyncDetails::FilterResult {
                        return SyncDetails::FilterResult::Skip;
                    });
                    //HG_LOG_INFO("Align", "No change: Skip all. ({})", aAutodiffState.getNoChangeStreakCount());
                }
                else {
                    //HG_LOG_INFO("Align", "No change but streak not long enough ({})", aAutodiffState.getNoChangeStreakCount());
                }
            }
            //else HG_LOG_INFO("Align", "Change detected. ({})", aAutodiffState.getNoChangeStreakCount());
        }
        else {
            aAutodiffState.setPackMode(AutodiffPackMode::PackAll);
        }

        return previousPackMode;
    }
    else {
        // The specific return value doesn't matter here
        return AutodiffPackMode::Default;
    }
}

//! TODO: explanation/justification
template <class taAutodiffState>
void AlignAutodiffState_PostCompose(taAutodiffState& aAutodiffState, AutodiffPackMode aOldPackMode) {
    if constexpr (std::is_base_of<AutodiffStateTag, taAutodiffState>::value) {
        aAutodiffState.setPackMode(aOldPackMode);
    }
}

//! TODO: explanation/justification
template <class T>
T& StripConstFromRef(const T& aRef) {
    return const_cast<T&>(aRef);
}
} // namespace detail

// ===== All macros starting with USPEMPE_ are internal =====

#define USPEMPE_MACRO_EXPAND(_x_) _x_
#ifndef USPEMPE_MACRO_EXPAND_VA
#define USPEMPE_MACRO_EXPAND_VA(...) __VA_ARGS__
#endif // !USPEMPE_MACRO_EXPAND_VA

#define USPEMPE_GENERATE_DEFAULT_SYNC_HANDLER_EMPTY(_class_name_) /* Empty */

#define USPEMPE_GENERATE_DEFAULT_SYNC_HANDLER_CREATE(_class_name_) \
    RN_DEFINE_RPC(USPEMPE_Create##_class_name_, \
                  RN_ARGS(::jbatnozic::spempe::SyncId, syncId)) { \
        ::jbatnozic::spempe::DefaultSyncCreateHandler<_class_name_, \
                                                      ::jbatnozic::spempe::GameContext, \
                                                      ::jbatnozic::spempe::NetworkingManagerInterface>( \
            RN_NODE_IN_HANDLER(), syncId); \
    }

#define USPEMPE_GENERATE_DEFAULT_SYNC_HANDLER_UPDATE(_class_name_) \
    RN_DEFINE_RPC(USPEMPE_Update##_class_name_, \
                  RN_ARGS(::jbatnozic::spempe::SyncId, syncId, \
                          ::jbatnozic::spempe::SyncFlags, flags, \
                          _class_name_::VisibleState&, state)) { \
        ::jbatnozic::spempe::DefaultSyncUpdateHandler<_class_name_, \
                                                      ::jbatnozic::spempe::GameContext, \
                                                      ::jbatnozic::spempe::NetworkingManagerInterface>( \
            RN_NODE_IN_HANDLER(), syncId, flags, state); \
    }

#define USPEMPE_GENERATE_DEFAULT_SYNC_HANDLER_DESTROY(_class_name_) \
    RN_DEFINE_RPC(USPEMPE_Destroy##_class_name_, \
                  RN_ARGS(::jbatnozic::spempe::SyncId, syncId)) { \
        ::jbatnozic::spempe::DefaultSyncDestroyHandler<_class_name_, \
                                                       ::jbatnozic::spempe::GameContext, \
                                                       ::jbatnozic::spempe::NetworkingManagerInterface>( \
            RN_NODE_IN_HANDLER(), syncId); \
    }

#define USPEMPE_GENERATE_MULTIPLE_DEFAULT_SYNC_HANDLERS(_class_name_, _tag_1_, _tag_2_, _tag_3_, ...) \
    USPEMPE_GENERATE_DEFAULT_SYNC_HANDLER_##_tag_1_ (_class_name_) \
    USPEMPE_GENERATE_DEFAULT_SYNC_HANDLER_##_tag_2_ (_class_name_) \
    USPEMPE_GENERATE_DEFAULT_SYNC_HANDLER_##_tag_3_ (_class_name_)

#define USPEMPE_GENERATE_DEFAULT_SYNC_HANDLERS(_class_name_, ...) \
    USPEMPE_MACRO_EXPAND( \
        USPEMPE_GENERATE_MULTIPLE_DEFAULT_SYNC_HANDLERS(_class_name_, __VA_ARGS__, EMPTY, EMPTY) \
    )

//! Create default synchronization handlers for class _class_name_.
//! Events should be written together in parenthesis and can be CREATE, UPDATE and DESTROY.
//! Example of usage:
//!     SPEMPE_GENERATE_DEFAULT_SYNC_HANDLERS(MyClass, (CREATE, DESTROY));
#define SPEMPE_GENERATE_DEFAULT_SYNC_HANDLERS(_class_name_, _for_events_) \
    USPEMPE_MACRO_EXPAND( \
        USPEMPE_GENERATE_DEFAULT_SYNC_HANDLERS(_class_name_, USPEMPE_MACRO_EXPAND_VA _for_events_) \
    )

//! Calls the default sync/CREATE implementation of a SynchronizedObject
//! generated by SPEMPE_GENERATE_DEFAULT_SYNC_HANDLERS.
#define SPEMPE_SYNC_CREATE_DEFAULT_IMPL(_class_name_, _sync_details_) \
    Compose_USPEMPE_Create##_class_name_((_sync_details_).getNode(), \
                                         (_sync_details_).getRecepients(), \
                                         getSyncId())

//! Calls the default sync/UPDATE implementation of a SynchronizedObject
//! generated by SPEMPE_GENERATE_DEFAULT_SYNC_HANDLERS.
//! Note: Will properly detect and handle autodiff objects.
#define SPEMPE_SYNC_UPDATE_DEFAULT_IMPL(_class_name_, _sync_details_) \
    do { \
        const auto __spempeimpl_oldPackMode = ::jbatnozic::spempe::detail::AlignAutodiffState_PreCompose( \
            ::jbatnozic::spempe::detail::StripConstFromRef(_getCurrentState()), \
            _sync_details_ \
        ); \
        Compose_USPEMPE_Update##_class_name_((_sync_details_).getNode(), \
                                             (_sync_details_).getRecepients(), \
                                             getSyncId(), \
                                             (_sync_details_).getFlags(), \
                                             _getCurrentState()); \
        ::jbatnozic::spempe::detail::AlignAutodiffState_PostCompose( \
            ::jbatnozic::spempe::detail::StripConstFromRef(_getCurrentState()), \
            __spempeimpl_oldPackMode \
        ); \
    } while (false)

//! Calls the default sync/DESTROY implementation of a SynchronizedObject
//! generated by SPEMPE_GENERATE_DEFAULT_SYNC_HANDLERS.
#define SPEMPE_SYNC_DESTROY_DEFAULT_IMPL(_class_name_, _sync_details_) \
    Compose_USPEMPE_Destroy##_class_name_((_sync_details_).getNode(), \
                                          (_sync_details_).getRecepients(), \
                                          getSyncId())

//! Creates an instance of RPCReceiverContext from a reference to either a RN_ServerInterface or
//! a RN_ClientInterface.
//! The use of this macro in user code is discouraged (just use spempe::GetRPCReceiverContext directly).
#define SPEMPE_GET_RPC_RECEIVER_CONTEXT(_node_) \
    (::jbatnozic::spempe::detail::GetRPCReceiverContext<::jbatnozic::spempe::GameContext, \
                                                        ::jbatnozic::spempe::NetworkingManagerInterface>(_node_))

} // namespace spempe
} // namespace jbatnozic

#include <SPeMPE/Managers/Networking_manager_interface.hpp>

#endif // !SPEMPE_GAME_OBJECT_FRAMEWORK_GAME_OBJECT_BASES_HPP
