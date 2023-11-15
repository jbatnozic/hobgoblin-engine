
#include <SPeMPE/GameObjectFramework/Game_object_bases.hpp>

#include <Hobgoblin/HGExcept.hpp>

#include <algorithm>

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

bool SynchronizedObjectBase::isUsingAlternatingUpdates() const noexcept {
    return _alternatingUpdatesEnabled;
}

void SynchronizedObjectBase::__spempeimpl_destroySelfIn(int aStepCount) {
    if (aStepCount <= 0) {
        _deathCounter = 0;
        return;
    }

    if (aStepCount < _deathCounter) {
        _deathCounter = aStepCount;
    }
}

void SynchronizedObjectBase::doSyncCreate() const {
    if (!isMasterObject()) {
        HG_THROW_TRACED(hg::TracedLogicError, 0, "Dummy objects cannot request synchronization.");
    }
    _syncObjReg.syncObjectCreate(this);
}

void SynchronizedObjectBase::doSyncUpdate() const {
    if (!isMasterObject()) {
        HG_THROW_TRACED(hg::TracedLogicError, 0, "Dummy objects cannot request synchronization.");
    }
    _syncObjReg.syncObjectUpdate(this);
}

void SynchronizedObjectBase::doSyncDestroy() const {
    if (!isMasterObject()) {
        HG_THROW_TRACED(hg::TracedLogicError, 0, "Dummy objects cannot request synchronization");
    }
    _syncObjReg.syncObjectDestroy(this);
}

bool SynchronizedObjectBase::_willUpdateDeleteThis() const {
    return (_deathCounter == 0);
}

void SynchronizedObjectBase::_enableAlternatingUpdates() {
    _alternatingUpdatesEnabled = true;
    _setStateSchedulerDefaultDelay(_syncObjReg.getDefaultDelay());
}

bool SynchronizedObjectBase::_didAlternatingUpdatesSync() const {
    if (ctx().getQAORuntime().getCurrentEvent() != hg::QAO_Event::FinalizeFrame) {
        HG_THROW_TRACED(hg::TracedLogicError, 0,
                        "This method may only be called during the FinalizeFrame event.");
    }
    return _syncObjReg.getAlternatingUpdatesFlag();
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
        _advanceDummyAndScheduleNewStates();

        if (_deathCounter > 0) {
            _deathCounter -= 1;
        }
        else if (_deathCounter == 0) {
            QAO_PDestroy(this);
            return;
        }

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