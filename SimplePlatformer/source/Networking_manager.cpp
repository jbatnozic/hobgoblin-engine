
#include <iostream>

#include "Networking_manager.hpp"

NetworkingManager::NetworkingManager(bool isServer)
    : GOF_StateObject{0, 0, "NetworkingManager"}
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
            [](const RN_Event::Connected& ev) {
                std::cout << "Connected\n";
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