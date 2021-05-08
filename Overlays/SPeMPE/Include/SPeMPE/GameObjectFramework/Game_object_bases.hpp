#ifndef SPEMPE_GAME_OBJECT_FRAMEWORK_GAME_OBJECT_BASES_HPP
#define SPEMPE_GAME_OBJECT_FRAMEWORK_GAME_OBJECT_BASES_HPP

#include <Hobgoblin/QAO.hpp>
#include <Hobgoblin/Utility/State_scheduler.hpp>
#include <SPeMPE/GameContext/Game_context.hpp>
#include <SPeMPE/GameObjectFramework/Synchronized_object_registry.hpp>

#include <typeinfo>

#define SPEMPE_TYPEID_SELF (typeid(decltype(*this)))

namespace jbatnozic {
namespace spempe {

namespace hg = ::jbatnozic::hobgoblin;

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

//! Objects which are essential for the game's state, but will not be synchronized
//! with clients. For use with singleplayer games, or for server-side controller
//! objects in multiplayer games.
class StateObject : public GameObjectBase {
public:
    using GameObjectBase::GameObjectBase;
};

struct IfMaster {};
struct IfDummy  {};

class SynchronizedObjectBase : public StateObject {
public:
    virtual ~SynchronizedObjectBase();

    SyncId getSyncId() const noexcept;
    bool isMasterObject() const noexcept;

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
    virtual void eventUpdate(IfDummy)        {}
    virtual void eventPostUpdate(IfDummy)    {}
    virtual void eventDraw1(IfDummy)         {}
    virtual void eventDraw2(IfDummy)         {}
    virtual void eventDrawGUI(IfDummy)       {}
    virtual void eventFinalizeFrame(IfDummy) {}

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
    friend class SynchronizedObjectRegistry;

    SynchronizedObjectRegistry& _syncObjReg;
    const SyncId _syncId;

    //! Called when it's needed to sync this object's creation to one or more recepeints.
    virtual void _syncCreateImpl( hg::RN_NodeInterface& aNode,
                                  const std::vector<hg::PZInteger>& aRecepients) const = 0;

    //! Called when it's needed to sync this object's update to one or more recepeints.
    virtual void _syncUpdateImpl( hg::RN_NodeInterface& aNode,
                                  const std::vector<hg::PZInteger>& aRecepients) const = 0;

    //! Called when it's needed to sync this object's destruction to one or more recepeints.
    virtual void _syncDestroyImpl(hg::RN_NodeInterface& aNode,
                                  const std::vector<hg::PZInteger>& aRecepients) const = 0;

    virtual void _scheduleAndAdvanceStatesForDummy(hg::PZInteger aMaxStateSchedulerSize) = 0;
};

//! Objects which are essential to the game's state, so they are both saved when
//! writing game state, and synchronized with clients in multiplayer sessions.
//! For example, units, terrain, interactible items (and, basically, most other 
//! game objects).
class SynchronizedObjectOld : public StateObject {
public:
    SynchronizedObjectOld(hg::QAO_RuntimeRef runtimeRef, 
                          const std::type_info& typeInfo,
                          int executionPriority, 
                          std::string name,
                          SynchronizedObjectRegistry& syncObjReg, 
                          SyncId syncId);

    // virtual ~SynchronizedObjectOld();

    SyncId getSyncId() const noexcept;

    bool isMasterObject() const noexcept;

protected:
    void syncCreate() const;
    void syncUpdate() const;
    void syncDestroy() const;

private:
    SynchronizedObjectRegistry& _syncObjReg;
    const SyncId _syncId;

    virtual void _onSyncCreateNeeded( hg::RN_NodeInterface& node, const std::vector<hg::PZInteger>& rec) const = 0;
    virtual void _onSyncUpdateNeeded( hg::RN_NodeInterface& node, const std::vector<hg::PZInteger>& rec) const = 0;
    virtual void _onSyncDestroyNeeded(hg::RN_NodeInterface& node, const std::vector<hg::PZInteger>& rec) const = 0;

    friend class SynchronizedObjectRegistry;
};

template <class taVisibleState>
class SynchronizedObject : public SynchronizedObjectBase {
public:
    using SyncObjSuper = SynchronizedObject;
    using VisibleState = taVisibleState;

protected:
    SynchronizedObject(hg::QAO_RuntimeRef aRuntimeRef,
                       const std::type_info& aTypeInfo,
                       int aExecutionPriority,
                       std::string aName,
                       SynchronizedObjectRegistry& aSyncObjReg,
                       SyncId aSyncId = SYNC_ID_NEW)
        : SynchronizedObjectBase{ aRuntimeRef
                                , aTypeInfo
                                , aExecutionPriority
                                , aName
                                , aSyncObjReg
                                , aSyncId
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
};

} // namespace spempe
} // namespace jbatnozic

#endif // !SPEMPE_GAME_OBJECT_FRAMEWORK_GAME_OBJECT_BASES_HPP
