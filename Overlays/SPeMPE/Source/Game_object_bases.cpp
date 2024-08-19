// Copyright 2024 Jovan Batnozic. Released under MS-PL licence in Serbia.
// See https://github.com/jbatnozic/Hobgoblin?tab=readme-ov-file#licence

// clang-format off


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

    if (aStepCount < _deathCounter || _deathCounter < 0) {
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
    if (ctx().getQAORuntime().getCurrentEvent() != hg::QAO_Event::POST_UPDATE) {
        HG_THROW_TRACED(hg::TracedLogicError, 0,
                        "This method may only be called during the POST_UPDATE event.");
    }
    return _syncObjReg.getAlternatingUpdatesFlag();
}

void SynchronizedObjectBase::_eventPreUpdate() {
    if (isMasterObject()) {
        _eventPreUpdate(IfMaster{});
    }
    else {
        _eventPreUpdate(IfDummy{});
    }
}

void SynchronizedObjectBase::_eventBeginUpdate() {
    if (isMasterObject()) {
        _eventBeginUpdate(IfMaster{});
    }
    else {
        _eventBeginUpdate(IfDummy{});
    }
}

void SynchronizedObjectBase::_eventUpdate1() {
    if (isMasterObject()) {
        _eventUpdate1(IfMaster{});
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

        _eventUpdate1(IfDummy{});
    }
}

void SynchronizedObjectBase::_eventUpdate2() {
    if (isMasterObject()) {
        _eventUpdate2(IfMaster{});
    }
    else {
        _eventUpdate2(IfDummy{});
    }
}

void SynchronizedObjectBase::_eventEndUpdate() {
    if (isMasterObject()) {
        _eventEndUpdate(IfMaster{});
    }
    else {
        _eventEndUpdate(IfDummy{});
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

void SynchronizedObjectBase::_eventPreDraw() {
    if (isMasterObject()) {
        _eventPreDraw(IfMaster{});
    }
    else {
        _eventPreDraw(IfDummy{});
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

void SynchronizedObjectBase::_eventPostDraw() {
    if (isMasterObject()) {
        _eventPostDraw(IfMaster{});
    }
    else {
        _eventPostDraw(IfDummy{});
    }
}

void SynchronizedObjectBase::_eventDisplay() {
    if (isMasterObject()) {
        _eventDisplay(IfMaster{});
    }
    else {
        _eventDisplay(IfDummy{});
    }
}

void SynchronizedObjectBase::__spempeimpl_syncCreateImpl(SyncControlDelegate& aSyncCtrl) const {
    _syncCreateImpl(aSyncCtrl);
}

void SynchronizedObjectBase::__spempeimpl_syncUpdateImpl(SyncControlDelegate& aSyncCtrl) const {
    _syncUpdateImpl(aSyncCtrl);
}

void SynchronizedObjectBase::__spempeimpl_syncDestroyImpl(SyncControlDelegate& aSyncCtrl) const {
    _syncDestroyImpl(aSyncCtrl);
}

//! Deactivated, Skipped, Autodiff-Skipped
#define PER_CLIENT_FLAG_COUNT 3

void SynchronizedObjectBase::__spempeimpl_setDeactivationFlagForClient(hg::PZInteger aClientIdx, bool aFlag) const {
    const auto bitIdx = (aClientIdx * PER_CLIENT_FLAG_COUNT) + 0;
    if (aFlag) {
        _remoteSyncStatuses.setBit(bitIdx);
    } else {
        _remoteSyncStatuses.clearBit(bitIdx);
    }
}

bool SynchronizedObjectBase::__spempeimpl_getDeactivationFlagForClient(hg::PZInteger aClientIdx) const {
    const auto bitIdx = (aClientIdx * PER_CLIENT_FLAG_COUNT) + 0;
    return _remoteSyncStatuses.getBit(bitIdx);
}

void SynchronizedObjectBase::__spempeimpl_setSkipFlagForClient(hg::PZInteger aClientIdx, bool aFlag) const {
    const auto bitIdx = (aClientIdx * PER_CLIENT_FLAG_COUNT) + 1;
    if (aFlag) {
        _remoteSyncStatuses.setBit(bitIdx);
    } else {
        _remoteSyncStatuses.clearBit(bitIdx);
    }
}

bool SynchronizedObjectBase::__spempeimpl_getSkipFlagForClient(hg::PZInteger aClientIdx) const {
    const auto bitIdx = (aClientIdx * PER_CLIENT_FLAG_COUNT) + 1;
    return _remoteSyncStatuses.getBit(bitIdx);
}

void SynchronizedObjectBase::__spempeimpl_setNoDiffSkipFlagForClient(hg::PZInteger aClientIdx, bool aFlag) const {
    const auto bitIdx = (aClientIdx * PER_CLIENT_FLAG_COUNT) + 2;
    if (aFlag) {
        _remoteSyncStatuses.setBit(bitIdx);
    } else {
        _remoteSyncStatuses.clearBit(bitIdx);
    }
}

bool SynchronizedObjectBase::__spempeimpl_getNoDiffSkipFlagForClient(hg::PZInteger aClientIdx) const {
    const auto bitIdx = (aClientIdx * PER_CLIENT_FLAG_COUNT) + 2;
    return _remoteSyncStatuses.getBit(bitIdx);
}

void SynchronizedObjectBase::__spempeimpl_setStateSchedulerDefaultDelay(hg::PZInteger aNewDefaultDelaySteps) {
    _setStateSchedulerDefaultDelay(aNewDefaultDelaySteps);
}

} // namespace spempe
} // namespace jbatnozic

// clang-format on
