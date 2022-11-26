
#include <Hobgoblin/Common.hpp>
#include <Hobgoblin/Logging.hpp>
#include <SPeMPE/SPeMPE.hpp>

#include <gtest/gtest.h>

#include <array>
#include <memory>

using namespace jbatnozic::spempe;
using namespace hg::qao;

#define HOST 0
#define CLI1 1
#define CLI2 2

class DefaultLobbyBackendManagerTest : public ::testing::Test {
public:
    void SetUp() override {
        // hg::log::SetMinimalLogSeverity(hg::log::Severity::All);
        hg::RN_IndexHandlers();

        GameContext::RuntimeConfig rc{};
        _ctx[HOST] = std::make_unique<GameContext>(rc);
        _ctx[HOST]->setToMode(GameContext::Mode::Server);

        // Add networking manager
        _netMgr[HOST] = std::make_unique<DefaultNetworkingManager>(_ctx[HOST]->getQAORuntime().nonOwning(),
                                                                   PRIORITY_NETMGR,
                                                                   0);
        _netMgr[HOST]->setToMode(NetworkingManagerInterface::Mode::Server);
        _netMgr[HOST]->getServer().start(0);
        _netMgr[HOST]->getServer().resize(2);

        _ctx[HOST]->attachComponent(*_netMgr[HOST]);

        // Add varmap manager
        _svmMgr[HOST] = std::make_unique<DefaultSyncedVarmapManager>(_ctx[HOST]->getQAORuntime().nonOwning(), PRIORITY_SVMMGR);
        _svmMgr[HOST]->setToMode(SyncedVarmapManagerInterface::Mode::Host);

        _ctx[HOST]->attachComponent(*_svmMgr[HOST]);

        // Add lobby manager
        _lobbyMgr[HOST] = std::make_unique<DefaultLobbyBackendManager>(_ctx[HOST]->getQAORuntime().nonOwning(), PRIORITY_LOBMGR);
        _lobbyMgr[HOST]->setToHostMode(3);

        _ctx[HOST]->attachComponent(*_lobbyMgr[HOST]);
    }

    void TearDown() override {
        _cleanupContext(CLI2);
        _cleanupContext(CLI1);
        _cleanupContext(HOST);
    }

protected:
    constexpr static int PRIORITY_SVMMGR = 11;
    constexpr static int PRIORITY_NETMGR = 10;
    constexpr static int PRIORITY_LOBMGR =  9;

    std::array<std::unique_ptr<GameContext>, 3> _ctx;
    std::array<std::unique_ptr<NetworkingManagerInterface>, 3> _netMgr;
    std::array<std::unique_ptr<DefaultSyncedVarmapManager>, 3> _svmMgr;
    std::array<std::unique_ptr<DefaultLobbyBackendManager>, 3> _lobbyMgr;

    void _initClientContext(hg::PZInteger aContextIndex) {
        const auto pos = hg::pztos(aContextIndex);

        GameContext::RuntimeConfig rc{};
        _ctx[pos] = std::make_unique<GameContext>(rc);
        _ctx[pos]->setToMode(GameContext::Mode::Client);

        // Add networking manager
        _netMgr[pos] = std::make_unique<DefaultNetworkingManager>(_ctx[pos]->getQAORuntime().nonOwning(),
                                                                  PRIORITY_NETMGR,
                                                                  0);
        _netMgr[pos]->setToMode(NetworkingManagerInterface::Mode::Client);
        _netMgr[pos]->getClient().connectLocal(_netMgr[HOST]->getServer());

        _ctx[pos]->attachComponent(*_netMgr[pos]);

        // Add varmap manager
        _svmMgr[pos] = std::make_unique<DefaultSyncedVarmapManager>(_ctx[pos]->getQAORuntime().nonOwning(), PRIORITY_SVMMGR);
        _svmMgr[pos]->setToMode(SyncedVarmapManagerInterface::Mode::Client);

        _ctx[pos]->attachComponent(*_svmMgr[pos]);

        // Add lobby manager
        _lobbyMgr[pos] = std::make_unique<DefaultLobbyBackendManager>(_ctx[pos]->getQAORuntime().nonOwning(), PRIORITY_LOBMGR);
        _lobbyMgr[pos]->setToClientMode(1);

        _ctx[pos]->attachComponent(*_lobbyMgr[pos]);
    }

