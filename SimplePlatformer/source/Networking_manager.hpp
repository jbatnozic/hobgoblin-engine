#ifndef NETWORKING_MANAGER_HPP
#define NETWORKING_MANAGER_HPP

#include <Hobgoblin/RigelNet.hpp>
#include <Hobgoblin/RigelNet_Macros.hpp>

#include <list>
#include <variant>

#include "Game_object_framework.hpp"

using namespace hg::rn;

class NetworkingManager : public GOF_StateObject {
public:
    using ServerType = RN_UdpServer;
    using ClientType = RN_UdpClient;

    NetworkingManager(QAO_RuntimeRef runtimeRef);

    void initializeAsServer();
    void initializeAsClient();

    bool isServer() const noexcept;
    bool isClient() const noexcept;

    RN_Node& getNode();
    ServerType& getServer();
    ClientType& getClient();

    class EventListener {
    public:
        virtual void onNetworkingEvent(const RN_Event& ev) = 0;
    };

    void addEventListener(EventListener* listener);
    void removeEventListener(EventListener* listener);

protected:
    void eventPreUpdate() override;
    void eventPostUpdate() override;

private:
    enum class State {
        NotInitialized,
        Server,
        Client
    };

    std::variant<RN_FakeNode, ServerType, ClientType> _node;
    State _state = State::NotInitialized;

    std::list<EventListener*> _eventListeners;

    void handleEvents();
};

#endif // !NETWORKING_MANAGER_HPP
