
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
                                               RegistryId aRegId,
                                               SyncId aSyncId)
    : StateObject{aRuntimeRef, aTypeInfo, aExecutionPriority, std::move(aName)}
    , _syncObjReg{*reinterpret_cast<detail::SynchronizedObjectRegistry*>(aRegId.address)}
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

void SynchronizedObjectBase::__spempeimpl_destroySelfIn(int aStepCount) {
    _deathCounter = (aStepCount > 0) ? aStepCount : 0;
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

void SynchronizedObjectBase::_eventUpdate(IfDummy) {
    _scheduleAndAdvanceStatesForDummy((_syncObjReg.getDefaultDelay() + 1) * 2);

    if (_deathCounter > 0) {
        _deathCounter -= 1;
    }
    else if (_deathCounter == 0) {
        hg::QAO_PDestroy(this);
    }
}

bool SynchronizedObjectBase::_willDieAfterUpdate() const {
    return (_deathCounter == 0);
}

void SynchronizedObjectBase::_eventStartFrame() {
    if (isMasterObject()) {
        _eventStartFrame(IfMaster{});
    }
    else {
        _eventStartFrame(IfDummy{});
    }
}

void SynchronizedObjectBase::_eventPreUpdate() {
    if (isMasterObject()) {
        _eventPreUpdate(IfMaster{});
    }
    else {
        _eventPreUpdate(IfDummy{});
    }
}

void SynchronizedObjectBase::_eventUpdate() {
    if (isMasterObject()) {
        _eventUpdate(IfMaster{});
    }
    else {
        _eventUpdate(IfDummy{});
    }
}

void SynchronizedObjectBase::_eventPostUpdate() {
    if (isMasterObject()) {
        _eventPostUpdate(IfMaster{});
    }
    else {
        _eventPostUpdate(IfDummy{});
    }
}

void SynchronizedObjectBase::_eventDraw1() {
    if (isMasterObject()) {
        _eventDraw1(IfMaster{});
    }
    else {
        _eventDraw1(IfDummy{});
    }
}

void SynchronizedObjectBase::_eventDraw2() {
    if (isMasterObject()) {
        _eventDraw2(IfMaster{});
    }
    else {
        _eventDraw2(IfDummy{});
    }
}

void SynchronizedObjectBase::_eventDrawGUI() {
    if (isMasterObject()) {
        _eventDrawGUI(IfMaster{});
    }
    else {
        _eventDrawGUI(IfDummy{});
    }
}

void SynchronizedObjectBase::_eventFinalizeFrame() {
    if (isMasterObject()) {
        _eventFinalizeFrame(IfMaster{});
    }
    else {
        _eventFinalizeFrame(IfDummy{});
    }
}

} // namespace spempe
} // namespace jbatnozic