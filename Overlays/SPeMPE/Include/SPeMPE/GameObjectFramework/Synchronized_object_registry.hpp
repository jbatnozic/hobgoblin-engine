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

struct RegistryId {
    std::intptr_t address;
};

namespace detail {
class SynchronizedObjectRegistry;
} // namespace detail

struct SyncDetails {
    //! The node through which the sync messages need to be sent.
    hg::RN_NodeInterface& getNode() const {
        return *_node;
    }

    //! Vector of client indices of (recepients) to which sync messages
    //! need to be sent.
    const std::vector<hg::PZInteger> getRecepients() {
        return _recepients;
    }

private:
    hg::RN_NodeInterface* _node = nullptr;
    std::vector<hg::PZInteger> _recepients;

    friend class detail::SynchronizedObjectRegistry;
};

class SynchronizedObjectBase;

namespace detail {

class SynchronizedObjectRegistry 
    : public hg::util::NonCopyable
    , public hg::util::NonMoveable {
public:
    SynchronizedObjectRegistry(hg::RN_NodeInterface& node, hg::PZInteger defaultDelay);

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

    hg::PZInteger getDefaultDelay() const;

    //! 
    void setDefaultDelay(hg::PZInteger aNewDefaultDelaySteps);

private:
    std::unordered_map<SyncId, SynchronizedObjectBase*> _mappings;
    std::unordered_set<const SynchronizedObjectBase*> _newlyCreatedObjects;
    std::unordered_set<const SynchronizedObjectBase*> _alreadyUpdatedObjects;
    std::unordered_set<const SynchronizedObjectBase*> _alreadyDestroyedObjects;

    SyncDetails _syncDetails;

    SyncId _syncIdCounter = 2;
    hg::PZInteger _defaultDelay;
};

} // namespace detail

} // namespace spempe
} // namespace jbatnozic

#endif // !SPEMPE_GAME_OBJECT_FRAMEWORK_SYNCHRONIZED_OBJECT_REGISTRY_HPP

