
#include <SPeMPE/GameContext/Game_context.hpp>
#include <SPeMPE/GameObjectFramework/Game_object_bases.hpp>
#include <SPeMPE/GameObjectFramework/Synchronized_object_registry.hpp>
#include <SPeMPE/Managers/Networking_manager_interface.hpp>
#include <SPeMPE/Utility/Rpc_receiver_context_template.hpp>

#include <Hobgoblin/HGExcept.hpp>
#include <Hobgoblin/Logging.hpp>
#include <Hobgoblin/RigelNet.hpp>
#include <Hobgoblin/RigelNet_macros.hpp>

#include <cassert>
#include <sstream>

namespace jbatnozic {
namespace spempe {

namespace {
constexpr auto LOG_ID = "SPeMPE";

RN_DEFINE_RPC(USPEMPE_DeactivateObject, RN_ARGS(SyncId, aSyncId)) {
    RN_NODE_IN_HANDLER().callIfClient(
        [=](hg::RN_ClientInterface& aClient) {
            const auto rc    = SPEMPE_GET_RPC_RECEIVER_CONTEXT(aClient);
            auto  regId      = rc.netwMgr.getRegistryId();
            auto& syncObjReg = *reinterpret_cast<detail::SynchronizedObjectRegistry*>(regId.address);

            syncObjReg.deactivateObject(aSyncId, rc.pessimisticLatencyInSteps);
        });

    RN_NODE_IN_HANDLER().callIfServer(
        [](hg::RN_ServerInterface&) {
            throw hg::RN_IllegalMessage("Server received a sync message");
        });
}
} // namespace

///////////////////////////////////////////////////////////////////////////
// SYNC CONTROL DELEGATE                                                 //
///////////////////////////////////////////////////////////////////////////

class SyncControlDelegate::Impl {
public:
    void setLocalNode(hg::RN_NodeInterface& aLocalNode) {
        _node = &aLocalNode;
    }

    hg::RN_NodeInterface& getLocalNode() const {
        HG_ASSERT(_node != nullptr);
        return *_node;
    }

    void setSyncFlags(SyncFlags aFlags) {
        _flags = aFlags;
    }

    SyncFlags getSyncFlags() const {
        return _flags;
    }

    const std::vector<hg::PZInteger>& getAllRecepients() const {
        return _allRecepients;
    }

    const std::vector<hg::PZInteger>& getFilteredRecepients() const {
        if (_filteredRecepientsDirty) {
            _filteredRecepients.clear();
            for (std::size_t i = 0; i < _allRecepients.size(); i += 1) {
                const auto status = _filterStatuses[i];
                if (status == SyncFilterStatus::REGULAR_SYNC ||
                    status == SyncFilterStatus::__spempeimpl_FULL_STATE_SYNC) {
                    _filteredRecepients.push_back(_allRecepients[i]);
                }
            }
            _filteredRecepientsDirty = false;
        }
        return _filteredRecepients;
    }

    //! \returns numerical value of actually applied status.
    int applyFilterStatus(std::size_t aIndex, SyncFilterStatus aStatus) {
        const auto currentStatus = _filterStatuses[aIndex];
        if (currentStatus > aStatus ||
            currentStatus == SyncFilterStatus::__spempeimpl_FULL_STATE_SYNC)
        {
            // Due to status priorities, if the current status is higher than
            // the new one, we know right away that no change will be needed.
            // Additionally, FULL_STATE_SYNC, though having the lowest value,
            // is treated specially so that once set it cannot be changed.
            return static_cast<int>(currentStatus);
        }

        // If the object has an Autodiff state and there is no diff since
        // the last frame, usually we will skip a sync message. However,
        // if the object was previously skipped or deactivated for this
        // client, we have to send a full state sync instead, otherwise
        // this client could see it in an invalid state. An unfortunate
        // side effect (due to how other components work) is that this will
        // prompt sending full state syncs to all other clients as well,
        // but it should happen rarely enough for it to not be a problem.
        //
        // A similar problem happens when we're going a regular sync
        // with an Autodiff state but the change happened while the object
        // was deactivated or being skipped.
        if (aStatus == SyncFilterStatus::REGULAR_SYNC ||
            aStatus == SyncFilterStatus::__spempeimpl_SKIP_NO_DIFF) {
            HG_ASSERT(_forObject != nullptr);

            const auto client = _allRecepients[aIndex];
            if (_forObject->__spempeimpl_getSkipFlagForClient(client) ||
                _forObject->__spempeimpl_getDeactivationFlagForClient(client)) {
                aStatus = SyncFilterStatus::__spempeimpl_FULL_STATE_SYNC; 
            }
        }

        // |= because false->true is the only allowed transition
        // for the dirty flag here
        _filteredRecepientsDirty |= (_filterStatuses[aIndex] != aStatus);
        _filterStatuses[aIndex] = aStatus;
        return static_cast<int>(aStatus);
    }

