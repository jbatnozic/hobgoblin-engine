#ifndef SPEMPE_NETWORKING_MANAGER_HPP
#define SPEMPE_NETWORKING_MANAGER_HPP

#include <Hobgoblin/RigelNet.hpp>
#include <Hobgoblin/RigelNet_Macros.hpp>
#include <SPeMPE/Include/Game_object_framework.hpp>

#include <list>
#include <variant>

namespace spempe {

class NetworkingManager : public NonstateObject {
public:
    using ServerType = hg::RN_UdpServer;
    using ClientType = hg::RN_UdpClient;

    NetworkingManager(hg::QAO_RuntimeRef runtimeRef);

    void initializeAsServer();
    void initializeAsClient();

    bool isServer() const noexcept;
    bool isClient() const noexcept;

    hg::RN_Node& getNode();
    ServerType& getServer();
    ClientType& getClient();

    class EventListener {
    public:
        virtual void onNetworkingEvent(const hg::RN_Event& ev) = 0;
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

    std::variant<hg::RN_FakeNode, ServerType, ClientType> _node;
    State _state = State::NotInitialized;

    std::list<EventListener*> _eventListeners;

    void handleEvents();
};

} // namespace spempe

#endif // !SPEMPE_NETWORKING_MANAGER_HPP

