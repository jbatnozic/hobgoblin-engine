
#include <Hobgoblin/Utility/Exceptions.hpp>

#include <cassert>

#include "Game_object_framework.hpp"
#include "Game_context.hpp"

GameContext& GOF_Base::ctx() const {
    assert(getRuntime() && "Game object not registered to any runtime!");
    return *(getRuntime()->getUserData<GameContext>());
}

KbInputTracker& GOF_Base::kbi() const {
    return ctx().windowMgr.getKeyboardInput();
}

// ========================================================================= //

namespace {

void GetIndicesForComposingToEveryone(const RN_Node& node, std::vector<hg::PZInteger>& vec) {
    vec.clear();
    if (!RN_IsServer(node.getType())) {
        // Do nothing
    }
    else if (node.getType() == RN_NodeType::UdpServer) {
        auto& server = static_cast<const RN_UdpServer&>(node);
        for (hg::PZInteger i = 0; i < server.getSize(); i += 1) {
            auto& client = server.getClient(i);
            if (client.getStatus() == RN_ConnectorStatus::Connected) {
                vec.push_back(i);
            }
        }
    }
    /*else if (node.getType() == RN_NodeType::TcpServer) {
        // ...
    }*/
}

} // namespace

SynchronizedObjectManager::SynchronizedObjectManager(RN_Node& node)
    : _node{&node}
{
}

void SynchronizedObjectManager::setNode(RN_Node& node) {
    _node = &node;
}

SyncId SynchronizedObjectManager::registerMasterObject(GOF_SynchronizedObject* object) {
    assert(object);

    // Bit 0 represents "masterness"
    const SyncId id = _syncIdCounter | 1;
    _syncIdCounter += 2;
    _mappings[id] = object;

    _newlyCreatedObjects.insert(object);

    return id;
}

void SynchronizedObjectManager::registerDummyObject(GOF_SynchronizedObject* object, SyncId masterSyncId) {
    assert(object);
    _mappings[masterSyncId] = object;
}

void SynchronizedObjectManager::unregisterObject(GOF_SynchronizedObject* object) {
    assert(object);
    if (object->isMasterObject()) {
        // TODO If object is in _newlyCreatedObjects, erase it from there,
        // sync create, then continue

        auto iter = _alreadyDestroyedObjects.find(object);
        if (iter == _alreadyDestroyedObjects.end()) {
            throw hg::util::TracedLogicError("Unregistering object which did not sync its destruction");
        }
        else {
            _alreadyDestroyedObjects.erase(iter);
        }
    }
    _mappings.erase(object->getSyncId());
}

GOF_SynchronizedObject* SynchronizedObjectManager::getMapping(SyncId syncId) const {
    return _mappings.at(syncId);
}

void SynchronizedObjectManager::syncStateUpdates() {
    GetIndicesForComposingToEveryone(*_node, _recepientVec);

    // Sync creations:
    for (auto* object : _newlyCreatedObjects) {
        object->syncCreateImpl(*_node, _recepientVec);
    }
    _newlyCreatedObjects.clear();

    // Sync updates:
    for (auto& pair : _mappings) {
        GOF_SynchronizedObject* object = pair.second;
        
        auto iter = _alreadyUpdatedObjects.find(object);
        if (iter != _alreadyUpdatedObjects.end()) {
            _alreadyUpdatedObjects.erase(iter);
        }
        else {
            object->syncUpdateImpl(*_node, _recepientVec);
        }
    }

    // Sync destroys - not needed (dealt with in destructors)
}

void SynchronizedObjectManager::syncCompleteState(hg::PZInteger clientIndex) {
    _recepientVec.resize(1);
    _recepientVec[0] = clientIndex;

    for (auto& mapping : _mappings) {
        auto* object = mapping.second;
        object->syncCreateImpl(*_node, _recepientVec);
        object->syncUpdateImpl(*_node, _recepientVec);
    }
}

void SynchronizedObjectManager::syncObjectCreate(const GOF_SynchronizedObject* object) {
    assert(object);
    GetIndicesForComposingToEveryone(*_node, _recepientVec);
    object->syncCreateImpl(*_node, _recepientVec);

    _newlyCreatedObjects.erase(object);
}

void SynchronizedObjectManager::syncObjectUpdate(const GOF_SynchronizedObject* object) {
    assert(object);
    GetIndicesForComposingToEveryone(*_node, _recepientVec);
    object->syncUpdateImpl(*_node, _recepientVec);

    _alreadyUpdatedObjects.insert(object);
}

void SynchronizedObjectManager::syncObjectDestroy(const GOF_SynchronizedObject* object) {
    assert(object);
    GetIndicesForComposingToEveryone(*_node, _recepientVec);
    object->syncDestroyImpl(*_node, _recepientVec);

    _alreadyDestroyedObjects.insert(object);
}

// ========================================================================= //

void GOF_SynchronizedObject::syncCreate() const {
    if (!isMasterObject()) {
        throw hg::util::TracedLogicError("Dummy objects cannot request synchronization!");
    }
    _syncObjMgr.syncObjectCreate(this);
}

void GOF_SynchronizedObject::syncUpdate() const {
    if (!isMasterObject()) {
        throw hg::util::TracedLogicError("Dummy objects cannot request synchronization!");
    }
    _syncObjMgr.syncObjectUpdate(this);
}

void GOF_SynchronizedObject::syncDestroy() const {
    if (!isMasterObject()) {
        throw hg::util::TracedLogicError("Dummy objects cannot request synchronization!");
    }
    _syncObjMgr.syncObjectDestroy(this);
}