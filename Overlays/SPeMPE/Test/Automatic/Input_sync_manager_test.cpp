// Copyright 2024 Jovan Batnozic. Released under MS-PL licence in Serbia.
// See https://github.com/jbatnozic/Hobgoblin?tab=readme-ov-file#licence

// clang-format off


#include <Hobgoblin/Common.hpp>
#include <SPeMPE/SPeMPE.hpp>

#include <gtest/gtest.h>

#include <memory>

using namespace jbatnozic::spempe;
using namespace hg::qao;
using namespace hg::rn;

class InputSyncManagerTest : public ::testing::Test {
public:
    void SetUp() override {
        RN_IndexHandlers();
    }

    void TearDown() override {
        _netMgr2->getClient().disconnect(false);
        _netMgr1->getServer().stop();

        DetachStatus detachStatus;
        _ctx1->detachComponent<NetworkingManagerInterface>(&detachStatus);
        ASSERT_EQ(detachStatus, DetachStatus::NOT_OWNED_BY_CONTEXT);
        _ctx1->detachComponent<InputSyncManagerInterface>(&detachStatus);
        ASSERT_EQ(detachStatus, DetachStatus::NOT_OWNED_BY_CONTEXT);
        _ctx2->detachComponent<NetworkingManagerInterface>(&detachStatus);
        ASSERT_EQ(detachStatus, DetachStatus::NOT_OWNED_BY_CONTEXT);
        _ctx2->detachComponent<InputSyncManagerInterface>(&detachStatus);
        ASSERT_EQ(detachStatus, DetachStatus::NOT_OWNED_BY_CONTEXT);

        _netMgr1.reset();
        _insMgr1.reset();
        _netMgr2.reset();
        _insMgr2.reset();

        _ctx1.reset();
        _ctx2.reset();
    }

protected:
    constexpr static int PRIORITY_NETMGR = 5;
    constexpr static int PRIORITY_INSMGR = 0;

    using SignalType  = int;
    using EventWPType = int;
    constexpr static const char* SIGNAL_NAME  = "signal";
    constexpr static const char* EVENT_NAME   = "event";
    constexpr static const char* EVENTWP_NAME = "eventwp";
    constexpr static const SignalType  SIGNAL_INIT = 0;

    std::unique_ptr<GameContext> _ctx1;
    std::unique_ptr<GameContext> _ctx2;

    std::unique_ptr<NetworkingManagerInterface> _netMgr1;
    std::unique_ptr<NetworkingManagerInterface> _netMgr2;

    std::unique_ptr<InputSyncManagerInterface> _insMgr1;
    std::unique_ptr<InputSyncManagerInterface> _insMgr2;

    void _setUp(hg::PZInteger aStateBufferingLength) {
        GameContext::RuntimeConfig rc{};
        _ctx1 = std::make_unique<GameContext>(rc);
        _ctx1->setToMode(GameContext::Mode::Server);
        _ctx2 = std::make_unique<GameContext>(rc);
        _ctx2->setToMode(GameContext::Mode::Client);

        // Add networking managers
        _netMgr1 = std::make_unique<DefaultNetworkingManager>(_ctx1->getQAORuntime().nonOwning(),
                                                              PRIORITY_NETMGR,
                                                              aStateBufferingLength);
        _netMgr1->setToServerMode(RN_Protocol::UDP, "pass", 2, 512, RN_NetworkingStack::Default);

        _netMgr2 = std::make_unique<DefaultNetworkingManager>(_ctx2->getQAORuntime().nonOwning(),
                                                              PRIORITY_NETMGR,
                                                              aStateBufferingLength);
        _netMgr2->setToClientMode(RN_Protocol::UDP, "pass", 512, RN_NetworkingStack::Default);

        {
            auto& server = _netMgr1->getServer();
            auto& client = _netMgr2->getClient();

            server.start(0);
            client.connectLocal(server);
        }

        _ctx1->attachComponent(*_netMgr1);
        _ctx2->attachComponent(*_netMgr2);

        // Add input sync managers
        _insMgr1 = std::make_unique<DefaultInputSyncManager>(_ctx1->getQAORuntime().nonOwning(), PRIORITY_INSMGR);
        // aPlayerCount is 2 because player 0 is the local player and player 1 is the actual client
        _insMgr1->setToHostMode(2, aStateBufferingLength);
        _defineInputs(*_insMgr1);

        _insMgr2 = std::make_unique<DefaultInputSyncManager>(_ctx2->getQAORuntime().nonOwning(), PRIORITY_INSMGR);
        _insMgr2->setToClientMode();
        _defineInputs(*_insMgr2);

        _ctx1->attachComponent(*_insMgr1);
        _ctx2->attachComponent(*_insMgr2);

        // Run both contexts a little to propagate the connection
        _ctx2->runFor(1);
        _ctx1->runFor(1);
        _ctx2->runFor(1);
        _ctx1->runFor(1);
    }

    void _defineInputs(InputSyncManagerInterface& aInputSyncManager) {
        const InputSyncManagerWrapper wrap{aInputSyncManager};

        wrap.defineSignal<int>(SIGNAL_NAME, SIGNAL_INIT);
        wrap.defineSimpleEvent(EVENT_NAME);
        wrap.defineEventWithPayload<int>(EVENTWP_NAME);
    }

