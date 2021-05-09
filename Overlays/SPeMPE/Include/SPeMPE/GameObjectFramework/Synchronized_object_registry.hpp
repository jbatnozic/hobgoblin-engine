#ifndef SPEMPE_GAME_OBJECT_FRAMEWORK_SYNCHRONIZED_OBJECT_REGISTRY_HPP
#define SPEMPE_GAME_OBJECT_FRAMEWORK_SYNCHRONIZED_OBJECT_REGISTRY_HPP

#include <Hobgoblin/Common.hpp>
#include <Hobgoblin/RigelNet.hpp>
#include <Hobgoblin/Utility/No_copy_no_move.hpp>

#include <cstdint>
#include <unordered_map>
#include <unordered_set>

namespace jbatnozic {
namespace spempe {

namespace hg = ::jbatnozic::hobgoblin;

using SyncId = std::uint64_t;
constexpr SyncId SYNC_ID_NEW = 0;

class SynchronizedObjectBase;

// TODO Cover edge case when an object is created and then immediately destroyed (in the same step)
// TODO Control state buffering from here
class SynchronizedObjectRegistry : public hg::util::NonCopyable, public hg::util::NonMoveable {
public:
    SynchronizedObjectRegistry(hg::RN_NodeInterface& node);

    void setNode(hg::RN_NodeInterface& node);

    SyncId registerMasterObject(SynchronizedObjectBase* object);
    void registerDummyObject(SynchronizedObjectBase* object, SyncId masterSyncId);
    void unregisterObject(SynchronizedObjectBase* object);

    SynchronizedObjectBase* getMapping(SyncId syncId) const;

    void syncObjectCreate(const SynchronizedObjectBase* object);
    void syncObjectUpdate(const SynchronizedObjectBase* object);
    void syncObjectDestroy(const SynchronizedObjectBase* object);

    void syncStateUpdates();
    void syncCompleteState(hg::PZInteger clientIndex);

private:
    std::unordered_map<SyncId, SynchronizedObjectBase*> _mappings;
    std::unordered_set<const SynchronizedObjectBase*> _newlyCreatedObjects;
    std::unordered_set<const SynchronizedObjectBase*> _alreadyUpdatedObjects;
    std::unordered_set<const SynchronizedObjectBase*> _alreadyDestroyedObjects;
    std::vector<hg::PZInteger> _recepientVec;
    SyncId _syncIdCounter = 2;
    hg::RN_NodeInterface* _node = nullptr;
};

} // namespace spempe
} // namespace jbatnozic

#endif // !SPEMPE_GAME_OBJECT_FRAMEWORK_SYNCHRONIZED_OBJECT_REGISTRY_HPP

