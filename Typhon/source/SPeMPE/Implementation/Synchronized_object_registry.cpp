
#include <SPeMPE/Include/Game_object_framework.hpp>
#include <SPeMPE/Include/Synchronized_object_registry.hpp>

namespace spempe {

namespace {

void GetIndicesForComposingToEveryone(const hg::RN_Node& node, std::vector<hg::PZInteger>& vec) {
    vec.clear();
    if (!hg::RN_IsServer(node.getType())) {
        // Do nothing
    }
    else if (node.getType() == hg::RN_NodeType::UdpServer) {
        auto& server = static_cast<const hg::RN_UdpServer&>(node);
        for (hg::PZInteger i = 0; i < server.getSize(); i += 1) {
            auto& client = server.getClient(i);
            if (client.getStatus() == hg::RN_ConnectorStatus::Connected) {
                vec.push_back(i);
            }
        }
    }
    /*else if (node.getType() == RN_NodeType::TcpServer) {
        // ...
    }*/
}

} // namespace

SynchronizedObjectRegistry::SynchronizedObjectRegistry(hg::RN_Node& node)
    : _node{&node}
{
}

void SynchronizedObjectRegistry::setNode(hg::RN_Node& node) {
    _node = &node;
}

SyncId SynchronizedObjectRegistry::registerMasterObject(SynchronizedObject* object) {
    assert(object);

    // Bit 0 represents "masterness"
    const SyncId id = _syncIdCounter | 1;
    _syncIdCounter += 2;
    _mappings[(id & ~std::int64_t{1})] = object;

    _newlyCreatedObjects.insert(object);

    return id;
}

void SynchronizedObjectRegistry::registerDummyObject(SynchronizedObject* object, SyncId masterSyncId) {
    assert(object);
    _mappings[(masterSyncId & ~std::int64_t{1})] = object;
}

void SynchronizedObjectRegistry::unregisterObject(SynchronizedObject* object) {
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

SynchronizedObject* SynchronizedObjectRegistry::getMapping(SyncId syncId) const {
    return _mappings.at(syncId);
}

void SynchronizedObjectRegistry::syncStateUpdates() {
    GetIndicesForComposingToEveryone(*_node, _recepientVec);

    // Sync creations:
    for (auto* object : _newlyCreatedObjects) {
        object->syncCreateImpl(*_node, _recepientVec);
    }
    _newlyCreatedObjects.clear();

    // Sync updates:
    for (auto& pair : _mappings) {
        SynchronizedObject* object = pair.second;

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

void SynchronizedObjectRegistry::syncCompleteState(hg::PZInteger clientIndex) {
    _recepientVec.resize(1);
    _recepientVec[0] = clientIndex;

    for (auto& mapping : _mappings) {
        auto* object = mapping.second;
        object->syncCreateImpl(*_node, _recepientVec);
        object->syncUpdateImpl(*_node, _recepientVec);
    }
}

void SynchronizedObjectRegistry::syncObjectCreate(const SynchronizedObject* object) {
    assert(object);
    GetIndicesForComposingToEveryone(*_node, _recepientVec);
    object->syncCreateImpl(*_node, _recepientVec);

    _newlyCreatedObjects.erase(object);
}

void SynchronizedObjectRegistry::syncObjectUpdate(const SynchronizedObject* object) {
    assert(object);
    GetIndicesForComposingToEveryone(*_node, _recepientVec);
    object->syncUpdateImpl(*_node, _recepientVec);

    _alreadyUpdatedObjects.insert(object);
}

void SynchronizedObjectRegistry::syncObjectDestroy(const SynchronizedObject* object) {
    assert(object);
    GetIndicesForComposingToEveryone(*_node, _recepientVec);
    object->syncDestroyImpl(*_node, _recepientVec);

    _alreadyDestroyedObjects.insert(object);
}

} // namespace spempe