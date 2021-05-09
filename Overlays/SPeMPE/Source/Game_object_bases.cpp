
#include <SPeMPE/GameObjectFramework/Game_object_bases.hpp>

#include <Hobgoblin/Common.hpp>

namespace jbatnozic {
namespace spempe {

///////////////////////////////////////////////////////////////////////////
// SYNCHRONIZED OBJECT BASE                                              //
///////////////////////////////////////////////////////////////////////////

SynchronizedObjectBase::SynchronizedObjectBase(hg::QAO_RuntimeRef aRuntimeRef,
                                               const std::type_info& aTypeInfo,
                                               int aExecutionPriority,
                                               std::string aName,
                                               SynchronizedObjectRegistry& aSyncObjReg,
                                               SyncId aSyncId)
    : StateObject{aRuntimeRef, aTypeInfo, aExecutionPriority, std::move(aName)}
    , _syncObjReg{aSyncObjReg}
    , _syncId{(aSyncId == SYNC_ID_NEW) ? _syncObjReg.registerMasterObject(this) : aSyncId}
{
    if (!isMasterObject()) {
        _syncObjReg.registerDummyObject(this, _syncId);
    }
}

SynchronizedObjectBase::~SynchronizedObjectBase() {
    _syncObjReg.unregisterObject(this);
}

constexpr SyncId SYNC_ID_1 = 1;

SyncId SynchronizedObjectBase::getSyncId() const noexcept {
    return _syncId & ~SYNC_ID_1;
}

bool SynchronizedObjectBase::isMasterObject() const noexcept {
    return ((_syncId & SYNC_ID_1) != 0);
}

void SynchronizedObjectBase::doSyncCreate() const {
    if (!isMasterObject()) {
        throw hg::TracedLogicError("Dummy objects cannot request synchronization!");
    }
    _syncObjReg.syncObjectCreate(this);
}

void SynchronizedObjectBase::doSyncUpdate() const {
    if (!isMasterObject()) {
        throw hg::TracedLogicError("Dummy objects cannot request synchronization!");
    }
    _syncObjReg.syncObjectUpdate(this);
}

void SynchronizedObjectBase::doSyncDestroy() const {
    if (!isMasterObject()) {
        throw hg::TracedLogicError("Dummy objects cannot request synchronization!");
    }
    _syncObjReg.syncObjectDestroy(this);
}

void SynchronizedObjectBase::eventStartFrame() {
    if (isMasterObject()) {
        eventStartFrame(IfMaster{});
    }
    else {
        eventStartFrame(IfDummy{});
    }
}

void SynchronizedObjectBase::eventPreUpdate() {
    if (isMasterObject()) {
        eventPreUpdate(IfMaster{});
    }
    else {
        eventPreUpdate(IfDummy{});
    }
}

void SynchronizedObjectBase::eventUpdate() {
    if (isMasterObject()) {
        eventUpdate(IfMaster{});
    }
    else {
        eventUpdate(IfDummy{});
    }
}

void SynchronizedObjectBase::eventPostUpdate() {
    if (isMasterObject()) {
        eventPostUpdate(IfMaster{});
    }
    else {
        eventPostUpdate(IfDummy{});
    }
}

void SynchronizedObjectBase::eventDraw1() {
    if (isMasterObject()) {
        eventDraw1(IfMaster{});
    }
    else {
        eventDraw1(IfDummy{});
    }
}

void SynchronizedObjectBase::eventDraw2() {
    if (isMasterObject()) {
        eventDraw2(IfMaster{});
    }
    else {
        eventDraw2(IfDummy{});
    }
}

void SynchronizedObjectBase::eventDrawGUI() {
    if (isMasterObject()) {
        eventDrawGUI(IfMaster{});
    }
    else {
        eventDrawGUI(IfDummy{});
    }
}

void SynchronizedObjectBase::eventFinalizeFrame() {
    if (isMasterObject()) {
        eventFinalizeFrame(IfMaster{});
    }
    else {
        eventFinalizeFrame(IfDummy{});
    }
}

} // namespace spempe
} // namespace jbatnozic