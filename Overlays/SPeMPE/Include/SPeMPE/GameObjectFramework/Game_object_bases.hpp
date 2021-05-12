#ifndef SPEMPE_GAME_OBJECT_FRAMEWORK_GAME_OBJECT_BASES_HPP
#define SPEMPE_GAME_OBJECT_FRAMEWORK_GAME_OBJECT_BASES_HPP

#include <Hobgoblin/QAO.hpp>
#include <Hobgoblin/RigelNet.hpp>
#include <Hobgoblin/RigelNet_Macros.hpp>
#include <Hobgoblin/Utility/State_scheduler.hpp>
#include <SPeMPE/GameContext/Game_context.hpp>
#include <SPeMPE/GameObjectFramework/Synchronized_object_registry.hpp>

#include <algorithm>
#include <cstdint>
#include <typeinfo>
#include <utility>
#include <vector>

#define SPEMPE_TYPEID_SELF (typeid(decltype(*this)))

namespace jbatnozic {
namespace spempe {

namespace hg = ::jbatnozic::hobgoblin;

//! Not to be inherited from directly. Use one of the classes below.
class GameObjectBase : public hg::QAO_Base {
public:
    using hg::QAO_Base::QAO_Base;

    //! Shorthand to get the GameContext in which this object lives.
    GameContext& ctx() const;