    void _cleanupContext(hg::PZInteger aContextIndex) {
        const auto pos = hg::pztos(aContextIndex);

        if (!_ctx[pos]) {
            return;
        }

        if (_lobbyMgr[pos]) {
            _ctx[pos]->detachComponent(*_lobbyMgr[pos]);
            _lobbyMgr[pos].reset();
        }

        if (_svmMgr[pos]) {
            _ctx[pos]->detachComponent(*_svmMgr[pos]);
            _svmMgr[pos].reset();
        }

        if (_netMgr[pos]) {
            _ctx[pos]->detachComponent(*_netMgr[pos]);
            _netMgr[pos].reset();
        }

        _ctx[pos].reset();
    }

    void _runAllContextsFor(hg::PZInteger aStepCount) {
        for (hg::PZInteger i = 0; i < aStepCount; i += 1) {
            if (_ctx[CLI2]) {
                _ctx[CLI2]->runFor(1);
            }
            if (_ctx[CLI1]) {
                _ctx[CLI1]->runFor(1);
            }
            if (_ctx[HOST]) {
                _ctx[HOST]->runFor(1);
            }
        }
    }
};

TEST_F(DefaultLobbyBackendManagerTest, InitialStateTest) {
    EXPECT_EQ(_lobbyMgr[HOST]->getLocalPlayerIndex(), 0); // Host is in slot 0 unless changed

    _lobbyMgr[HOST]->setLocalName("host");
    _lobbyMgr[HOST]->setLocalUniqueId("1234");
    _lobbyMgr[HOST]->setLocalCustomData(0, "cdat0");

    EXPECT_EQ(_lobbyMgr[HOST]->getLocalName(), "host");
    EXPECT_EQ(_lobbyMgr[HOST]->getLocalUniqueId(), "1234");
    EXPECT_EQ(_lobbyMgr[HOST]->getLocalCustomData(0), "cdat0");

    // Check initial state - slot 0 should be the host and the other 2 empty
    {
        const auto& info = _lobbyMgr[HOST]->getPendingPlayerInfo(0);
        EXPECT_EQ(info.name, "host");
        EXPECT_EQ(info.uniqueId, "1234");
        EXPECT_EQ(info.customData[0], "cdat0");

        EXPECT_EQ(_lobbyMgr[HOST]->getLockedInPlayerInfo(0), info);

        EXPECT_EQ(_lobbyMgr[HOST]->playerIdxToClientIdx(0), CLIENT_INDEX_LOCAL);
    }
    {
        for (hg::PZInteger slot = 1; slot < 3; slot += 1) {
            const auto& info = _lobbyMgr[HOST]->getPendingPlayerInfo(slot);
            EXPECT_EQ(info.name, "");
            EXPECT_EQ(info.uniqueId, "");
            EXPECT_EQ(info.customData[0], "");

            EXPECT_EQ(_lobbyMgr[HOST]->getPendingPlayerInfo(slot), info);

            EXPECT_EQ(_lobbyMgr[HOST]->playerIdxToClientIdx(slot), CLIENT_INDEX_UNKNOWN);
        }
    }
}

