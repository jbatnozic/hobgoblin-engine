
#include <Hobgoblin/Common.hpp>
#include <SPeMPE/SPeMPE.hpp>

#include <gtest/gtest.h>

#include <memory>

using namespace jbatnozic::spempe;
using namespace hg::qao;

class DefaultSyncedVarmapManagerTest : public ::testing::Test {
public:
    void SetUp() override {
        hg::RN_IndexHandlers();

        GameContext::RuntimeConfig rc{};
        _ctx1 = std::make_unique<GameContext>(rc);
        _ctx2 = std::make_unique<GameContext>(rc);

        // Add networking managers
        _netMgr1 = std::make_unique<DefaultNetworkingManager>(_ctx1->getQAORuntime().nonOwning(),
                                                              PRIORITY_NETMGR,
                                                              0);
        _netMgr1->setToMode(NetworkingManagerInterface::Mode::Server);

        _netMgr2 = std::make_unique<DefaultNetworkingManager>(_ctx2->getQAORuntime().nonOwning(),
                                                              PRIORITY_NETMGR,
                                                              0);
        _netMgr2->setToMode(NetworkingManagerInterface::Mode::Client);

        {
            auto& server = _netMgr1->getServer();
            auto& client = _netMgr2->getClient();

            server.start(0);
            client.connectLocal(server);
        }

        _ctx1->attachComponent(*_netMgr1);
        _ctx2->attachComponent(*_netMgr2);

        // Add varmap managers
        _svmMgr1 = std::make_unique<DefaultSyncedVarmapManager>(_ctx1->getQAORuntime().nonOwning(), PRIORITY_SVMMGR);
        _svmMgr1->setToMode(SyncedVarmapManagerInterface::Mode::Host);

        _svmMgr2 = std::make_unique<DefaultSyncedVarmapManager>(_ctx2->getQAORuntime().nonOwning(), PRIORITY_SVMMGR);
        _svmMgr2->setToMode(SyncedVarmapManagerInterface::Mode::Client);

        _ctx1->attachComponent(*_svmMgr1);
        _ctx2->attachComponent(*_svmMgr2);

        // Run both contexts a little to propagate the connection
        _ctx2->runFor(1);
        _ctx1->runFor(1);
        _ctx2->runFor(1);
        _ctx1->runFor(1);

        ASSERT_EQ(_netMgr2->getClient().getServerConnector().getStatus(),
                  jbatnozic::hobgoblin::RN_ConnectorStatus::Connected);
        ASSERT_EQ(_netMgr2->getLocalClientIndex(), 0);
    }

    void TearDown() override {
        _netMgr2->getClient().disconnect(false);
        _netMgr1->getServer().stop();

        _ctx1->detachComponent(*_netMgr1);
        _ctx1->detachComponent(*_svmMgr1);
        _ctx2->detachComponent(*_netMgr2);
        _ctx2->detachComponent(*_svmMgr2);

        _svmMgr1.reset();
        _netMgr1.reset();
        _svmMgr2.reset();
        _netMgr2.reset();
        
        _ctx1.reset();
        _ctx2.reset();
    }

protected:
    constexpr static int PRIORITY_SVMMGR = 11;
    constexpr static int PRIORITY_NETMGR = 10;

    constexpr static auto ALLOWED = DefaultSyncedVarmapManager::ALLOWED;

    std::unique_ptr<GameContext> _ctx1;
    std::unique_ptr<GameContext> _ctx2;

    std::unique_ptr<NetworkingManagerInterface> _netMgr1;
    std::unique_ptr<NetworkingManagerInterface> _netMgr2;

    std::unique_ptr<DefaultSyncedVarmapManager> _svmMgr1;
    std::unique_ptr<DefaultSyncedVarmapManager> _svmMgr2;
};

