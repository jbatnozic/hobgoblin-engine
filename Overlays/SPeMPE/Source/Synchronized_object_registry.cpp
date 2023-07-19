
#include <SPeMPE/GameContext/Game_context.hpp>
#include <SPeMPE/GameObjectFramework/Game_object_bases.hpp>
#include <SPeMPE/GameObjectFramework/Synchronized_object_registry.hpp>

#include <Hobgoblin/Logging.hpp>

#include <cassert>
#include <sstream>

namespace jbatnozic {
namespace spempe {

namespace {
constexpr auto LOG_ID = "SynchronizedObjectRegistry";

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

namespace detail {

SynchronizedObjectRegistry::SynchronizedObjectRegistry(hg::QAO_Runtime& aQaoRuntime,
                                                       hg::RN_NodeInterface& node,
                                                       hg::PZInteger defaultDelay)
    : _qaoRuntime{&aQaoRuntime}
    , _node{&node}
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
    GetIndicesForComposingToEveryone(*_node, _syncDetails._recepients);

    // Sync creations:
    for (auto* object : _newlyCreatedObjects) {
        object->_syncCreateImpl(_syncDetails);
    }
    _newlyCreatedObjects.clear();

    // Sync updates:
    if (!_alternatingUpdateFlag && _pacemakerPulseCountdown > 0) {
        _pacemakerPulseCountdown -= 1;
    }

    _syncDetails._qaoStepOrdinal = _qaoRuntime->getCurrentStepOrdinal();
    _syncDetails._flags = SyncFlags::None;
    if (_pacemakerPulseCountdown == 0) {
        _syncDetails._flags |= SyncFlags::PacemakerPulse;
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
            if (_alternatingUpdateFlag
                || _pacemakerPulseCountdown == 0
                || !object->isUsingAlternatingUpdates())
            {
                SyncDetails syncDetailsCopy = _syncDetails;
                syncDetailsCopy._forObject = pair.first;

                object->_syncUpdateImpl(syncDetailsCopy);
                for (auto rec : syncDetailsCopy._recepients) {
                    setObjectDeactivatedFlagForClient(pair.first, rec, false);
                }
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
    _syncDetails._recepients.resize(1);
    _syncDetails._recepients[0] = clientIndex;
    _syncDetails._qaoStepOrdinal = _qaoRuntime->getCurrentStepOrdinal();
    _syncDetails._flags = SyncFlags::FullState;

    for (auto& mapping : _mappings) {
        auto* object = mapping.second;
        object->_syncCreateImpl(_syncDetails);
        object->_syncUpdateImpl(_syncDetails);
        for (auto rec : _syncDetails._recepients) {
            setObjectDeactivatedFlagForClient(mapping.first, rec, false);
        }
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
        object->_setStateSchedulerDefaultDelay(aNewDefaultDelaySteps);
    }
}

void SynchronizedObjectRegistry::setPacemakerPulsePeriod(hg::PZInteger aPeriod) {
    if ((aPeriod % 2 == 1) || (aPeriod < 2)) {
        throw hg::TracedLogicError{
            "SynchronizedObjectRegistry - Pacemaker pulse period must be an even number and at least 2!"
        };
    }
    _pacemakerPulsePeriod = (aPeriod / 2);
}

bool SynchronizedObjectRegistry::getAlternatingUpdatesFlag() const {
    // Inverted because... TODO
    return !_alternatingUpdateFlag;
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

bool SynchronizedObjectRegistry::isObjectDeactivatedForClient(SyncId aObjectId, hg::PZInteger aForClient) const {
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