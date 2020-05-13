#ifndef SYNCHRONIZED_OBJECT_REGISTRY
#define SYNCHRONIZED_OBJECT_REGISTRY

#include <unordered_map>
#include <unordered_set>

#include "GameObjects/Framework/Common.hpp"

class GOF_SynchronizedObjectRegistry : public hg::util::NonCopyable, public hg::util::NonMoveable {
    // TODO Cover edge case when an object is created and then immediately destroyed (in the same step)
public:
    GOF_SynchronizedObjectRegistry(RN_Node& node);

    void setNode(RN_Node& node);

    GOF_SyncId registerMasterObject(GOF_SynchronizedObject* object);
    void registerDummyObject(GOF_SynchronizedObject* object, GOF_SyncId masterSyncId);
    void unregisterObject(GOF_SynchronizedObject* object);

    GOF_SynchronizedObject* getMapping(GOF_SyncId syncId) const;

    void syncObjectCreate(const GOF_SynchronizedObject* object);
    void syncObjectUpdate(const GOF_SynchronizedObject* object);
    void syncObjectDestroy(const GOF_SynchronizedObject* object);

    void syncStateUpdates();
    void syncCompleteState(hg::PZInteger clientIndex);

private:
    std::unordered_map<GOF_SyncId, GOF_SynchronizedObject*> _mappings;
    std::unordered_set<const GOF_SynchronizedObject*> _newlyCreatedObjects;
    std::unordered_set<const GOF_SynchronizedObject*> _alreadyUpdatedObjects;
    std::unordered_set<const GOF_SynchronizedObject*> _alreadyDestroyedObjects;
    std::vector<hg::PZInteger> _recepientVec;
    GOF_SyncId _syncIdCounter = 2;
    RN_Node* _node;
};

#endif // !SYNCHRONIZED_OBJECT_REGISTRY