
#include <gtest/gtest.h>

#define HOBGOBLIN_SHORT_NAMESPACE
#include <Hobgoblin/RigelNet.hpp>
#include <Hobgoblin/RigelNet_Macros.hpp>
using namespace hg::rn;

#include <chrono>
#include <iostream>
#include <memory>
#include <thread>

namespace {
constexpr hg::PZInteger CLIENT_COUNT = 1;
const std::string PASS = "beetlejuice";

struct EventCount {
    hg::PZInteger badPassphrase = 0;
    hg::PZInteger connectAttemptFailed = 0;
    hg::PZInteger connected = 0;
    hg::PZInteger disconnected = 0;
};
} // namespace

// Check that the handler compiles:
RN_DEFINE_HANDLER(TestHandler, RN_ARGS(int, a, std::string, s)) {
    RN_NODE_IN_HANDLER().callIfServer(
        [](RN_ServerInterface& /*server*/) {

        });

    RN_NODE_IN_HANDLER().callIfClient(
        [](RN_ClientInterface& /*client*/) {

        });
}

// Check that the compose function for TestHandler is generated:
void DummyFuncCallsComposeForTestHandler() {
    auto node = reinterpret_cast<RN_NodeInterface*>(0x12345678);
    Compose_TestHandler(*node, RN_COMPOSE_FOR_ALL, 1337, "tubular");
}

class RigelNetTest : public ::testing::Test {
public:
    RigelNetTest()
        : _server{RN_ServerFactory::createServer(RN_Protocol::UDP, PASS, CLIENT_COUNT)}
        , _client{RN_ClientFactory::createClient(RN_Protocol::UDP, PASS)}
    {
    }

protected:
    std::unique_ptr<RN_ServerInterface> _server;
    std::unique_ptr<RN_ClientInterface> _client;

    EventCount pollAndCountEvents(RN_NodeInterface& node) {
        EventCount cnt;

        RN_Event ev;
        while (node.pollEvent(ev)) {
            ev.visit(
                [&](const RN_Event::BadPassphrase& ev) {
                    cnt.badPassphrase += 1;
                },
                [&](const RN_Event::ConnectAttemptFailed& ev) {
                    cnt.connectAttemptFailed += 1;
                },
                [&](const RN_Event::Connected& ev) {
                    std::cout << "connect\n";
                    cnt.connected += 1;
                },
                [&](const RN_Event::Disconnected& ev) {
                    cnt.disconnected += 1;
                }
                );
        }

        return cnt;
    }
};

TEST_F(RigelNetTest, ClientConnectsAndDisconnects) {
    _server->start(0);
    _client->connect(0, sf::IpAddress::LocalHost, _server->getLocalPort());

    for (int i = 0; i < 20; i += 1) {
        _server->update(RN_UpdateMode::Receive);
        _client->update(RN_UpdateMode::Receive);

        std::this_thread::sleep_for(std::chrono::milliseconds{25});

        _server->update(RN_UpdateMode::Send);
        _client->update(RN_UpdateMode::Send);

        if (_server->getClientConnector(0).getStatus() == RN_ConnectorStatus::Connected &&
            _client->getServerConnector().getStatus() == RN_ConnectorStatus::Connected) {
            break;
        }
    }

    ASSERT_TRUE(_server->getClientConnector(0).getStatus() == RN_ConnectorStatus::Connected &&
                _client->getServerConnector().getStatus() == RN_ConnectorStatus::Connected);

    {
        auto cnt = pollAndCountEvents(*_server);
        ASSERT_EQ(cnt.connected, 1);
    }

    {
        auto cnt = pollAndCountEvents(*_client);
        ASSERT_EQ(cnt.connected, 1);
    }

    _client->disconnect(true);

    for (int i = 0; i < 20; i += 1) {
        _server->update(RN_UpdateMode::Receive);

        std::this_thread::sleep_for(std::chrono::milliseconds{25});

        _server->update(RN_UpdateMode::Send);

        if (_server->getClientConnector(0).getStatus() == RN_ConnectorStatus::Disconnected) {
            break;
        }
    }

    ASSERT_EQ(_server->getClientConnector(0).getStatus(), RN_ConnectorStatus::Disconnected);
    ASSERT_TRUE(_server->getClientConnector(0).getStatus() == RN_ConnectorStatus::Disconnected);

    {
        auto cnt = pollAndCountEvents(*_server);
        ASSERT_EQ(cnt.disconnected, 1);
    }
}