    void resetAllStatuses(SyncFilterStatus aStatus = SyncFilterStatus::REGULAR_SYNC) {
        for (auto& status : _filterStatuses) {
            if (status != aStatus) {
                status = aStatus;
                _filteredRecepientsDirty = true;
            }
        }
    }

    const std::vector<SyncFilterStatus>& getAllStatuses() const {
        return _filterStatuses;
    }

    void targetObject(const SynchronizedObjectBase* aObject) {
        _forObject = aObject;
    }

    void targetRecepient(hg::PZInteger aRecepientIndex) {
        if (_allRecepients.size() != 1 || _allRecepients[0] != aRecepientIndex) {
            _allRecepients.resize(1);
            _allRecepients[0] = aRecepientIndex;
            _filteredRecepientsDirty = true;
        }
        _alignStatusVectorSizeIfNeeded();
    }

    void targetAllRecepientsConnectedToNode(const hg::RN_NodeInterface& aNode) {     
        if (!aNode.isServer()) {
            // CLIENT
            _allRecepients.clear();
            _filterStatuses.clear();
            _filteredRecepients.clear();
            _filteredRecepientsDirty = false;
        } else {
            // SERVER
            const auto& server = static_cast<const hg::RN_ServerInterface&>(aNode);
            const hg::PZInteger serverSize = server.getSize();
            if (hg::stopz(_allRecepients.size()) != serverSize) {
                _allRecepients.resize(hg::pztos(serverSize));
                _filteredRecepientsDirty = true;
            }
            std::size_t hand = 0;
            for (hg::PZInteger i = 0; i < serverSize; i += 1) {
                const auto& client = server.getClientConnector(i);

                if (client.getStatus() != hg::RN_ConnectorStatus::Connected) {
                    continue; // Ignore not connected clients
                }

                if (_allRecepients[hand] != i) {
                    _allRecepients[hand] = i;
                    _filteredRecepientsDirty = true;
                }
                hand += 1;
            }
            _allRecepients.resize(hand);
        }

        // CLEANUP
        _alignStatusVectorSizeIfNeeded();
    }

private:
    hg::RN_NodeInterface* _node = nullptr;
    const SynchronizedObjectBase* _forObject = nullptr;
    std::vector<hg::PZInteger> _allRecepients;
    std::vector<SyncFilterStatus> _filterStatuses;
    mutable std::vector<hg::PZInteger> _filteredRecepients;

    SyncFlags _flags = SyncFlags::NONE;
    mutable bool _filteredRecepientsDirty = false;

