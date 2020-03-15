#ifndef NETWORKING_MANAGER_HPP
#define NETWORKING_MANAGER_HPP

#include <Hobgoblin/RigelNet.hpp>
#include <Hobgoblin/RigelNet_Macros.hpp>

#include <optional>

#include "Object_framework.hpp"

using namespace hg::rn;

class NetworkingManager : public QAO_Base {
public:
    using ServerType = RN_UdpServer;
    using ClientType = RN_UdpClient;

    NetworkingManager(bool isServer);

    bool isServer() const noexcept;
    RN_Node& getNode();
    ServerType& getServer();
    ClientType& getClient();

protected:
    void eventPreUpdate() override;
    // void eventUpdate() override;
    void eventPostUpdate() override;

private:
    std::variant<ServerType, ClientType> _node;
    bool _isServer;

    void handleEvents();
};

#endif // !NETWORKING_MANAGER_HPP