    //! Shorthand to get one of the context components of the context 
    //! in which this object lives.
    template <class taComponent>
    taComponent& ccomp() {
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

    virtual ~SynchronizedObjectBase() override;

    SyncId getSyncId() const noexcept;
    bool isMasterObject() const noexcept;

    //! Internal implementation, do not call manually!
    void __spempeimpl_destroySelfIn(int aStepCount);

protected:
    // Call the following to sync this object's creation/update/destruction right away.

    void doSyncCreate() const;
    void doSyncUpdate() const;
    void doSyncDestroy() const;

    // These overloads will be called if the object is a Master object 
    // (that is, executing in a Privileged context).

    virtual void eventStartFrame(IfMaster)    {}
    virtual void eventPreUpdate(IfMaster)     {}
    virtual void eventUpdate(IfMaster)        {}
    virtual void eventPostUpdate(IfMaster)    {}
    virtual void eventDraw1(IfMaster)         {}
    virtual void eventDraw2(IfMaster)         {}
    virtual void eventDrawGUI(IfMaster)       {}
    virtual void eventFinalizeFrame(IfMaster) {}

    // These overloads will be called if the object is a Dummy object 
    // (that is, executing in a non-Privileged context).

    virtual void eventStartFrame(IfDummy)    {}
    virtual void eventPreUpdate(IfDummy)     {}
    virtual void eventUpdate(IfDummy)         ; // This one is special!
    virtual void eventPostUpdate(IfDummy)    {}
    virtual void eventDraw1(IfDummy)         {}
    virtual void eventDraw2(IfDummy)         {}
    virtual void eventDrawGUI(IfDummy)       {}
    virtual void eventFinalizeFrame(IfDummy) {}

    // Misc.
    bool _willDieAfterUpdate() const;

    // If you override any of the below, the overloads above will not be used.
    // The same code will be executed on both ends.

    void eventStartFrame() override;
    void eventPreUpdate() override;
    void eventUpdate() override;
    void eventPostUpdate() override;
    void eventDraw1() override;
    void eventDraw2() override;
    void eventDrawGUI() override;
    void eventFinalizeFrame() override;

private:
    friend class detail::SynchronizedObjectRegistry;

    detail::SynchronizedObjectRegistry& _syncObjReg;
    const SyncId _syncId;

    int _deathCounter = -1;

    //! Called when it's needed to sync this object's creation to one or more recepeints.
    virtual void _syncCreateImpl(SyncDetails& aSyncDetails) const = 0;

    //! Called when it's needed to sync this object's update to one or more recepeints.
    virtual void _syncUpdateImpl(SyncDetails& aSyncDetails) const = 0;

    //! Called when it's needed to sync this object's destruction to one or more recepeints.
    virtual void _syncDestroyImpl(SyncDetails& aSyncDetails) const = 0;

    virtual void _scheduleAndAdvanceStatesForDummy(hg::PZInteger aMaxStateSchedulerSize) = 0;

    virtual void _setStateSchedulerDefaultDelay(hg::PZInteger aNewDefaultDelaySteps) = 0;
};

/*
Note:
    If an object drived from SynchronizedObject (below), and transitively from
    SynchronizedObjectBase overries either eventUpdate() or eventUpdate(IfDummy),
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
        SynchronizedObjectBase::eventUpdate(::jbatnozic::spempe::IfDummy{}); \
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

    //! Internal implementation, do not call manually!
    void __spempeimpl_applyUpdate(const VisibleState& aNewState, hg::PZInteger aDelaySteps) {
        _ssch.putNewState(aNewState, aDelaySteps);
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
            reinterpret_cast<detail::SynchronizedObjectRegistry*>(aRegId.address)->getDefaultDelay()
        }
    {
    }

    taVisibleState& _getCurrentState() {
        return _ssch.getCurrentState();
    }

    const taVisibleState& _getCurrentState() const {
        return _ssch.getCurrentState();
    }

private:
    hg::util::StateScheduler<taVisibleState> _ssch;

    void _scheduleAndAdvanceStatesForDummy(hg::PZInteger aMaxStateSchedulerSize) override final {
        _ssch.scheduleNewStates();
        _ssch.advance();
        _ssch.advanceDownTo(aMaxStateSchedulerSize);
    }

    void _setStateSchedulerDefaultDelay(hg::PZInteger aNewDefaultDelaySteps) override final {
        _ssch.setDefaultDelay(aNewDefaultDelaySteps);
    }
};

///////////////////////////////////////////////////////////////////////////
// SYNCHRONIZATION HELPER MACROS                                         //
///////////////////////////////////////////////////////////////////////////

template <class taSyncObj, class taContext, class taNetwMgr>
void DefaultSyncCreateHandler(hg::RN_NodeInterface& node, 
                              SyncId syncId) {
    node.callIfClient(
        [&](hg::RN_ClientInterface& client) {
            auto& ctx     = *client.getUserDataOrThrow<taContext>();
            auto& runtime = ctx.getQAORuntime();
            auto  regId   = ctx.getComponent<taNetwMgr>().getRegistryId();

            hg::QAO_PCreate<taSyncObj>(&runtime, regId, syncId);
        });

    node.callIfServer(
        [](hg::RN_ServerInterface& server) {
            throw hg::RN_IllegalMessage("Server received a sync message");
        });
}

template <class taSyncObj, class taContext, class taNetwMgr>
void DefaultSyncUpdateHandler(hg::RN_NodeInterface& node,
                              SyncId syncId, 
                              typename taSyncObj::VisibleState& state) {
    node.callIfClient(
        [&](hg::RN_ClientInterface& client) {
            auto& ctx        = *client.getUserDataOrThrow<taContext>();
            auto& runtime    = ctx.getQAORuntime();
            auto  regId      = ctx.getComponent<taNetwMgr>().getRegistryId();
            auto& syncObjReg = *reinterpret_cast<detail::SynchronizedObjectRegistry*>(regId.address);
            auto& object     = *static_cast<taSyncObj*>(syncObjReg.getMapping(syncId));

            const auto latency = client.getServerConnector().getRemoteInfo().latency;
            using Time = std::remove_cv_t<decltype(latency)>;
            const auto dt = std::chrono::duration_cast<Time>(ctx.getRuntimeConfig().deltaTime);
            const auto delaySteps = static_cast<hg::PZInteger>(latency / dt) / 2;

            object.__spempeimpl_applyUpdate(state, delaySteps);
        });

    node.callIfServer(
        [](hg::RN_ServerInterface& server) {
            throw hg::RN_IllegalMessage("Server received a sync message");
        });
}

template <class taSyncObj, class taContext, class taNetwMgr>
void DefaultSyncDestroyHandler(hg::RN_NodeInterface& node, 
                               SyncId syncId) {
    node.callIfClient(
        [&](hg::RN_ClientInterface& client) {
            auto& ctx        = *client.getUserDataOrThrow<taContext>();
            auto& runtime    = ctx.getQAORuntime();
            auto  regId      = ctx.getComponent<taNetwMgr>().getRegistryId();
            auto& syncObjReg = *reinterpret_cast<detail::SynchronizedObjectRegistry*>(regId.address);
            auto* object     = static_cast<taSyncObj*>(syncObjReg.getMapping(syncId));

            const auto latency = client.getServerConnector().getRemoteInfo().latency;
            using Time = std::remove_cv_t<decltype(latency)>;
            const auto dt = std::chrono::duration_cast<Time>(ctx.getRuntimeConfig().deltaTime);
            const auto delaySteps = static_cast<hg::PZInteger>(latency / dt) / 2;

            object->__spempeimpl_destroySelfIn(static_cast<int>(syncObjReg.getDefaultDelay()) - (delaySteps + 1));
        });

    node.callIfServer(
        [](hg::RN_ServerInterface& server) {
            throw hg::RN_IllegalMessage("Server received a sync message");
        });
}

#define SPEMPEIMPL_MACRO_CONCAT_WITHARG(_x_, _y_, _arg_) _x_##_y_(_arg_)
#define SPEMPEIMPL_MACRO_EXPAND(_x_) _x_
#define SPEMPEIMPL_MACRO_EXPAND_VA(...) __VA_ARGS__

#define SPEMPEIMPL_GENERATE_DEFAULT_SYNC_HANDLER_EMPTY(_class_name_) /* Empty */

#define SPEMPEIMPL_GENERATE_DEFAULT_SYNC_HANDLER_CREATE(_class_name_) \
    RN_DEFINE_RPC(SPEMPEIMPL_Create##_class_name_, \
                  RN_ARGS(::jbatnozic::spempe::SyncId, syncId)) { \
        ::jbatnozic::spempe::DefaultSyncCreateHandler<_class_name_, \
                                                      ::jbatnozic::spempe::GameContext, \
                                                      ::jbatnozic::spempe::NetworkingManagerInterface>( \
            RN_NODE_IN_HANDLER(), syncId); \
    }

#define SPEMPEIMPL_GENERATE_DEFAULT_SYNC_HANDLER_UPDATE(_class_name_) \
    RN_DEFINE_RPC(SPEMPEIMPL_Update##_class_name_, \
                  RN_ARGS(::jbatnozic::spempe::SyncId, syncId, _class_name_::VisibleState&, state)) { \
        ::jbatnozic::spempe::DefaultSyncUpdateHandler<_class_name_, \
                                                      ::jbatnozic::spempe::GameContext, \
                                                      ::jbatnozic::spempe::NetworkingManagerInterface>( \
            RN_NODE_IN_HANDLER(), syncId, state); \
    }

#define SPEMPEIMPL_GENERATE_DEFAULT_SYNC_HANDLER_DESTROY(_class_name_) \
    RN_DEFINE_RPC(SPEMPEIMPL_Destroy##_class_name_, \
                  RN_ARGS(::jbatnozic::spempe::SyncId, syncId)) { \
        ::jbatnozic::spempe::DefaultSyncDestroyHandler<_class_name_, \
                                                       ::jbatnozic::spempe::GameContext, \
                                                       ::jbatnozic::spempe::NetworkingManagerInterface>( \
            RN_NODE_IN_HANDLER(), syncId); \
    }

#define SPEMPEIMPL_GENERATE_MULTIPLE_DEFAULT_SYNC_HANDLERS(_class_name_, _tag_1_, _tag_2_, _tag_3_, ...) \
    SPEMPEIMPL_GENERATE_DEFAULT_SYNC_HANDLER_##_tag_1_ (_class_name_) \
    SPEMPEIMPL_GENERATE_DEFAULT_SYNC_HANDLER_##_tag_2_ (_class_name_) \
    SPEMPEIMPL_GENERATE_DEFAULT_SYNC_HANDLER_##_tag_3_ (_class_name_)

#define SPEMPEIMPL_GENERATE_DEFAULT_SYNC_HANDLERS(_class_name_, ...) \
    SPEMPEIMPL_MACRO_EXPAND( \
        SPEMPEIMPL_GENERATE_MULTIPLE_DEFAULT_SYNC_HANDLERS(_class_name_, __VA_ARGS__, EMPTY, EMPTY) \
    )

#define SPEMPE_GENERATE_DEFAULT_SYNC_HANDLERS(_class_name_, _for_events_) \
    SPEMPEIMPL_MACRO_EXPAND( \
        SPEMPEIMPL_GENERATE_DEFAULT_SYNC_HANDLERS(_class_name_, SPEMPEIMPL_MACRO_EXPAND_VA _for_events_) \
    )

#define SPEMPE_SYNC_CREATE_DEFAULT_IMPL(_class_name_, _sync_details_) \
    Compose_SPEMPEIMPL_Create##_class_name_(_sync_details_.getNode(), \
                                            _sync_details_.getRecepients(), getSyncId())

#define SPEMPE_SYNC_UPDATE_DEFAULT_IMPL(_class_name_, _sync_details_) \
    Compose_SPEMPEIMPL_Update##_class_name_(_sync_details_.getNode(), \
                                            _sync_details_.getRecepients(), getSyncId(), \
                                            _getCurrentState())

#define SPEMPE_SYNC_DESTROY_DEFAULT_IMPL(_class_name_, _sync_details_) \
    Compose_SPEMPEIMPL_Destroy##_class_name_(_sync_details_.getNode(), \
                                            _sync_details_.getRecepients(), getSyncId())

} // namespace spempe
} // namespace jbatnozic

#endif // !SPEMPE_GAME_OBJECT_FRAMEWORK_GAME_OBJECT_BASES_HPP
