
#include <SPeMPE/GameObjectFramework/Game_object_bases.hpp>
#include <SPeMPE/GameObjectFramework/Synchronized_object_registry.hpp>

#include <cassert>

namespace jbatnozic {
namespace spempe {

namespace {
void GetIndicesForComposingToEveryone(const hg::RN_NodeInterface& node, std::vector<hg::PZInteger>& vec) {
    vec.clear();
    if (node.isServer()) {
        auto& server = static_cast<const hg::RN_ServerInterface&>(node);
        for (hg::PZInteger i = 0; i < server.getSize(); i += 1) {
            auto& client = server.getClientConnector(i);
            if (client.getStatus() == hg::RN_ConnectorStatus::Connected) {
                vec.push_back(i);
            }
        }
    }
}
} // namespace

SynchronizedObjectRegistry::SynchronizedObjectRegistry(hg::RN_NodeInterface& node)
    : _node{&node}
{
}

void SynchronizedObjectRegistry::setNode(hg::RN_NodeInterface& node) {
    _node = &node;
}

SyncId SynchronizedObjectRegistry::registerMasterObject(SynchronizedObjectBase* object) {
    assert(object);

    // Bit 0 represents "masterness"
    const SyncId id = _syncIdCounter | 1;
    _syncIdCounter += 2;
    _mappings[(id & ~std::int64_t{1})] = object;

    _newlyCreatedObjects.insert(object);

    return id;
}

void SynchronizedObjectRegistry::registerDummyObject(SynchronizedObjectBase* object, SyncId masterSyncId) {
    assert(object);
    _mappings[(masterSyncId & ~std::int64_t{1})] = object;
}

void SynchronizedObjectRegistry::unregisterObject(SynchronizedObjectBase* object) {
    assert(object);
    if (object->isMasterObject()) {
        auto iter = _alreadyDestroyedObjects.find(object);
        if (iter == _alreadyDestroyedObjects.end()) {
            assert(false && "Unregistering object which did not sync its destruction");
            throw hg::TracedLogicError("Unregistering object which did not sync its destruction");
        }
        else {
            _alreadyDestroyedObjects.erase(iter);
        }
    }
    _mappings.erase(object->getSyncId());
}

SynchronizedObjectBase* SynchronizedObjectRegistry::getMapping(SyncId syncId) const {
    return _mappings.at(syncId);
}

void SynchronizedObjectRegistry::syncStateUpdates() {
    GetIndicesForComposingToEveryone(*_node, _recepientVec);

    // Sync creations:
    for (auto* object : _newlyCreatedObjects) {
        object->_syncCreateImpl(*_node, _recepientVec);
    }
    _newlyCreatedObjects.clear();

    // Sync updates:
    for (auto& pair : _mappings) {
        SynchronizedObjectBase* object = pair.second;

        auto iter = _alreadyUpdatedObjects.find(object);
        if (iter != _alreadyUpdatedObjects.end()) {
            // Not needed to remove elements one by one; we'll 
            // just clear the whole container at the end.
            // _alreadyUpdatedObjects.erase(iter);
        }
        else {
            object->_syncUpdateImpl(*_node, _recepientVec);
        }
    }
    _alreadyUpdatedObjects.clear();

    // Sync destroys - not needed (dealt with in destructors)
}

void SynchronizedObjectRegistry::syncCompleteState(hg::PZInteger clientIndex) {
    _recepientVec.resize(1);
    _recepientVec[0] = clientIndex;

    for (auto& mapping : _mappings) {
        auto* object = mapping.second;
        object->_syncCreateImpl(*_node, _recepientVec);
        object->_syncUpdateImpl(*_node, _recepientVec);
    }
}

void SynchronizedObjectRegistry::syncObjectCreate(const SynchronizedObjectBase* object) {
    assert(object);
    GetIndicesForComposingToEveryone(*_node, _recepientVec);
    object->_syncCreateImpl(*_node, _recepientVec);

    _newlyCreatedObjects.erase(object);
}

void SynchronizedObjectRegistry::syncObjectUpdate(const SynchronizedObjectBase* object) {
    assert(object);
    GetIndicesForComposingToEveryone(*_node, _recepientVec);

    // Synchronized object sync Update called manualy, before the registry got to
    // sync its Create. We need to fix this because the order of these is important!
    {
        auto iter = _newlyCreatedObjects.find(object);
        if (iter != _newlyCreatedObjects.end()) {
            object->_syncCreateImpl(*_node, _recepientVec);
            _newlyCreatedObjects.erase(iter);
        }
    }

    object->_syncUpdateImpl(*_node, _recepientVec);

    _alreadyUpdatedObjects.insert(object);
}

void SynchronizedObjectRegistry::syncObjectDestroy(const SynchronizedObjectBase* object) {
    assert(object);
    GetIndicesForComposingToEveryone(*_node, _recepientVec);

    // It could happen that a Synchronized object is destroyed before
    // its Update event - or even its Create event - were synced.
    {
        {
            auto iter = _newlyCreatedObjects.find(object);
            if (iter != _newlyCreatedObjects.end()) {
                object->_syncCreateImpl(*_node, _recepientVec);
                _newlyCreatedObjects.erase(iter);
            }
        }
        {
            auto iter = _alreadyUpdatedObjects.find(object);
            if (iter == _alreadyUpdatedObjects.end()) {
                object->_syncUpdateImpl(*_node, _recepientVec);
            #ifndef NDEBUG
                // This isn't really needed as we don't expect to sync an object's
                // destruction from anywhere other than its destructor, where it will
                // get unregistered so this won't matter anyway. It's left here in
                // Debug mode for optimistic correctness reasons.
                _alreadyUpdatedObjects.insert(object);
            #endif
            }
        }
    }

    object->_syncDestroyImpl(*_node, _recepientVec);

    _alreadyDestroyedObjects.insert(object);
}

} // namespace spempe
} // namespace jbatnozic