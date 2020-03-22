
#include <iostream>

#include "Global_program_state.hpp"
#include "Networking_manager.hpp"

NetworkingManager::NetworkingManager(QAO_Runtime* runtime, bool isServer)
    : GOF_StateObject{runtime, TYPEID_SELF, 50, "NetworkingManager"}
    , _isServer{isServer}
{
    if (isServer) {
        _node.emplace<ServerType>(1); // TODO refactor hardcoded 4 player limit
    }
    else {
        _node.emplace<ClientType>();
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

void NetworkingManager::handleEvents() {
    RN_Event event;
    while (getNode().pollEvent(event)) {
        event.visit(
            [](const RN_Event::BadPassphrase& ev) {
                std::cout << "Bad passphrase\n";
            },
            [](const RN_Event::AttemptTimedOut& ev) {
                std::cout << "Attempt timed out\n";
            },
            [this](const RN_Event::Connected& ev) {
                if (RN_IsServer(getNode().getType())) {
                    std::cout << "New client connected\n";
                    global().syncObjMgr.syncAllToNewClient(*ev.clientIndex);
                    QAO_PCreate<Player>(&global().qaoRuntime, global().syncObjMgr,
                                        200.f, 200.f, *ev.clientIndex + 1);
                }
                else {
                    std::cout << "Connected to server\n";
                    global().playerIndex = getClient().getClientIndex() + 1;
                }
            },
            [](const RN_Event::Disconnected& ev) {
                std::cout << "Disconnected\n";
            },
            [](const RN_Event::ConnectionTimedOut& ev) {
                std::cout << "Connection timed out\n";
            }
        );
    }
}