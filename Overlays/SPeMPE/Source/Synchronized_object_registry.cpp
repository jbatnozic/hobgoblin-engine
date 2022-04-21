
#include <SPeMPE/GameObjectFramework/Game_object_bases.hpp>
#include <SPeMPE/GameObjectFramework/Synchronized_object_registry.hpp>
#include <SPeMPE/Other/Sync_parameters.hpp>

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

RN_DEFINE_RPC(USPEMPE_DeactivateObject, RN_ARGS(SyncId, aSyncId)) {
    RN_NODE_IN_HANDLER().callIfClient(
        [=](hg::RN_ClientInterface& client) {
            SyncParameters sp{client};
            auto  regId      = sp.netwMgr.getRegistryId();
            auto& syncObjReg = *reinterpret_cast<detail::SynchronizedObjectRegistry*>(regId.address);

            syncObjReg.deactivateObject(aSyncId, sp.pessimisticLatencyInSteps);
        });

    RN_NODE_IN_HANDLER().callIfServer(
        [](hg::RN_ServerInterface& server) {
            throw hg::RN_IllegalMessage("Server received a sync message");
        });
}
} // namespace

SyncDetails::SyncDetails(detail::SynchronizedObjectRegistry& aRegistry)
    : _registry{&aRegistry}
{
}

hg::RN_NodeInterface& SyncDetails::getNode() const {
    return _registry->getNode();
}

const std::vector<hg::PZInteger>& SyncDetails::getRecepients() const {
    return _recepients;
}

void SyncDetails::filterSyncs(const SyncDetails::FilterPrecidateFunc& aPredicate) {
    constexpr static bool REMOVE_IF_DO_REMOVE   = true;
    constexpr static bool REMOVE_IF_DONT_REMOVE = false;

    auto& reg = *_registry;
    const SyncId forObject = _forObject;

    auto iter = std::remove_if(_recepients.begin(), _recepients.end(),
                               [&](hg::PZInteger aRecepient) -> bool {
                                   switch (aPredicate(aRecepient)) {
                                   case FilterResult::FullSync:
                                       return REMOVE_IF_DONT_REMOVE;

                                   case FilterResult::Skip:
                                       return REMOVE_IF_DO_REMOVE;

                                   case FilterResult::Deactivate:
                                       if (reg.isObjectDeactivatedForClient(forObject, aRecepient)) {
                                           break;
                                       }
                                       reg.setObjectDeactivatedFlagForClient(forObject, aRecepient, true);
                                       Compose_USPEMPE_DeactivateObject(reg.getNode(), aRecepient, forObject);
                                       return REMOVE_IF_DO_REMOVE;

                                   default:
                                       assert(false && "Unreachable");
                                       break;
                                   }
                               });

    _recepients.erase(iter, _recepients.end());
}

