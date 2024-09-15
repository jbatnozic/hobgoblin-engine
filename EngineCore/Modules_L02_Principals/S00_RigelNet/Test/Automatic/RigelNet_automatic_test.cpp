// Copyright 2024 Jovan Batnozic. Released under MS-PL licence in Serbia.
// See https://github.com/jbatnozic/Hobgoblin?tab=readme-ov-file#licence

#include <gtest/gtest.h>

#define HOBGOBLIN_SHORT_NAMESPACE
#include <Hobgoblin/Common.hpp>
#include <Hobgoblin/HGExcept.hpp>
#include <Hobgoblin/RigelNet.hpp>
#include <Hobgoblin/RigelNet_macros.hpp>
using namespace hg::rn;

#include <chrono>
#include <cstdint>
#include <cstring>
#include <memory>
#include <thread>
#include <vector>

namespace {
const std::string       PASS            = "beetlejuice";
constexpr hg::PZInteger CLIENT_COUNT    = 1;
constexpr hg::PZInteger MAX_PACKET_SIZE = 200;

struct EventCount {
    hg::PZInteger badPassphrase        = 0;
    hg::PZInteger connectAttemptFailed = 0;
    hg::PZInteger connected            = 0;
    hg::PZInteger disconnected         = 0;
};
} // namespace

// Check that the whole RPC machinery compiles:
RN_DEFINE_RPC(TestHandler, RN_ARGS(int, a, std::string, s)) {
    RN_NODE_IN_HANDLER().callIfServer([](RN_ServerInterface& /*server*/) {});
    RN_NODE_IN_HANDLER().callIfClient([](RN_ClientInterface& /*client*/) {});
}

// Check that the compose function for TestHandler is generated:
void DummyFuncCallsComposeForTestHandler() {
    auto node = reinterpret_cast<RN_NodeInterface*>(0x12345678);
    Compose_TestHandler(*node, RN_COMPOSE_FOR_ALL, 1337, "tubular");
}

// Check that a handler with up to 10 arguments can be compiled
RN_DEFINE_RPC(
    HandlerWithTenArguments,
    RN_ARGS(int, i0, int, i1, int, i2, int, i3, int, i4, int, i5, int, i6, int, i7, int, i8, int, i9)) {
    i0 = i1 = i2 = i3 = i4 = i5 = i6 = i7 = i8 = i9 = 666;
}

class RigelNetTest : public ::testing::Test {
public:
    RigelNetTest()
        : _server{RN_ServerFactory::createServer(RN_Protocol::UDP, PASS, CLIENT_COUNT, MAX_PACKET_SIZE)}
        , _client{RN_ClientFactory::createClient(RN_Protocol::UDP, PASS, MAX_PACKET_SIZE)} {
        RN_IndexHandlers();
    }

    void SetUp() override {
        _server->addEventListener(&_eventListenerServer);
        _client->addEventListener(&_eventListenerClient);
    }

    void TearDown() override {
        _server->removeEventListener(&_eventListenerServer);
        _client->removeEventListener(&_eventListenerClient);
    }

protected:
    std::unique_ptr<RN_ServerInterface> _server;
    std::unique_ptr<RN_ClientInterface> _client;

    const EventCount& _getEventCount(const RN_NodeInterface& aNode) const {
        if (&aNode == _server.get()) {
            return _eventCountServer;
        }
        if (&aNode == _client.get()) {
            return _eventCountClient;
        }
        HG_UNREACHABLE();
        return _eventCountServer;
    }

private:
    EventCount _eventCountServer;
    EventCount _eventCountClient;

    class EventListener : public RN_EventListener {
    public:
        explicit EventListener(EventCount& aEventCount)
            : _cnt{aEventCount} {}

        void onNetworkingEvent(const RN_Event& aEvent) override {
            aEvent.strictVisit(
                [&](const RN_Event::BadPassphrase& ev) {
                    _cnt.badPassphrase += 1;
                },
                [&](const RN_Event::ConnectAttemptFailed& ev) {
                    _cnt.connectAttemptFailed += 1;
                },
                [&](const RN_Event::Connected& ev) {
                    _cnt.connected += 1;
                },
                [&](const RN_Event::Disconnected& ev) {
                    _cnt.disconnected += 1;
                });
        }

    private:
        EventCount& _cnt;
    };

