
#include <gtest/gtest.h>

#define HOBGOBLIN_SHORT_NAMESPACE
#include <Hobgoblin/Common.hpp>
#include <Hobgoblin/RigelNet.hpp>
#include <Hobgoblin/RigelNet_Macros.hpp>
using namespace hg::rn;

#include <chrono>
#include <cstdint>
#include <cstring>
#include <memory>
#include <thread>
#include <vector>

namespace {
const std::string PASS = "beetlejuice";
constexpr hg::PZInteger CLIENT_COUNT = 1;
constexpr hg::PZInteger MAX_PACKET_SIZE = 200;

struct EventCount {
    hg::PZInteger badPassphrase = 0;
    hg::PZInteger connectAttemptFailed = 0;
    hg::PZInteger connected = 0;
    hg::PZInteger disconnected = 0;
};
} // namespace

// Check that the whole RPC machinery compiles:
RN_DEFINE_RPC(TestHandler, RN_ARGS(int, a, std::string, s)) {
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

// Check that a handler with up to 10 arguments can be compiled
RN_DEFINE_RPC(HandlerWithTenArguments,
              RN_ARGS(int, i0, int, i1, int, i2, int, i3, int, i4,
                      int, i5, int, i6, int, i7, int, i8, int, i9)) {
    i0 = i1 = i2 = i3 = i4 = i5 = i6 = i7 = i8 = i9 = 666;
}

class RigelNetTest : public ::testing::Test {
public:
    RigelNetTest()
        : _server{RN_ServerFactory::createServer(RN_Protocol::UDP, PASS, CLIENT_COUNT, MAX_PACKET_SIZE)}
        , _client{RN_ClientFactory::createClient(RN_Protocol::UDP, PASS, MAX_PACKET_SIZE)}
    {
        RN_IndexHandlers();
    }

protected:
    std::unique_ptr<RN_ServerInterface> _server;
    std::unique_ptr<RN_ClientInterface> _client;

    EventCount pollAndCountEvents(RN_NodeInterface& node) {
        EventCount cnt;

        RN_Event ev;
        while (node.pollEvent(ev)) {
            ev.strictVisit(
                [&](const RN_Event::BadPassphrase& ev) {
                    cnt.badPassphrase += 1;
                },
                [&](const RN_Event::ConnectAttemptFailed& ev) {
                    cnt.connectAttemptFailed += 1;
                },
                [&](const RN_Event::Connected& ev) {
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

TEST_F(RigelNetTest, CanStopANodeMultipleTimesSafely) {
    // When not even started:
    _server->stop();
    _server->stop();
    _client->disconnect(false);
    _client->disconnect(false);

    // After being started:
    _server->start(0);
    _client->connectLocal(*_server);

    _server->stop();
    _server->stop();
    _client->disconnect(false);
    _client->disconnect(false);
}

TEST_F(RigelNetTest, ConsecutiveStartsFail) {
    _server->start(0);
    EXPECT_THROW(_server->start(0), hg::TracedException);
}

TEST_F(RigelNetTest, ConsecutiveConnectsFail) {
    _client->connect(0, sf::IpAddress::LocalHost, 2048);
    EXPECT_THROW(_client->connect(0, sf::IpAddress::LocalHost, 2048), hg::TracedException);
}

TEST_F(RigelNetTest, LocalConnectToUnstartedServerFails) {
    EXPECT_THROW(_client->connectLocal(*_server), hg::TracedException);
}

RN_DEFINE_RPC_P(SendBinaryBuffer, PREF_, RN_ARGS(RN_RawDataView, bytes)) {
    RN_NODE_IN_HANDLER().callIfServer(
        [](RN_ServerInterface& /*server*/) {
            std::abort();
        });

    RN_NODE_IN_HANDLER().callIfClient(
        [&](RN_ClientInterface& client) {
            auto& vec = *client.getUserDataOrThrow<std::vector<std::int32_t>>();
            vec.resize((bytes.getDataSize() + 3) / 4);
            std::memcpy(vec.data(), bytes.getData(), bytes.getDataSize());
        });
}

TEST_F(RigelNetTest, FragmentedPacketReceived) {
    std::vector<std::int32_t> serverVector;
    for (int i = 0; i < 64; i += 1) { // 64 ints is 256 bytes, our test nodes can send only 200 at a time
        serverVector.push_back(i);
    }

    std::vector<std::int32_t> clientVector;
    _client->setUserData(&clientVector);

    _server->start(0);
    _client->connectLocal(*_server);

    ASSERT_EQ(_client->getServerConnector().getStatus(),  RN_ConnectorStatus::Connected);
    ASSERT_EQ(_server->getClientConnector(0).getStatus(), RN_ConnectorStatus::Connected);

    PREF_Compose_SendBinaryBuffer(*_server, RN_COMPOSE_FOR_ALL,
                                  RN_RawDataView(serverVector.data(), serverVector.size() * sizeof(std::int32_t)));

    _server->update(RN_UpdateMode::Send);
    _client->update(RN_UpdateMode::Receive);

    ASSERT_EQ(serverVector.size(), clientVector.size());
    for (std::size_t i = 0; i < serverVector.size(); i += 1) {
        ASSERT_EQ(serverVector[i], clientVector[i]);
    }
}

RN_DEFINE_HANDLER(PiecemealHandler, RN_ARGS()) {
    RN_NODE_IN_HANDLER().callIfServer(
        [](RN_ServerInterface& /*server*/) {
            std::abort();
        });

    RN_NODE_IN_HANDLER().callIfClient(
        [](RN_ClientInterface& client) {
            auto& flag = *client.getUserDataOrThrow<bool>();
            flag = true;
        });
}
RN_DEFINE_COMPOSEFUNC(PiecemealHandler, RN_ARGS());
RN_REGISTER_HANDLER_BEFORE_MAIN(PiecemealHandler);

TEST_F(RigelNetTest, PiecemealHandlerWorks) {
    bool flag = false;
    _client->setUserData(&flag);

    _server->start(0);
    _client->connectLocal(*_server);

    ASSERT_EQ(_client->getServerConnector().getStatus(),  RN_ConnectorStatus::Connected);
    ASSERT_EQ(_server->getClientConnector(0).getStatus(), RN_ConnectorStatus::Connected);

    Compose_PiecemealHandler(*_server, 0);

    _server->update(RN_UpdateMode::Send);
    _client->update(RN_UpdateMode::Receive);

    ASSERT_EQ(flag, true);
}