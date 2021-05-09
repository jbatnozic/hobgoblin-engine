
#include <SPeMPE/Managers/Networking_manager_one.hpp>

#include <algorithm>
#include <cassert>

#include <iostream> 

namespace jbatnozic {
namespace spempe {

NetworkingManagerOne::NetworkingManagerOne(hg::QAO_RuntimeRef aRuntimeRef,
                                           int aExecutionPriority)
    : NonstateObject{aRuntimeRef, SPEMPE_TYPEID_SELF, aExecutionPriority, "spempe::NetworkingManagerOne"}
    , _node{hg::RN_ServerFactory::createDummyServer()}
    , _syncObjReg{getNode()}
{
}

///////////////////////////////////////////////////////////////////////////
// CONFIGURATION                                                         //
///////////////////////////////////////////////////////////////////////////

void NetworkingManagerOne::setToMode(Mode aMode) {
    if (_mode == aMode) {
        return;
    }

    _mode = aMode;

    switch (_mode) {
    case Mode::Uninitialized:
        _localPlayerIndex = PLAYER_INDEX_NONE;
        break;

    case Mode::Server:
        _localPlayerIndex = PLAYER_INDEX_LOCAL_PLAYER;
        break;

    case Mode::Client:
        _localPlayerIndex = PLAYER_INDEX_UNKNOWN;
        break;

    default: {}
    }

    // TODO !!!!!!!!!!!!!!!!!
}

NetworkingManagerInterface::Mode NetworkingManagerOne::getMode() const {
    return _mode;
}

bool NetworkingManagerOne::isUninitialized() const {
    return _mode == Mode::Uninitialized;
}

bool NetworkingManagerOne::isServer() const {
    return _mode == Mode::Server;
}

bool NetworkingManagerOne::isClient() const {
    return _mode == Mode::Client;
}

///////////////////////////////////////////////////////////////////////////
// NODE ACCESS                                                           //
///////////////////////////////////////////////////////////////////////////

NetworkingManagerInterface::NodeType& NetworkingManagerOne::getNode() const {
    return *_node;
}

NetworkingManagerInterface::ServerType& NetworkingManagerOne::getServer() const {
    assert(isServer());
    return static_cast<ServerType&>(getNode());
}

NetworkingManagerInterface::ClientType& NetworkingManagerOne::getClient() const {
    assert(isClient());
    return static_cast<ClientType&>(getNode());
}

///////////////////////////////////////////////////////////////////////////
// LISTENER MANAGEMENT                                                   //
///////////////////////////////////////////////////////////////////////////

void NetworkingManagerOne::addEventListener(NetworkingEventListener& aListener) {
    for (const auto listener : _eventListeners) {
        if (listener == &aListener) {
            return;
        }
    }
    _eventListeners.push_back(&aListener);
}

void NetworkingManagerOne::removeEventListener(NetworkingEventListener& aListener) {
    _eventListeners.erase(
        std::remove_if(_eventListeners.begin(), _eventListeners.end(),
                       [&aListener](const NetworkingEventListener* aCurr) {
                           return aCurr == &aListener;
                       }), _eventListeners.end());
}

///////////////////////////////////////////////////////////////////////////
// SYNCHRONIZATION                                                       //
///////////////////////////////////////////////////////////////////////////

SynchronizedObjectRegistry& NetworkingManagerOne::getSyncObjReg() {
    return _syncObjReg;
}

///////////////////////////////////////////////////////////////////////////
// MISC.                                                                 //
///////////////////////////////////////////////////////////////////////////

int NetworkingManagerOne::getLocalPlayerIndex() {
    return _localPlayerIndex;
}

///////////////////////////////////////////////////////////////////////////
// PROTECTED & PRIVATE METHODS                                           //
///////////////////////////////////////////////////////////////////////////

void NetworkingManagerOne::eventPreUpdate() {
    _node->update(hg::RN_UpdateMode::Receive);
    _handleEvents();
}

void NetworkingManagerOne::eventPostUpdate() {
    // Update all Synchronized objects
    if (_node->isServer()) {
        _syncObjReg.syncStateUpdates();
    }

    _node->update(hg::RN_UpdateMode::Send);
    _handleEvents();
}

void NetworkingManagerOne::_handleEvents() {
    // TODO Temporary couts (move to logger)

    using hg::RN_Event;
    RN_Event event;
    while (_node->pollEvent(event)) {
        event.visit(
            [](const RN_Event::BadPassphrase& ev) {
                std::cout << "Bad passphrase\n";
            },
            [](const RN_Event::ConnectAttemptFailed& ev) {
                std::cout << "Connection attempt failed\n";
            },
            [this](const RN_Event::Connected& ev) {
                if (_node->isServer()) {
                    std::cout << "New client connected\n";
                    _syncObjReg.syncCompleteState(*ev.clientIndex);
                }
                else {
                    std::cout << "Connected to server\n";
                    _localPlayerIndex = (getClient().getClientIndex() + 1);
                }
            },
            [](const RN_Event::Disconnected& ev) {
                std::cout << "Disconnected (message: " << ev.message << ")\n";
            }
            );

        for (auto& listener : _eventListeners) {
            listener->onNetworkingEvent(event);
        }
    }
}

} // namespace spempe
} // namespace jbatnozic