    EventListener _eventListenerServer{_eventCountServer};
    EventListener _eventListenerClient{_eventCountClient};
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
        auto cnt = _getEventCount(*_server);
        EXPECT_EQ(cnt.connected, 1);
    }

    {
        auto cnt = _getEventCount(*_client);
        EXPECT_EQ(cnt.connected, 1);
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
        auto cnt = _getEventCount(*_server);
        EXPECT_EQ(cnt.disconnected, 1);
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

RN_DEFINE_HANDLER(PiecemealHandler, RN_ARGS()) {
    RN_NODE_IN_HANDLER().callIfServer([](RN_ServerInterface& /*server*/) {
        std::abort();
    });

    RN_NODE_IN_HANDLER().callIfClient([](RN_ClientInterface& client) {
        auto& flag = *client.getUserDataOrThrow<bool>();
        flag       = true;
    });
}
RN_DEFINE_COMPOSEFUNC(PiecemealHandler, RN_ARGS());
RN_REGISTER_HANDLER_BEFORE_MAIN(PiecemealHandler);

TEST_F(RigelNetTest, PiecemealHandlerWorks) {
    bool flag = false;
    _client->setUserData(&flag);

    _server->start(0);
    _client->connectLocal(*_server);

    ASSERT_EQ(_client->getServerConnector().getStatus(), RN_ConnectorStatus::Connected);
    ASSERT_EQ(_server->getClientConnector(0).getStatus(), RN_ConnectorStatus::Connected);

    Compose_PiecemealHandler(*_server, 0);

    _server->update(RN_UpdateMode::Send);
    _client->update(RN_UpdateMode::Receive);

    ASSERT_EQ(flag, true);
}

// MARK: Fragmented Packets Test

using FragmentedPacketTestParam = int;

class RigelNetFragmentedPacketsTest
    : public RigelNetTest
    , public ::testing::WithParamInterface<FragmentedPacketTestParam> {};

RN_DEFINE_RPC_P(SendBinaryBuffer, RNTest_, RN_ARGS(RN_RawDataView, bytes)) {
    RN_NODE_IN_HANDLER().callIfServer([](RN_ServerInterface& /*server*/) {
        throw RN_IllegalMessage{};
    });

    RN_NODE_IN_HANDLER().callIfClient([&](RN_ClientInterface& client) {
        auto& vec = *client.getUserDataOrThrow<std::vector<std::uint16_t>>();
        if (bytes.getDataSize() % sizeof(vec[0]) != 0) {
            throw RN_IllegalMessage{};
        }
        vec.resize(bytes.getDataSize() / sizeof(vec[0]));
        std::memcpy(vec.data(), bytes.getData(), bytes.getDataSize());
    });
}

TEST_P(RigelNetFragmentedPacketsTest, FragmentedPacketReceived_2) {
    const int sizeModifier = GetParam();

    const int packetSizeToSend = MAX_PACKET_SIZE + sizeModifier;
    SCOPED_TRACE("packetSizeToSend = " + std::to_string(packetSizeToSend));

    std::vector<std::uint16_t> serverVector;
    for (int i = 0; i < packetSizeToSend / 2; i += 1) {
        serverVector.push_back(i);
    }

    std::vector<std::uint16_t> clientVector;
    _client->setUserData(&clientVector);

    _server->start(0);
    _client->connectLocal(*_server);

    ASSERT_EQ(_client->getServerConnector().getStatus(), RN_ConnectorStatus::Connected);
    ASSERT_EQ(_server->getClientConnector(0).getStatus(), RN_ConnectorStatus::Connected);

    RNTest_Compose_SendBinaryBuffer(
        *_server,
        RN_COMPOSE_FOR_ALL,
        RN_RawDataView(serverVector.data(), serverVector.size() * sizeof(std::uint16_t)));

    _server->update(RN_UpdateMode::Send);
    _client->update(RN_UpdateMode::Receive);

    ASSERT_EQ(serverVector.size(), clientVector.size());
    for (std::size_t i = 0; i < serverVector.size(); i += 1) {
        ASSERT_EQ(serverVector[i], clientVector[i]);
    }
}

INSTANTIATE_TEST_SUITE_P(RigelNetFragmentedPacketsTest,
                         RigelNetFragmentedPacketsTest,
                         ::testing::Range(/* start (included) */ -100,
                                          /* end (not included)*/ 101,
                                          /* step */ 1));
