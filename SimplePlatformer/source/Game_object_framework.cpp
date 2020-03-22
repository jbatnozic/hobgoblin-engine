
#include <cassert>

#include "Game_object_framework.hpp"
#include "Global_program_state.hpp"

GlobalProgramState& GOF_Base::global() const {
    assert(getRuntime() && "Game object not registered to any runtime!");
    return *(getRuntime()->getUserData<GlobalProgramState>());
}

hg::QAO_Runtime& GOF_Base::rt() const {
    assert(getRuntime() && "Game object not registered to any runtime!");
    return *getRuntime();
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

}

SynchronizedObjectMapper::SynchronizedObjectMapper(RN_Node& node)
    : _node{node}
{
}

SyncId SynchronizedObjectMapper::createMasterObject(GOF_SynchronizedObject* object) {
    assert(object);

    SyncId id = _syncIdCounter;
    _syncIdCounter += 1;
    _mappings[id] = object;

    // TODO Add to _objectToSync

    return id;
}

void SynchronizedObjectMapper::createDummyObject(GOF_SynchronizedObject* object, SyncId masterSyncId) {
    assert(object);
    _mappings[masterSyncId] = object;
}

void SynchronizedObjectMapper::destroyMasterObject(GOF_SynchronizedObject* object) {
    assert(object);
    _mappings.erase(object->getSyncId());
}

GOF_SynchronizedObject* SynchronizedObjectMapper::getMapping(SyncId syncId) const {
    return _mappings.at(syncId);
}

const std::unordered_map<SyncId, GOF_SynchronizedObject*>& SynchronizedObjectMapper::getAllMappings() const {
    return _mappings;
}