
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

SyncId SynchronizedObjectMapper::mapMasterObject(GOF_SynchronizedObject& object) {
    SyncId id = _syncIdCounter;
    _syncIdCounter += 1;

    _mappings[id] = &object;

    return id;
}

void SynchronizedObjectMapper::mapDummyObject(GOF_SynchronizedObject& object, SyncId masterSyncId) {
    _mappings[masterSyncId] = &object;
}

GOF_SynchronizedObject* SynchronizedObjectMapper::getMapping(SyncId syncId) const {
    return _mappings.at(syncId);
}

const std::unordered_map<SyncId, GOF_SynchronizedObject*>& SynchronizedObjectMapper::getAllMappings() const {
    return _mappings;
}

void SynchronizedObjectMapper::unmap(SyncId syncId) {
    _mappings.erase(syncId);
}