    static hg::PZInteger _countEventWP(const InputSyncManagerWrapper& aWrapper, 
                                       hg::PZInteger aForPlayer, 
                                       std::string aEventName) {
        hg::PZInteger count = 0;
        aWrapper.pollEventWithPayload<EventWPType>(aForPlayer, std::move(aEventName),
                                                   [&](const EventWPType&) {
                                                       count++;
                                                   });
        return count;
    }
};

TEST_F(InputSyncManagerTest, OverlappingNameInDefinition_ThrowsException) {
    _setUp(0);

    const InputSyncManagerWrapper wrap{*_insMgr1};

    EXPECT_THROW(wrap.defineSignal<SignalType>(SIGNAL_NAME, 123), hg::TracedLogicError);
    EXPECT_THROW(wrap.defineSimpleEvent(EVENT_NAME), hg::TracedLogicError);
    EXPECT_THROW(wrap.defineEventWithPayload<int>(EVENTWP_NAME), hg::TracedLogicError);
}

TEST_F(InputSyncManagerTest, OverlappingNameInDefinition_OkWithDifferentKind) {
    _setUp(0);

    const InputSyncManagerWrapper wrap{*_insMgr1};

    EXPECT_NO_THROW(wrap.defineSignal<SignalType>(EVENTWP_NAME, 123));
    EXPECT_NO_THROW(wrap.defineSimpleEvent(SIGNAL_NAME));
    EXPECT_NO_THROW(wrap.defineEventWithPayload<int>(EVENT_NAME));
}

TEST_F(InputSyncManagerTest, HappyPathTest_NoStateBuffering) {
    _setUp(0); // No state buffering

    const InputSyncManagerWrapper wrap1{*_insMgr1}; // Host
    const InputSyncManagerWrapper wrap2{*_insMgr2}; // Client

    EXPECT_EQ(wrap1.getSignalValue<SignalType>(CLIENT_INDEX_LOCAL, SIGNAL_NAME), SIGNAL_INIT);
    EXPECT_EQ(wrap1.countSimpleEvent(CLIENT_INDEX_LOCAL, EVENT_NAME), 0);
    EXPECT_EQ(_countEventWP(wrap1, CLIENT_INDEX_LOCAL, EVENTWP_NAME), 0);

    wrap2.setSignalValue<SignalType>(SIGNAL_NAME, 420);
    wrap2.triggerEvent(EVENT_NAME);
    wrap2.triggerEventWithPayload<EventWPType>(EVENTWP_NAME, 1337);

    _ctx2->runFor(1); // Inputs sent
    _ctx1->runFor(1); // Inputs received

    // There is no state buffering, so we expect the new inputs immediately
    EXPECT_EQ(wrap1.getSignalValue<SignalType>(0, SIGNAL_NAME), 420);
    EXPECT_EQ(wrap1.countSimpleEvent(0, EVENT_NAME), 1);
    EventWPType payload = 0;
    EXPECT_EQ(wrap1.pollEventWithPayload<EventWPType>(0, EVENTWP_NAME,
                                                      [&](const EventWPType& aPayload) {
                                                          payload = aPayload;
                                                      }), 1);
    EXPECT_EQ(payload, 1337);

    _ctx1->runFor(1);

    // After one more cycle, the signals should remain the same but the events chould be cleared
    EXPECT_EQ(wrap1.getSignalValue<SignalType>(0, SIGNAL_NAME), 420);
    EXPECT_EQ(wrap1.countSimpleEvent(0, EVENT_NAME), 0);
}

TEST_F(InputSyncManagerTest, HappyPathTest_StateBuffering_1) {
    _setUp(1); // 1 frame of state buffering

    const InputSyncManagerWrapper wrap1{*_insMgr1}; // Host
    const InputSyncManagerWrapper wrap2{*_insMgr2}; // Client

    EXPECT_EQ(wrap1.getSignalValue<SignalType>(0, SIGNAL_NAME), SIGNAL_INIT);
    EXPECT_EQ(wrap1.countSimpleEvent(0, EVENT_NAME), 0);

    wrap2.setSignalValue<SignalType>(SIGNAL_NAME, 420);
    wrap2.triggerEvent(EVENT_NAME);

    _ctx2->runFor(1); // Inputs sent
    _ctx1->runFor(1); // Inputs received

    // Inputs are delayed by 1 frame so we still expect the original/default values
    EXPECT_EQ(wrap1.getSignalValue<SignalType>(0, SIGNAL_NAME), SIGNAL_INIT);
    EXPECT_EQ(wrap1.countSimpleEvent(0, EVENT_NAME), 0);

    _ctx1->runFor(1);

    // After 1 more cycle, the received inputs become current
    EXPECT_EQ(wrap1.getSignalValue<SignalType>(0, SIGNAL_NAME), 420);
    EXPECT_EQ(wrap1.countSimpleEvent(0, EVENT_NAME), 1);

    _ctx1->runFor(1);

    // After yet another cycle, the signals should remain the same but the events chould be cleared
    EXPECT_EQ(wrap1.getSignalValue<SignalType>(0, SIGNAL_NAME), 420);
    EXPECT_EQ(wrap1.countSimpleEvent(0, EVENT_NAME), 0);
}

// clang-format on
