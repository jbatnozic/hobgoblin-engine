
#include <iostream>

#include "Global_program_state.hpp"
#include "Networking_manager.hpp"

NetworkingManager::NetworkingManager(QAO_RuntimeRef runtimeRef, bool isServer)
    : GOF_StateObject{runtimeRef, TYPEID_SELF, 50, "NetworkingManager"}
    , _isServer{isServer}
{
    auto retransmitPredicate =
        [](hg::PZInteger cyclesSinceLastTransmit,
           std::chrono::microseconds timeSinceLastTransmit,
           std::chrono::microseconds currentLatency) 
    {
               return (timeSinceLastTransmit >= 2 * currentLatency) || cyclesSinceLastTransmit >= 3;
               //return 1; // Maximize user experience (super bandwidth-unfriendly)
    };

    if (isServer) {
        _node.emplace<ServerType>(4); // TODO refactor hardcoded 4 player limit
        getServer().setRetransmitPredicate(retransmitPredicate);
    }
    else {
        _node.emplace<ClientType>();
        getClient().setRetransmitPredicate(retransmitPredicate);
    }
}

bool NetworkingManager::isServer() const noexcept {
    return _isServer;
}

RN_Node& NetworkingManager::getNode() {
    if (_isServer) {
        return getServer();
    }
    else {
        return getClient();
    }
}

NetworkingManager::ServerType& NetworkingManager::getServer() {
    return std::get<ServerType>(_node);
}

NetworkingManager::ClientType& NetworkingManager::getClient() {
    return std::get<ClientType>(_node);
}

void NetworkingManager::eventPreUpdate()  {
    if (_isServer) {
        getServer().update(RN_UpdateMode::Receive);
    }
    else {
        getClient().update(RN_UpdateMode::Receive);
    }
    handleEvents();
}

void NetworkingManager::eventPostUpdate() {
    // Update all Synchronized objects
    if (RN_IsServer(getNode().getType())) {
        global().syncObjMgr.syncAll(); // TODO Maybe this shouldn't be a responsibility of this object?
    }

    if (_isServer) {
        getServer().update(RN_UpdateMode::Send);
    }
    else {
        getClient().update(RN_UpdateMode::Send);
    }
    handleEvents();
}

void NetworkingManager::addEventListener(EventListener* listener) {
    _eventListeners.push_back(listener);
}

void NetworkingManager::removeEventListener(EventListener* listener) {
    _eventListeners.remove_if(
        [=](EventListener* listener_) {
            return listener_ == listener;
        });
}

void NetworkingManager::handleEvents() {
    RN_Event event;
    while (getNode().pollEvent(event)) {
        event.visit(
            [](const RN_Event::BadPassphrase& ev) {
                std::cout << "Bad passphrase\n";
            },
            [](const RN_Event::ConnectAttemptFailed& ev) {
                std::cout << "Connection attempt failed\n";
            },
            [this](const RN_Event::Connected& ev) {
                if (RN_IsServer(getNode().getType())) {
                    std::cout << "New client connected\n";
                    global().syncObjMgr.syncAllToNewClient(*ev.clientIndex);
                }
                else {
                    std::cout << "Connected to server\n";
                    global().playerIndex = getClient().getClientIndex() + 1;
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