TEST_F(DefaultLobbyBackendManagerTest, SingleClientTest) {
    _lobbyMgr[HOST]->setLocalName("host");
    _lobbyMgr[HOST]->setLocalUniqueId("1234");
    _lobbyMgr[HOST]->setLocalCustomData(0, "cdat0");

    _initClientContext(CLI1);
    _runAllContextsFor(2);

    ASSERT_EQ(_netMgr[CLI1]->getClient().getServerConnector().getStatus(),
              jbatnozic::hobgoblin::RN_ConnectorStatus::Connected);

    ASSERT_EQ(_lobbyMgr[CLI1]->getSize(), 3);

    // Host is in both [0] slots
    {
        const auto& info = _lobbyMgr[HOST]->getPendingPlayerInfo(0);
        EXPECT_EQ(info.name, "host");
        EXPECT_EQ(info.uniqueId, "1234");
        EXPECT_EQ(info.customData[0], "cdat0");

        EXPECT_EQ(_lobbyMgr[HOST]->getLockedInPlayerInfo(0), info);
    }
    // CLI1 is pending in slot [1] but locked-in slot [1] is empty
    {
        const auto& info = _lobbyMgr[CLI1]->getPendingPlayerInfo(1);
        EXPECT_FALSE(info.isEmpty());
        EXPECT_FALSE(info.isComplete());
    }
    {
        const auto& info = _lobbyMgr[CLI1]->getLockedInPlayerInfo(1);
        EXPECT_TRUE(info.isEmpty());
        EXPECT_EQ(info.customData[0], "");
    }
    // Both [2] slots are empty
    {
        const auto& info = _lobbyMgr[CLI1]->getPendingPlayerInfo(2);
        EXPECT_TRUE(info.isEmpty());
        EXPECT_EQ(info.customData[0], "");

        EXPECT_EQ(_lobbyMgr[CLI1]->getLockedInPlayerInfo(2), info);
    }

    _lobbyMgr[CLI1]->setLocalName("cli1");
    _lobbyMgr[CLI1]->setLocalUniqueId("5678");
    _lobbyMgr[CLI1]->setLocalCustomData(0, "CDAT0");
    _lobbyMgr[CLI1]->uploadLocalInfo();

    _runAllContextsFor(2);

    {
        const auto& info = _lobbyMgr[CLI1]->getPendingPlayerInfo(1);
        EXPECT_EQ(info.name, "cli1");
        EXPECT_EQ(info.uniqueId, "5678");
        EXPECT_EQ(info.customData[0], "CDAT0");
    }

    ASSERT_EQ(_lobbyMgr[HOST]->clientIdxToPlayerIdx(0), PLAYER_INDEX_UNKNOWN);
    ASSERT_EQ(_lobbyMgr[CLI1]->getLocalPlayerIndex(), PLAYER_INDEX_UNKNOWN);

    _lobbyMgr[HOST]->lockInPendingChanges();

    _runAllContextsFor(2);

    ASSERT_EQ(_lobbyMgr[CLI1]->getLocalPlayerIndex(), 1);
    ASSERT_EQ(_lobbyMgr[HOST]->clientIdxToPlayerIdx(0), 1);

    // Test disconnect
    _netMgr[HOST]->getServer().setTimeoutLimit(std::chrono::microseconds{1});
    _netMgr[CLI1]->getClient().disconnect(false);
    _runAllContextsFor(4);
    ASSERT_EQ(_netMgr[HOST]->getServer().getClientConnector(0).getStatus(),
              jbatnozic::hobgoblin::RN_ConnectorStatus::Disconnected);

    {
        const auto& info = _lobbyMgr[HOST]->getPendingPlayerInfo(1);
        EXPECT_TRUE(info.isEmpty());
    }
    {
        const auto& info = _lobbyMgr[HOST]->getLockedInPlayerInfo(1);
        EXPECT_EQ(info.name, "cli1");
        EXPECT_EQ(info.uniqueId, "5678");
        EXPECT_EQ(info.customData[0], "CDAT0");

        EXPECT_FALSE(info.isEmpty());
        EXPECT_TRUE(info.isComplete());
    }
}