TEST_F(DefaultSyncedVarmapManagerTest, BasicFunctionalityTest) {
    // Check that there are no values:
    ASSERT_FALSE(_svmMgr1->getInt64("valInt64_h").has_value());
    ASSERT_FALSE(_svmMgr1->getDouble("valDouble_h").has_value());
    ASSERT_FALSE(_svmMgr1->getString("valString_h").has_value());

    ASSERT_FALSE(_svmMgr1->getInt64("valInt64_c").has_value());
    ASSERT_FALSE(_svmMgr1->getDouble("valDouble_c").has_value());
    ASSERT_FALSE(_svmMgr1->getString("valString_c").has_value());

    ASSERT_FALSE(_svmMgr2->getInt64("valInt64_h").has_value());
    ASSERT_FALSE(_svmMgr2->getDouble("valDouble_h").has_value());
    ASSERT_FALSE(_svmMgr2->getString("valString_h").has_value());

    ASSERT_FALSE(_svmMgr2->getInt64("valInt64_c").has_value());
    ASSERT_FALSE(_svmMgr2->getDouble("valDouble_c").has_value());
    ASSERT_FALSE(_svmMgr2->getString("valString_c").has_value());

    // Authorize client & set some values from host side:
    _svmMgr1->int64SetClientWritePermission("valInt64_c", 1, ALLOWED);
    _svmMgr1->doubleSetClientWritePermission("valDouble_c", 1, ALLOWED);
    _svmMgr1->stringSetClientWritePermission("valString_c", 1, ALLOWED);

    _svmMgr1->setInt64("valInt64_h", 5);
    _svmMgr1->setDouble("valDouble_h", 5.0);
    _svmMgr1->setString("valString_h", "5s");

    // Then set some values from client side:
    _svmMgr2->requestToSetInt64("valInt64_c", 6);
    _svmMgr2->requestToSetDouble("valDouble_c", 6.0);
    _svmMgr2->requestToSetString("valString_c", "6s");

    // Run both contexts a bit to propagate the values
    _ctx2->runFor(1);
    _ctx1->runFor(1);
    _ctx2->runFor(1);
    _ctx1->runFor(1);

    // Check that the values are correct now:
    ASSERT_EQ(_svmMgr1->getInt64("valInt64_h").value(), 5);
    ASSERT_EQ(_svmMgr1->getDouble("valDouble_h").value(), 5.0);
    ASSERT_EQ(_svmMgr1->getString("valString_h").value(), "5s");

    ASSERT_EQ(_svmMgr1->getInt64("valInt64_c").value(), 6);
    ASSERT_EQ(_svmMgr1->getDouble("valDouble_c").value(), 6.0);
    ASSERT_EQ(_svmMgr1->getString("valString_c").value(), "6s");

    ASSERT_EQ(_svmMgr2->getInt64("valInt64_h").value(), 5);
    ASSERT_EQ(_svmMgr2->getDouble("valDouble_h").value(), 5.0);
    ASSERT_EQ(_svmMgr2->getString("valString_h").value(), "5s");

    ASSERT_EQ(_svmMgr2->getInt64("valInt64_c").value(), 6);
    ASSERT_EQ(_svmMgr2->getDouble("valDouble_c").value(), 6.0);
    ASSERT_EQ(_svmMgr2->getString("valString_c").value(), "6s");
}

TEST_F(DefaultSyncedVarmapManagerTest, ClientTriesToSetValueAndPermission_ThrowsException) {
    EXPECT_THROW(_svmMgr2->setInt64("valInt64_c", 6), hg::TracedLogicError);
    EXPECT_THROW(_svmMgr2->setDouble("valDouble_c", 6.0), hg::TracedLogicError);
    EXPECT_THROW(_svmMgr2->setString("valString_c", "6s"), hg::TracedLogicError);

    EXPECT_THROW(_svmMgr2->int64SetClientWritePermission("valInt64_c", 1, ALLOWED), hg::TracedLogicError);
    EXPECT_THROW(_svmMgr2->doubleSetClientWritePermission("valDouble_c", 1, ALLOWED), hg::TracedLogicError);
    EXPECT_THROW(_svmMgr2->stringSetClientWritePermission("valString_c", 1, ALLOWED), hg::TracedLogicError);
}

TEST_F(DefaultSyncedVarmapManagerTest, HostTriesToRequestSet_ThrowsException) {
    EXPECT_THROW(_svmMgr1->requestToSetInt64("valInt64_h", 5), hg::TracedLogicError);
    EXPECT_THROW(_svmMgr1->requestToSetDouble("valDouble_h", 5.0), hg::TracedLogicError);
    EXPECT_THROW(_svmMgr1->requestToSetString("valString_h", "5s"), hg::TracedLogicError);
}

TEST_F(DefaultSyncedVarmapManagerTest, ClientRequestsToSetUnauthorizedValue_Disconnects) {
    _svmMgr2->requestToSetInt64("valInt64_c", 6);

    // Run both contexts a bit to propagate the values
    _ctx2->runFor(1);
    _ctx1->runFor(1);
    _ctx2->runFor(1);
    _ctx1->runFor(1);

    // Check that the client has been disconnected
    ASSERT_EQ(_netMgr1->getServer().getClientConnector(0).getStatus(),
              jbatnozic::hobgoblin::RN_ConnectorStatus::Disconnected);

    ASSERT_EQ(_netMgr2->getClient().getServerConnector().getStatus(),
              jbatnozic::hobgoblin::RN_ConnectorStatus::Disconnected);
}