namespace detail {

SynchronizedObjectRegistry::SynchronizedObjectRegistry(hg::RN_NodeInterface& node,
                                                       hg::PZInteger defaultDelay)
    : _node{&node}
    , _syncDetails{*this}
    , _defaultDelay{defaultDelay}
{
}

void SynchronizedObjectRegistry::setNode(hg::RN_NodeInterface& node) {
    _node = &node;
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
    GetIndicesForComposingToEveryone(*_node, _syncDetails._recepients);

    // Sync creations:
    for (auto* object : _newlyCreatedObjects) {
        object->_syncCreateImpl(_syncDetails);
    }
    _newlyCreatedObjects.clear();

    // Sync updates:
    if (!_sinclaireDoSync && _sinclaireInversionCountdown > 0) {
        _sinclaireInversionCountdown -= 1;
    }

    for (auto& pair : _mappings) {
        SynchronizedObjectBase* object = pair.second;

        auto iter = _alreadyUpdatedObjects.find(object);
        if (iter != _alreadyUpdatedObjects.end()) {
            // Not needed to remove elements one by one; we'll 
            // just clear the whole container at the end.
            // _alreadyUpdatedObjects.erase(iter);
        }
        else {
            if (object->_sinclaireState == SPEMPE_SINCLAIRE_DISABLED 
                || _sinclaireDoSync 
                || _sinclaireInversionCountdown == 0) {
                SyncDetails syncDetailsCopy = _syncDetails;
                syncDetailsCopy._forObject = pair.first;
                syncDetailsCopy._alignFrame = (_sinclaireInversionCountdown == 0);

                object->_syncUpdateImpl(syncDetailsCopy);
                for (auto rec : syncDetailsCopy._recepients) {
                    setObjectDeactivatedFlagForClient(pair.first, rec, false);
                }
            }
        }
    }
    _alreadyUpdatedObjects.clear();

    _sinclaireDoSync = !_sinclaireDoSync;

    if (_sinclaireInversionCountdown == 0) {
        _sinclaireInversionCountdown = 15;
    }

    // Sync destroys - not needed (dealt with in destructors)
}

void SynchronizedObjectRegistry::syncCompleteState(hg::PZInteger clientIndex) {
    _syncDetails._recepients.resize(1);
    _syncDetails._recepients[0] = clientIndex;

    for (auto& mapping : _mappings) {
        auto* object = mapping.second;
        object->_syncCreateImpl(_syncDetails);
        object->_syncUpdateImpl(_syncDetails); // Maybe this is not needed (double updates) ??????????????????????????? TODO
        for (auto rec : _syncDetails._recepients) {
            setObjectDeactivatedFlagForClient(mapping.first, rec, false);
        }
    }
}

hg::PZInteger SynchronizedObjectRegistry::getDefaultDelay() const {
    return _defaultDelay;
}

void SynchronizedObjectRegistry::setDefaultDelay(hg::PZInteger aNewDefaultDelaySteps) {
    _defaultDelay = aNewDefaultDelaySteps;
    for (auto& mapping : _mappings) {
        auto* object = mapping.second;
        object->_setStateSchedulerDefaultDelay(aNewDefaultDelaySteps);
    }
}

void SynchronizedObjectRegistry::syncObjectCreate(const SynchronizedObjectBase* object) {
    assert(object);
    GetIndicesForComposingToEveryone(*_node, _syncDetails._recepients);
    object->_syncCreateImpl(_syncDetails);

    // TODO: Fail if object is not in _newlyCreatedObjects

    _newlyCreatedObjects.erase(object);
}

void SynchronizedObjectRegistry::syncObjectUpdate(const SynchronizedObjectBase* object) {
    assert(object);
    GetIndicesForComposingToEveryone(*_node, _syncDetails._recepients);

    // Synchronized object sync Update called manualy, before the registry got to
    // sync its Create. We need to fix this because the order of these is important!
    {
        auto iter = _newlyCreatedObjects.find(const_cast<SynchronizedObjectBase*>(object));
        if (iter != _newlyCreatedObjects.end()) {
            object->_syncCreateImpl(_syncDetails);
            _newlyCreatedObjects.erase(iter);
        }
    }

    object->_syncUpdateImpl(_syncDetails);
    for (auto rec : _syncDetails._recepients) {
        setObjectDeactivatedFlagForClient(object->getSyncId(), rec, false);
    }

    _alreadyUpdatedObjects.insert(object);
}

void SynchronizedObjectRegistry::syncObjectDestroy(const SynchronizedObjectBase* object) {
    assert(object);
    GetIndicesForComposingToEveryone(*_node, _syncDetails._recepients);

    // It could happen that a Synchronized object is destroyed before
    // its Update event - or even its Create event - were synced.
    {
        {
            auto iter = _newlyCreatedObjects.find(object);
            if (iter != _newlyCreatedObjects.end()) {
                object->_syncCreateImpl(_syncDetails);
                _newlyCreatedObjects.erase(iter);
            }
        }
        {
            auto iter = _alreadyUpdatedObjects.find(object);
            if (iter == _alreadyUpdatedObjects.end()) {
                object->_syncUpdateImpl(_syncDetails);
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

    object->_syncDestroyImpl(_syncDetails);

    _alreadyDestroyedObjects.insert(object);
}

void SynchronizedObjectRegistry::deactivateObject(SyncId aObjectId, hg::PZInteger aDelayInSteps) {
    auto& object = _mappings.at(aObjectId);
    object->_deactivateSelfIn(aDelayInSteps);
}

bool SynchronizedObjectRegistry::isObjectDeactivatedForClient(SyncId aObjectId, hg::PZInteger aForClient) {
    auto& object = _mappings.at(aObjectId);
    return object->_remoteStatuses.getBit(aForClient);
}

void SynchronizedObjectRegistry::setObjectDeactivatedFlagForClient(SyncId aObjectId,
                                                                   hg::PZInteger aForClient,
                                                                   bool aFlag) {
    auto& object = _mappings.at(aObjectId);
    if (aFlag) {
        object->_remoteStatuses.setBit(aForClient);
    }
    else {
        object->_remoteStatuses.clearBit(aForClient);
    }
}

} // namespace detail

} // namespace spempe
} // namespace jbatnozic