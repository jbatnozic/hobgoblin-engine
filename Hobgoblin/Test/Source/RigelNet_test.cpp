
#include <gtest/gtest.h>

#define HOBGOBLIN_SHORT_NAMESPACE
#include <Hobgoblin/RigelNet.hpp>
#include <Hobgoblin/RigelNet_Macros.hpp>
using namespace hg::rn;

#include <chrono>
#include <iostream>
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
}

class RigelNetTest : public ::testing::Test {
public:
    RigelNetTest()
        : _server{CLIENT_COUNT}
        , _client{}
    {
    }

protected:
    RN_UdpServer _server;
    RN_UdpClient _client;

    EventCount pollAndCountEvents(RN_Node& node) {
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
    _server.start(0, PASS);
    _client.connect(0, "localhost", _server.getLocalPort(), PASS);

    for (int i = 0; i < 20; i += 1) {
        _server.update(RN_UpdateMode::Receive);
        _client.update(RN_UpdateMode::Receive);

        std::this_thread::sleep_for(std::chrono::milliseconds{25});

        _server.update(RN_UpdateMode::Send);
        _client.update(RN_UpdateMode::Send);

        if (_server.getClient(0).getStatus() == RN_ConnectorStatus::Connected &&
            _client.getServer().getStatus() == RN_ConnectorStatus::Connected) {
            break;
        }
    }

    ASSERT_TRUE(_server.getClient(0).getStatus() == RN_ConnectorStatus::Connected &&
                _client.getServer().getStatus() == RN_ConnectorStatus::Connected);

    {
        auto cnt = pollAndCountEvents(_server);
        ASSERT_EQ(cnt.connected, 1);
    }

    {
        auto cnt = pollAndCountEvents(_client);
        ASSERT_EQ(cnt.connected, 1);
    }

    _client.disconnect(true);

    for (int i = 0; i < 20; i += 1) {
        _server.update(RN_UpdateMode::Receive);

        std::this_thread::sleep_for(std::chrono::milliseconds{25});

        _server.update(RN_UpdateMode::Send);

        if (_server.getClient(0).getStatus() == RN_ConnectorStatus::Disconnected) {
            break;
        }
    }

    ASSERT_EQ(_server.getClient(0).getStatus(), RN_ConnectorStatus::Disconnected);
    ASSERT_TRUE(_server.getClient(0).getStatus() == RN_ConnectorStatus::Disconnected);

    {
        auto cnt = pollAndCountEvents(_server);
        ASSERT_EQ(cnt.disconnected, 1);
    }
}