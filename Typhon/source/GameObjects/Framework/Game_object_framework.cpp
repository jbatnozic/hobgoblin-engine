
#include <Hobgoblin/Utility/Exceptions.hpp>

#include <cassert>

#include "GameObjects/Framework/Game_object_framework.hpp"
#include "GameContext/Game_context.hpp"

GameContext& GOF_Base::ctx() const {
    assert(getRuntime() && "Game object not registered to any runtime!");
    return *(getRuntime()->getUserData<GameContext>());
}

KbInputTracker& GOF_Base::kbi() const {
    return ctx().windowMgr.getKeyboardInput();
}

const hg::gr::Multisprite& GOF_Base::getspr(SpriteId spriteId) const {
    return ctx().getResourceConfig().spriteLoader->getSprite(static_cast<hg::PZInteger>(spriteId));
}

// ========================================================================= //

GOF_SynchronizedObject::GOF_SynchronizedObject(QAO_RuntimeRef runtimeRef, const std::type_info& typeInfo,
                                               int executionPriority, std::string name,
                                               GOF_SynchronizedObjectRegistry& syncObjReg, GOF_SyncId syncId)
    : GOF_StateObject{runtimeRef, typeInfo, executionPriority, std::move(name)}
    , _syncObjReg{syncObjReg}
    , _syncId{(syncId == GOF_SYNC_ID_CREATE_MASTER) ? _syncObjReg.registerMasterObject(this) : syncId}
{
    _syncObjReg.registerDummyObject(this, _syncId);
}

GOF_SynchronizedObject::~GOF_SynchronizedObject() {
    _syncObjReg.unregisterObject(this);
}

GOF_SyncId GOF_SynchronizedObject::getSyncId() const noexcept {
    return _syncId & ~std::int64_t{1};
}

bool GOF_SynchronizedObject::isMasterObject() const noexcept {
    return (_syncId & 1) != 0;
}

void GOF_SynchronizedObject::syncCreate() const {
    if (!isMasterObject()) {
        throw hg::util::TracedLogicError("Dummy objects cannot request synchronization!");
    }
    _syncObjReg.syncObjectCreate(this);
}

void GOF_SynchronizedObject::syncUpdate() const {
    if (!isMasterObject()) {
        throw hg::util::TracedLogicError("Dummy objects cannot request synchronization!");
    }
    _syncObjReg.syncObjectUpdate(this);
}

void GOF_SynchronizedObject::syncDestroy() const {
    if (!isMasterObject()) {
        throw hg::util::TracedLogicError("Dummy objects cannot request synchronization!");
    }
    _syncObjReg.syncObjectDestroy(this);
}