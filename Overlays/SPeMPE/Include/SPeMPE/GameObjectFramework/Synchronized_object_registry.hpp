#ifndef SPEMPE_GAME_OBJECT_FRAMEWORK_SYNCHRONIZED_OBJECT_REGISTRY_HPP
#define SPEMPE_GAME_OBJECT_FRAMEWORK_SYNCHRONIZED_OBJECT_REGISTRY_HPP

#include <SPeMPE/GameObjectFramework/Sync_control_delegate.hpp>
#include <SPeMPE/GameObjectFramework/Sync_id.hpp>

#include <Hobgoblin/Common.hpp>
#include <Hobgoblin/GSL/HG_adapters.hpp>
#include <Hobgoblin/RigelNet.hpp>
#include <Hobgoblin/Utility/No_copy_no_move.hpp>
#include <Hobgoblin/Utility/Packet.hpp>

#include <cstdint>
#include <functional>
#include <unordered_map>
#include <unordered_set>

namespace jbatnozic {
namespace spempe {

namespace hg = ::jbatnozic::hobgoblin;

struct RegistryId {
    std::intptr_t address;
};

class SynchronizedObjectBase;

namespace detail {

class SynchronizedObjectRegistry 
    : public hg::util::NonCopyable
    , public hg::util::NonMoveable {
public:
    SynchronizedObjectRegistry(hg::RN_NodeInterface& node, hg::PZInteger defaultDelay);

    void setNode(hg::RN_NodeInterface& node);
    hg::RN_NodeInterface& getNode() const;

    SyncId registerMasterObject(SynchronizedObjectBase* object);
    void registerDummyObject(SynchronizedObjectBase* object, SyncId masterSyncId);
    void unregisterObject(SynchronizedObjectBase* object);

    void destroyAllRegisteredObjects();

    //! Can return nullptr if there is no matching object.
    SynchronizedObjectBase* getMapping(SyncId syncId) const;

    void syncObjectCreate(const SynchronizedObjectBase* object);
    void syncObjectUpdate(const SynchronizedObjectBase* object);
    void syncObjectDestroy(const SynchronizedObjectBase* object);

    void syncStateUpdates();
    void syncCompleteState(hg::PZInteger clientIndex);

    hg::PZInteger getDefaultDelay() const;
    hg::PZInteger adjustDelayForLag(hg::PZInteger aDelay) const;

    //! Sets the default delay (state buffering length) for all current and future
    //! registered synchronized objects.
    void setDefaultDelay(hg::PZInteger aNewDefaultDelaySteps);

    //! Argument must be an even number!
    void setPacemakerPulsePeriod(hg::PZInteger aPeriod);

    //! Checks the alternating updates flag:
    //! - true:  objects with alternating updates have synced in this cycle.
    //! - false: objects with alternating updates have NOT synced in this cycle.
    //! Note: this is meant to be called during _eventFinalizeFrame(), otherwise
    //! the returned value may not be reliable.
    bool getAlternatingUpdatesFlag() const;

    ///////////////////////////////////////////////////////////////////////////
    // HELPERS & ACCESSORS                                                   //
    ///////////////////////////////////////////////////////////////////////////

    void deactivateObject(SyncId aObjectId, hg::PZInteger aDelayInSteps);

private:
    std::unordered_map<SyncId, SynchronizedObjectBase*> _mappings;
    std::unordered_set<const SynchronizedObjectBase*> _newlyCreatedObjects;
    std::unordered_set<const SynchronizedObjectBase*> _alreadyUpdatedObjects;
    std::unordered_set<const SynchronizedObjectBase*> _alreadyDestroyedObjects;

    hg::NotNull<hg::RN_NodeInterface*> _node;
    SyncControlDelegate _syncControlDelegate;

    SyncId _syncIdCounter = 2;
    hg::PZInteger _defaultDelay;

    hg::PZInteger _pacemakerPulsePeriod = 30; // 30 => 60 frames
    hg::PZInteger _pacemakerPulseCountdown = _pacemakerPulsePeriod;
    bool _alternatingUpdateFlag = true;

    static void Align(const SynchronizedObjectBase* aObject,
                      const SyncControlDelegate& aSyncCtrl,
                      hg::RN_NodeInterface& aLocalNode);
};

} // namespace detail

} // namespace spempe
} // namespace jbatnozic

#endif // !SPEMPE_GAME_OBJECT_FRAMEWORK_SYNCHRONIZED_OBJECT_REGISTRY_HPP