TEST_F(DefaultLobbyBackendManagerTest, MultipleClientsTest) {
    /*
     * SCENARIO: server start, client 1 connect, lock in, client 2 connect, client 1 disconnect, lock in
     */

    _lobbyMgr[HOST]->setLocalName("host");
    _lobbyMgr[HOST]->setLocalUniqueId("1234");
    _lobbyMgr[HOST]->setLocalCustomData(0, "cdat0");

    _initClientContext(CLI1);
    _runAllContextsFor(2);

    ASSERT_EQ(_netMgr[CLI1]->getClient().getServerConnector().getStatus(),
              jbatnozic::hobgoblin::RN_ConnectorStatus::Connected);

    _lobbyMgr[HOST]->lockInPendingChanges();

    _runAllContextsFor(2);

    // Verify slots from CLI1 perspective
    {
        const auto& info = _lobbyMgr[CLI1]->getPendingPlayerInfo(0);
        EXPECT_EQ(info.name, "host");
        EXPECT_EQ(info.uniqueId, "1234");
        EXPECT_EQ(info.customData[0], "cdat0");

        EXPECT_EQ(_lobbyMgr[CLI1]->getLockedInPlayerInfo(0), info);
    }
    {
        const auto& info = _lobbyMgr[CLI1]->getPendingPlayerInfo(1);
        EXPECT_FALSE(info.isEmpty());

        EXPECT_EQ(_lobbyMgr[CLI1]->getLockedInPlayerInfo(1), info);
    }
    {
        const auto& info = _lobbyMgr[CLI1]->getPendingPlayerInfo(2);
        EXPECT_TRUE(info.isEmpty());
        EXPECT_EQ(info.customData[0], "");

        EXPECT_EQ(_lobbyMgr[CLI1]->getLockedInPlayerInfo(2), info);
    }

    _initClientContext(CLI2);
    _runAllContextsFor(2);

    ASSERT_EQ(_netMgr[CLI2]->getClient().getServerConnector().getStatus(),
              jbatnozic::hobgoblin::RN_ConnectorStatus::Connected);

    // Verify slots from CLI1 perspective
    {
        const auto& info = _lobbyMgr[CLI1]->getPendingPlayerInfo(0);
        EXPECT_EQ(info.name, "host");
        EXPECT_EQ(info.uniqueId, "1234");
        EXPECT_EQ(info.customData[0], "cdat0");

        EXPECT_EQ(_lobbyMgr[CLI1]->getLockedInPlayerInfo(0), info);
    }
    {
        const auto& info = _lobbyMgr[CLI1]->getPendingPlayerInfo(1);
        EXPECT_FALSE(info.isEmpty());

        EXPECT_EQ(_lobbyMgr[CLI1]->getLockedInPlayerInfo(1), info);
    }
    {
        const auto& info = _lobbyMgr[CLI1]->getPendingPlayerInfo(2);
        EXPECT_FALSE(info.isEmpty());
    }
    {
        const auto& info = _lobbyMgr[CLI1]->getLockedInPlayerInfo(2);
        EXPECT_TRUE(info.isEmpty());
    }

    // Verify slots from CLI2 perspective
    {
        const auto& info = _lobbyMgr[CLI2]->getPendingPlayerInfo(0);
        EXPECT_EQ(info.name, "host");
        EXPECT_EQ(info.uniqueId, "1234");
        EXPECT_EQ(info.customData[0], "cdat0");

        EXPECT_EQ(_lobbyMgr[CLI2]->getLockedInPlayerInfo(0), info);
    }
    {
        const auto& info = _lobbyMgr[CLI2]->getPendingPlayerInfo(1);
        EXPECT_FALSE(info.isEmpty());

        EXPECT_EQ(_lobbyMgr[CLI2]->getLockedInPlayerInfo(1), info);
    }
    {
        const auto& info = _lobbyMgr[CLI2]->getPendingPlayerInfo(2);
        EXPECT_FALSE(info.isEmpty());
    }
    {
        const auto& info = _lobbyMgr[CLI2]->getLockedInPlayerInfo(2);
        EXPECT_TRUE(info.isEmpty());
    }

    _netMgr[HOST]->getServer().setTimeoutLimit(std::chrono::microseconds{1});
    _netMgr[CLI1]->getClient().disconnect(false);
    _runAllContextsFor(4);
    ASSERT_EQ(_netMgr[HOST]->getServer().getClientConnector(0).getStatus(),
              jbatnozic::hobgoblin::RN_ConnectorStatus::Disconnected);
    ASSERT_EQ(_netMgr[HOST]->getServer().getClientConnector(1).getStatus(),
              jbatnozic::hobgoblin::RN_ConnectorStatus::Connected);

    // Verify slots from CLI2 perspective
    {
        const auto& info = _lobbyMgr[CLI2]->getPendingPlayerInfo(0);
        EXPECT_EQ(info.name, "host");
        EXPECT_EQ(info.uniqueId, "1234");
        EXPECT_EQ(info.customData[0], "cdat0");

        EXPECT_EQ(_lobbyMgr[CLI2]->getLockedInPlayerInfo(0), info);
    }
    {
        const auto& info = _lobbyMgr[CLI2]->getPendingPlayerInfo(1);
        EXPECT_TRUE(info.isEmpty());
    }
    {
        const auto& info = _lobbyMgr[CLI2]->getLockedInPlayerInfo(1);
        EXPECT_FALSE(info.isEmpty());
    }
    {
        const auto& info = _lobbyMgr[CLI2]->getPendingPlayerInfo(2);
        EXPECT_FALSE(info.isEmpty());
    }
    {
        const auto& info = _lobbyMgr[CLI2]->getLockedInPlayerInfo(2);
        EXPECT_TRUE(info.isEmpty());
    }

    _lobbyMgr[HOST]->lockInPendingChanges();
    _runAllContextsFor(2);

    {
        const auto& info = _lobbyMgr[CLI2]->getPendingPlayerInfo(0);
        EXPECT_EQ(info.name, "host");
        EXPECT_EQ(info.uniqueId, "1234");
        EXPECT_EQ(info.customData[0], "cdat0");

        EXPECT_EQ(_lobbyMgr[CLI2]->getLockedInPlayerInfo(0), info);
    }
    {
        const auto& info = _lobbyMgr[CLI2]->getPendingPlayerInfo(1);
        EXPECT_TRUE(info.isEmpty());

        EXPECT_EQ(_lobbyMgr[CLI2]->getLockedInPlayerInfo(1), info);
    }
    {
        const auto& info = _lobbyMgr[CLI2]->getPendingPlayerInfo(2);
        EXPECT_FALSE(info.isEmpty());

        EXPECT_EQ(_lobbyMgr[CLI2]->getLockedInPlayerInfo(2), info);
    }
}

TEST_F(DefaultLobbyBackendManagerTest, SwapSlotsTest) {
    // TODO
}