    void _alignStatusVectorSizeIfNeeded() {
        if (_filterStatuses.size() < _allRecepients.size()) {
            do {
                _filterStatuses.push_back(SyncFilterStatus::REGULAR_SYNC);
            } while (_filterStatuses.size() < _allRecepients.size());
        } else {
            _filterStatuses.resize(_allRecepients.size());
        }
    }
};

SyncControlDelegate::SyncControlDelegate()
    : _impl{std::make_unique<Impl>()}
{
}

SyncControlDelegate::~SyncControlDelegate() = default;

hg::RN_NodeInterface& SyncControlDelegate::getLocalNode() const {
    return _impl->getLocalNode();
}

SyncFlags SyncControlDelegate::getSyncFlags() const {
    return _impl->getSyncFlags();
}

const std::vector<hg::PZInteger>& SyncControlDelegate::getAllRecepients() const {
    return _impl->getAllRecepients();
}

const std::vector<hg::PZInteger>& SyncControlDelegate::getFilteredRecepients() const {
    return _impl->getFilteredRecepients();
}

int SyncControlDelegate::_applyFilterStatus(std::size_t aIndex, SyncFilterStatus aStatus) {
    return _impl->applyFilterStatus(aIndex, aStatus);
}

namespace detail {

///////////////////////////////////////////////////////////////////////////
// SYNCHRONIZED OBJECT REGISTRY                                          //
///////////////////////////////////////////////////////////////////////////

//! Use this macro to properly call a synchronized object's CREATE
//! implementation, ensuring that:
//! - The SyncControlDelegate is properly set up prior to its use;
//! - A DeactivateObject RPC is sent to appropriate clients after filtering;
//! - The Skip and Deactivation flags are properly set for the
//!   synchronized object after filtering.
//!
//! \note This macro is meant to be called only from within a method
//!       of SynchronizedObjectRegistry class.
#define CALL_SYNC_CREATE_IMPL(_object_ptr_, _sync_ctrl_ref_, _node_ref_) \
    do { \
        (_sync_ctrl_ref_)._impl->targetObject(_object_ptr_); \
        (_sync_ctrl_ref_)._impl->resetAllStatuses(); \
        (_object_ptr_)->__spempeimpl_syncCreateImpl(_sync_ctrl_ref_); \
        Align((_object_ptr_), (_sync_ctrl_ref_), (_node_ref_)); \
    } while (false)

//! Use this macro to properly call a synchronized object's UPDATE
//! implementation, ensuring that:
//! - The SyncControlDelegate is properly set up prior to its use;
//! - A DeactivateObject RPC is sent to appropriate clients after filtering;
//! - The Skip and Deactivation flags are properly set for the
//!   synchronized object after filtering.
//!
//! \note This macro is meant to be called only from within a method
//!       of SynchronizedObjectRegistry class.
#define CALL_SYNC_UPDATE_IMPL(_object_ptr_, _sync_ctrl_ref_, _node_ref_) \
    do { \
        (_sync_ctrl_ref_)._impl->targetObject(_object_ptr_); \
        (_sync_ctrl_ref_)._impl->resetAllStatuses(); \
        (_object_ptr_)->__spempeimpl_syncUpdateImpl(_sync_ctrl_ref_); \
        Align((_object_ptr_), (_sync_ctrl_ref_), (_node_ref_)); \
    } while (false)

//! Use this macro to properly call a synchronized object's DESTROY
//! implementation, ensuring that:
//! - The SyncControlDelegate is properly set up prior to its use;
//! - A DeactivateObject RPC is sent to appropriate clients after filtering;
//! - The Skip and Deactivation flags are properly set for the
//!   synchronized object after filtering.
//!
//! \note This macro is meant to be called only from within a method
//!       of SynchronizedObjectRegistry class.
#define CALL_SYNC_DESTROY_IMPL(_object_ptr_, _sync_ctrl_ref_, _node_ref_) \
    do { \
        (_sync_ctrl_ref_)._impl->targetObject(_object_ptr_); \
        (_sync_ctrl_ref_)._impl->resetAllStatuses(); \
        (_object_ptr_)->__spempeimpl_syncDestroyImpl(_sync_ctrl_ref_); \
        Align((_object_ptr_), (_sync_ctrl_ref_), (_node_ref_)); \
    } while (false)

SynchronizedObjectRegistry::SynchronizedObjectRegistry(hg::RN_NodeInterface& node,
                                                       hg::PZInteger defaultDelay)
    : _node{&node}
    , _defaultDelay{defaultDelay}
{
    _syncControlDelegate._impl->setLocalNode(node);
}

void SynchronizedObjectRegistry::setNode(hg::RN_NodeInterface& node) {
    _node = &node;
    _syncControlDelegate._impl->setLocalNode(node);
}

hg::RN_NodeInterface& SynchronizedObjectRegistry::getNode() const {
    return *_node;
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
            assert(false && "Unregistering object which did not sync its destruction.");
            HG_THROW_TRACED(hg::TracedLogicError, 0,
                            "Unregistering object which did not sync its destruction.");
        }
        else {
            _alreadyDestroyedObjects.erase(iter);
        }
    }
    _mappings.erase(object->getSyncId());
}

void SynchronizedObjectRegistry::destroyAllRegisteredObjects() {
    std::vector<SynchronizedObjectBase*> objectsToDelete;
    objectsToDelete.reserve(_mappings.size());
    for (const auto pair : _mappings) {
        objectsToDelete.push_back(pair.second);
    }

    auto& rt = _node->getUserDataOrThrow<GameContext>()->getQAORuntime();
    for (auto* object : objectsToDelete) {
        if (rt.ownsObject(object)) {
            rt.eraseObject(object);
        }
    }

    if (!_mappings.empty()) {
        std::stringstream ss;
        for (const auto pair : _mappings) {
            ss << pair.second->getName() << ", ";
        }
        HG_LOG_WARN(LOG_ID,
                    "{} registered objects remain after destroyAllRegisteredObjects() was called: {}",
                    _mappings.size(),
                    ss.str()
        );
    }
}

SynchronizedObjectBase* SynchronizedObjectRegistry::getMapping(SyncId syncId) const {
    if (const auto iter = _mappings.find(syncId); iter != _mappings.end()) {
        return iter->second;
    }
    return nullptr;
}

void SynchronizedObjectRegistry::syncStateUpdates() {
    _syncControlDelegate._impl->targetAllRecepientsConnectedToNode(*_node);
    _syncControlDelegate._impl->setSyncFlags(SyncFlags::NONE);

    // Sync creations:
    for (auto* object : _newlyCreatedObjects) {
        CALL_SYNC_CREATE_IMPL(object, _syncControlDelegate, *_node);
    }
    _newlyCreatedObjects.clear();

    // Sync updates:
    if (!_alternatingUpdateFlag && _pacemakerPulseCountdown > 0) {
        _pacemakerPulseCountdown -= 1;
    }

    _syncControlDelegate._impl->setSyncFlags(
        (_pacemakerPulseCountdown == 0) ? SyncFlags::PACEMAKER_PULSE 
                                        : SyncFlags::NONE
    );

    for (auto& pair : _mappings) {
        SynchronizedObjectBase* object = pair.second;

        auto iter = _alreadyUpdatedObjects.find(object);
        if (iter != _alreadyUpdatedObjects.end()) {
            // Not needed to remove elements one by one; we'll 
            // just clear the whole container at the end.
            // _alreadyUpdatedObjects.erase(iter);
        }
        else {
            if (_alternatingUpdateFlag
                || _pacemakerPulseCountdown == 0
                || !object->isUsingAlternatingUpdates())
            {
                CALL_SYNC_UPDATE_IMPL(object, _syncControlDelegate, *_node);
            }
        }
    }
    _alreadyUpdatedObjects.clear();

    _alternatingUpdateFlag = !_alternatingUpdateFlag;

    if (_pacemakerPulseCountdown == 0) {
        _pacemakerPulseCountdown = _pacemakerPulsePeriod;
    }

    // Sync destroys - not needed (dealt with in destructors)
}

void SynchronizedObjectRegistry::syncCompleteState(hg::PZInteger clientIndex) {
    _syncControlDelegate._impl->setSyncFlags(SyncFlags::FULL_STATE);
    _syncControlDelegate._impl->targetRecepient(clientIndex);

    for (auto& mapping : _mappings) {
        auto* object = mapping.second;

        CALL_SYNC_CREATE_IMPL(object, _syncControlDelegate, *_node);
        CALL_SYNC_UPDATE_IMPL(object, _syncControlDelegate, *_node);
    }
}

hg::PZInteger SynchronizedObjectRegistry::getDefaultDelay() const {
    return _defaultDelay;
}

hg::PZInteger SynchronizedObjectRegistry::adjustDelayForLag(hg::PZInteger aDelay) const {
    // TODO: If delay > buffering length, compensate by up to a few frames (otherwise the
    //       state scheduler is basically useless)
    return aDelay;
}

void SynchronizedObjectRegistry::setDefaultDelay(hg::PZInteger aNewDefaultDelaySteps) {
    _defaultDelay = aNewDefaultDelaySteps;
    for (auto& mapping : _mappings) {
        auto* object = mapping.second;
        object->__spempeimpl_setStateSchedulerDefaultDelay(aNewDefaultDelaySteps);
    }
}

void SynchronizedObjectRegistry::setPacemakerPulsePeriod(hg::PZInteger aPeriod) {
    if ((aPeriod % 2 == 1) || (aPeriod < 2)) {
        HG_THROW_TRACED(hg::InvalidArgumentError, 0,
                        "Pacemaker pulse period must be an even number and at least 2!");
    }
    _pacemakerPulsePeriod = (aPeriod / 2);
}

bool SynchronizedObjectRegistry::getAlternatingUpdatesFlag() const {
    // This method is only for use in _eventFinalizeFrame; however by this point
    // the flag has been read and flipped, so we flip it again before returning.
    return !_alternatingUpdateFlag;
}

void SynchronizedObjectRegistry::syncObjectCreate(const SynchronizedObjectBase* object) {
    HG_ASSERT(object != nullptr);

    _syncControlDelegate._impl->targetAllRecepientsConnectedToNode(*_node);
    _syncControlDelegate._impl->setSyncFlags(SyncFlags::NONE);

    CALL_SYNC_CREATE_IMPL(object, _syncControlDelegate, *_node);

    // TODO: Fail if object is not in _newlyCreatedObjects

    _newlyCreatedObjects.erase(object);
}

void SynchronizedObjectRegistry::syncObjectUpdate(const SynchronizedObjectBase* object) {
    HG_ASSERT(object != nullptr);

    _syncControlDelegate._impl->targetAllRecepientsConnectedToNode(*_node);
    _syncControlDelegate._impl->setSyncFlags(SyncFlags::NONE);

    // Synchronized object sync Update called manualy, before the registry got to
    // sync its Create. We need to fix this because the order of these is important!
    {
        auto iter = _newlyCreatedObjects.find(const_cast<SynchronizedObjectBase*>(object));
        if (iter != _newlyCreatedObjects.end()) {
            CALL_SYNC_CREATE_IMPL(object, _syncControlDelegate, *_node);
            _newlyCreatedObjects.erase(iter);
        }
    }

    CALL_SYNC_UPDATE_IMPL(object, _syncControlDelegate, *_node);

    _alreadyUpdatedObjects.insert(object);
}

void SynchronizedObjectRegistry::syncObjectDestroy(const SynchronizedObjectBase* object) {
    HG_ASSERT(object != nullptr);
    _syncControlDelegate._impl->targetAllRecepientsConnectedToNode(*_node);
    _syncControlDelegate._impl->setSyncFlags(SyncFlags::NONE);

    // It could happen that a Synchronized object is destroyed before
    // its Update event - or even its Create event - were synced.
    {
        {
            auto iter = _newlyCreatedObjects.find(object);
            if (iter != _newlyCreatedObjects.end()) {
                CALL_SYNC_CREATE_IMPL(object, _syncControlDelegate, *_node);
                _newlyCreatedObjects.erase(iter);
            }
        }
        {
            auto iter = _alreadyUpdatedObjects.find(object);
            if (iter == _alreadyUpdatedObjects.end()) {
                CALL_SYNC_UPDATE_IMPL(object, _syncControlDelegate, *_node);
            #ifndef NDEBUG // TODO: #ifdef HOBGOBLIN_DEBUG
                // This isn't really needed as we don't expect to sync an object's
                // destruction from anywhere other than its destructor, where it will
                // get unregistered so this won't matter anyway. It's left here in
                // Debug mode for optimistic correctness reasons.
                _alreadyUpdatedObjects.insert(object);
            #endif
            }
        }
    }

    CALL_SYNC_DESTROY_IMPL(object, _syncControlDelegate, *_node);

    _alreadyDestroyedObjects.insert(object);
}

void SynchronizedObjectRegistry::deactivateObject(SyncId aObjectId, hg::PZInteger aDelayInSteps) {
    auto& object = _mappings.at(aObjectId);
    object->__spempeimpl_deactivateSelfIn(aDelayInSteps);
}

void SynchronizedObjectRegistry::Align(const SynchronizedObjectBase* aObject,
                                       const SyncControlDelegate& aSyncCtrl,
                                       hg::RN_NodeInterface& aLocalNode) {
    const auto& recepients = aSyncCtrl._impl->getAllRecepients();
    const auto& statuses = aSyncCtrl._impl->getAllStatuses();

    HG_ASSERT(recepients.size() == statuses.size());

    for (std::size_t i = 0; i < recepients.size(); i += 1) {
        const hg::PZInteger client = recepients[i];

        switch (auto status = statuses[i]) {
        case SyncFilterStatus::__spempeimpl_FULL_STATE_SYNC:
        case SyncFilterStatus::REGULAR_SYNC:
            aObject->__spempeimpl_setSkipFlagForClient(client, false);
            aObject->__spempeimpl_setDeactivationFlagForClient(client, false);
            break;

        case SyncFilterStatus::__spempeimpl_SKIP_NO_DIFF:
        case SyncFilterStatus::SKIP:
            aObject->__spempeimpl_setSkipFlagForClient(client, true);
            aObject->__spempeimpl_setDeactivationFlagForClient(client, false);
            break;

        case SyncFilterStatus::DEACTIVATE:
            if (!aObject->__spempeimpl_getDeactivationFlagForClient(client)) {
                Compose_USPEMPE_DeactivateObject(aLocalNode, client, aObject->getSyncId());
            }
            aObject->__spempeimpl_setSkipFlagForClient(client, false);
            aObject->__spempeimpl_setDeactivationFlagForClient(client, true);
            break;

        case SyncFilterStatus::__spempeimpl_UNDEFINED:
        default:
            HG_UNREACHABLE("Invalid value for SyncFilterStatus ({}).", (int)status);
        }
    }
}

} // namespace detail

} // namespace spempe
} // namespace jbatnozic