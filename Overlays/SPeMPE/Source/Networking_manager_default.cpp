
#include <SPeMPE/Managers/Networking_manager_default.hpp>

#include <Hobgoblin/Logging.hpp>

#include <algorithm>
#include <cassert>
#include <type_traits>

namespace jbatnozic {
namespace spempe {
namespace {
constexpr const char* LOG_ID = "jbatnozic::spempe::DefaultNetworkingManager";
} // namespace

DefaultNetworkingManager::DefaultNetworkingManager(hg::QAO_RuntimeRef aRuntimeRef,
                                                   int aExecutionPriority,
                                                   hg::PZInteger aStateBufferingLength)
    : NonstateObject{aRuntimeRef, SPEMPE_TYPEID_SELF, aExecutionPriority, "::jbatnozic::spempe::DefaultNetworkingManager"}
    , _node{hg::RN_ServerFactory::createDummyServer()}
    , _syncObjReg{getNode(), aStateBufferingLength}
{
}

///////////////////////////////////////////////////////////////////////////
// CONFIGURATION                                                         //
///////////////////////////////////////////////////////////////////////////

void DefaultNetworkingManager::setToMode(Mode aMode) {
    if (_mode == aMode) {
        return;
    }

    _mode = aMode;

    switch (_mode) {
    case Mode::Uninitialized:
        _localClientIndex = CLIENT_INDEX_UNKNOWN;
        _node = hg::RN_ServerFactory::createDummyServer();
        _node->setUserData(&ctx());
        _syncObjReg.setNode(*_node);
        break;

    case Mode::Server:
        SPEMPE_VERIFY_GAME_CONTEXT_FLAGS(ctx(), privileged==true, networking==true);
        _localClientIndex = CLIENT_INDEX_LOCAL;
        _node = hg::RN_ServerFactory::createServer(hg::RN_Protocol::UDP, "pass"); // TODO Parametrize !!!!!!!!!!!
        _node->setUserData(&ctx());
        _syncObjReg.setNode(*_node);
        break;

    case Mode::Client:
        SPEMPE_VERIFY_GAME_CONTEXT_FLAGS(ctx(), privileged==false, networking==true);
        _localClientIndex = CLIENT_INDEX_UNKNOWN;
        _node = hg::RN_ClientFactory::createClient(hg::RN_Protocol::UDP, "pass");
        _node->setUserData(&ctx());
        _syncObjReg.setNode(*_node);
        break;

    default: {}
    }

    // TODO !!!!!!!!!!!!!!!!!
}

NetworkingManagerInterface::Mode DefaultNetworkingManager::getMode() const {
    return _mode;
}

bool DefaultNetworkingManager::isUninitialized() const {
    return _mode == Mode::Uninitialized;
}

bool DefaultNetworkingManager::isServer() const {
    return _mode == Mode::Server;
}

bool DefaultNetworkingManager::isClient() const {
    return _mode == Mode::Client;
}

///////////////////////////////////////////////////////////////////////////
// NODE ACCESS                                                           //
///////////////////////////////////////////////////////////////////////////

NetworkingManagerInterface::NodeType& DefaultNetworkingManager::getNode() const {
    return *_node;
}

NetworkingManagerInterface::ServerType& DefaultNetworkingManager::getServer() const {
    assert(isServer());
    return static_cast<ServerType&>(getNode());
}

NetworkingManagerInterface::ClientType& DefaultNetworkingManager::getClient() const {
    assert(isClient());
    return static_cast<ClientType&>(getNode());
}

///////////////////////////////////////////////////////////////////////////
// LISTENER MANAGEMENT                                                   //
///////////////////////////////////////////////////////////////////////////

void DefaultNetworkingManager::addEventListener(NetworkingEventListener& aListener) {
    for (const auto listener : _eventListeners) {
        if (listener == &aListener) {
            return;
        }
    }
    _eventListeners.push_back(&aListener);
}

void DefaultNetworkingManager::removeEventListener(NetworkingEventListener& aListener) {
    _eventListeners.erase(
        std::remove_if(_eventListeners.begin(), _eventListeners.end(),
                       [&aListener](const NetworkingEventListener* aCurr) {
                           return aCurr == &aListener;
                       }), _eventListeners.end());
}

///////////////////////////////////////////////////////////////////////////
// SYNCHRONIZATION                                                       //
///////////////////////////////////////////////////////////////////////////

RegistryId DefaultNetworkingManager::getRegistryId() {
    return {
        reinterpret_cast< decltype(std::declval<RegistryId>().address) >(&_syncObjReg)
    };
}

hg::PZInteger DefaultNetworkingManager::getStateBufferingLength() const {
    return _syncObjReg.getDefaultDelay();
}

void DefaultNetworkingManager::setStateBufferingLength(hg::PZInteger aNewStateBufferingLength) {
    _syncObjReg.setDefaultDelay(aNewStateBufferingLength);
}

void DefaultNetworkingManager::setPacemakerPulsePeriod(hg::PZInteger aPeriod) {
    _syncObjReg.setPacemakerPulsePeriod(aPeriod);
}

///////////////////////////////////////////////////////////////////////////
// MISC.                                                                 //
///////////////////////////////////////////////////////////////////////////

int DefaultNetworkingManager::getLocalClientIndex() const {
    return _localClientIndex;
}

///////////////////////////////////////////////////////////////////////////
// PROTECTED & PRIVATE METHODS                                           //
///////////////////////////////////////////////////////////////////////////

void DefaultNetworkingManager::_eventPreUpdate() {
    _node->update(hg::RN_UpdateMode::Receive);
    _handleEvents();
}

void DefaultNetworkingManager::_eventPostUpdate() {
    // Update all Synchronized objects
    if (_node->isServer()) {
        _syncObjReg.syncStateUpdates();
    }

    _node->update(hg::RN_UpdateMode::Send);
    _handleEvents();
}

void DefaultNetworkingManager::_handleEvents() {
    using hg::RN_Event;

    RN_Event event;

    while (_node->pollEvent(event)) {
        event.strictVisit(
            [](const RN_Event::BadPassphrase& ev) {
                HG_LOG_INFO(LOG_ID, "Received event: Bad passphrase.");
            },
            [](const RN_Event::ConnectAttemptFailed& ev) {
                HG_LOG_INFO(LOG_ID, "Received event: Connection attempt failed ({}).", /*ev.reason*/ "?");
            },
            [this](const RN_Event::Connected& ev) {
                if (_node->isServer()) {
                    HG_LOG_INFO(LOG_ID, "Received event: New client ({}) connected.", *ev.clientIndex);
                    _syncObjReg.syncCompleteState(*ev.clientIndex);
                }
                else {
                    HG_LOG_INFO(LOG_ID, "Received event: Connected to server.");
                    _localClientIndex = getClient().getClientIndex();
                }
            },
            [this](const RN_Event::Disconnected& ev) {
                HG_LOG_INFO(LOG_ID, "Received event: Disconnect ({}).", ev.message);
                if (!_node->isServer()) {
                    _localClientIndex = CLIENT_INDEX_UNKNOWN;
                }
            }
        );

        for (auto& listener : _eventListeners) {
            listener->onNetworkingEvent(event);
        }
    }
}

} // namespace spempe
} // namespace jbatnozic