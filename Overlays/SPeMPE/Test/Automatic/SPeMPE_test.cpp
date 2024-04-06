
#include <SPeMPE/SPeMPE.hpp>

#include <Hobgoblin/Utility/Autopack.hpp>

#include <gtest/gtest.h>

#include <cstdint>
#include <memory>
#include <optional>
#include <utility>

namespace jbatnozic {
namespace spempe {

namespace {

class SPeMPE_Test : public ::testing::Test {
protected:
    void SetUp() override {
        hg::RN_IndexHandlers();
    }

    // Held in an optional<> so we can destroy on demand.
    std::optional<GameContext> _gameCtx{GameContext::RuntimeConfig{}};
};

class DummyInterface : public ContextComponent {
public:
    virtual std::int64_t getData() const = 0;

private:
    SPEMPE_CTXCOMP_TAG("DummyComponent");
};

struct DummyComponent : public DummyInterface {
    std::int64_t data;

    std::int64_t getData() const override {
        return data;
    }
};
} // namespace

TEST_F(SPeMPE_Test, ContextComponentTest) {
    {
        SCOPED_TRACE("Context doesn't own component");

        DummyComponent dummy;
        dummy.data = 0x12345678;

        _gameCtx->attachComponent(dummy);
        ASSERT_EQ(_gameCtx->getComponent<DummyInterface>().getData(), 0x12345678);

        _gameCtx->detachComponent(dummy);
        ASSERT_THROW(_gameCtx->getComponent<DummyInterface>(), hg::TracedLogicError);
    }
    {
        SCOPED_TRACE("Context owns component");

        auto dummy = std::make_unique<DummyComponent>();
        dummy->data = 0x12345678;

        _gameCtx->attachAndOwnComponent(std::move(dummy));
        ASSERT_EQ(_gameCtx->getComponent<DummyInterface>().getData(), 0x12345678);

        _gameCtx->detachComponent(_gameCtx->getComponent<DummyInterface>());
        ASSERT_THROW(_gameCtx->getComponent<DummyInterface>(), hg::TracedLogicError);
    }
}

TEST_F(SPeMPE_Test, ChildContextTest) {
    ASSERT_FALSE(_gameCtx->hasChildContext());
    _gameCtx->attachChildContext(std::make_unique<GameContext>(GameContext::RuntimeConfig{}));
    ASSERT_THROW(
        _gameCtx->attachChildContext(std::make_unique<GameContext>(GameContext::RuntimeConfig{})),
        hg::TracedLogicError
    );

    auto cc = _gameCtx->detachChildContext();
    ASSERT_NE(cc, nullptr);
    ASSERT_EQ(_gameCtx->detachChildContext(), nullptr);
    ASSERT_FALSE(_gameCtx->hasChildContext());

    _gameCtx->attachChildContext(std::move(cc));
    ASSERT_FALSE(_gameCtx->isChildContextJoinable());

    // This will force the child context to run what's basically a busy wait loop...
    // It's only a test, I don't care.
    ASSERT_NO_THROW(_gameCtx->startChildContext(-1));

    ASSERT_TRUE(_gameCtx->isChildContextJoinable());
    ASSERT_EQ(_gameCtx->stopAndJoinChildContext(), 0);

    _gameCtx->detachChildContext();
    ASSERT_FALSE(_gameCtx->hasChildContext());

    ASSERT_THROW(
        _gameCtx->isChildContextJoinable(),
        hg::TracedLogicError
    );
}

///////////////////////////////////////////////////////////////////////////
// SYNCHRONIZATION TEST                                                  //
///////////////////////////////////////////////////////////////////////////

namespace {

class SPeMPE_SynchronizedTest : public ::testing::Test {
protected:
    void SetUp() override {
        hg::RN_IndexHandlers();
    }

    std::optional<GameContext> _serverCtx{GameContext::RuntimeConfig{}};
    std::optional<GameContext> _clientCtx{GameContext::RuntimeConfig{}};
};

//! "Dropped" when an Avatar object dies.
class AvatarDrop : public NonstateObject {
public:
    AvatarDrop(hg::QAO_RuntimeRef aRuntimeRef)
        : NonstateObject(aRuntimeRef, SPEMPE_TYPEID_SELF, 0, "AvatarDrop")
    {
    }

    int customData = 0x1337;
};

struct Avatar_VisibleState {
    constexpr static int CD_INITIAL = 0;

    int customData = CD_INITIAL;
    HG_ENABLE_AUTOPACK(Avatar_VisibleState, customData);
};

class Avatar : public SynchronizedObject<Avatar_VisibleState> {
public:
    Avatar(hg::QAO_RuntimeRef aRuntimeRef,
           RegistryId aRegId,
           SyncId aSyncId)
        : SyncObjSuper{aRuntimeRef, SPEMPE_TYPEID_SELF, 0, "Avatar", aRegId, aSyncId}
    {
    }

    ~Avatar() override {
        if (isMasterObject()) {
            doSyncDestroy();
        }

        hg::QAO_PCreate<AvatarDrop>(getRuntime())->customData = _getCurrentState().customData;
    }

    int getCustomData() const {
        return _getCurrentState().customData;
    }

    void setCustomData(int aCustomData) {
        assert(isMasterObject());
        _getCurrentState().customData = aCustomData;
    }

private:
    void _syncCreateImpl(SyncControlDelegate& aSyncCtrl) const override;
    void _syncUpdateImpl(SyncControlDelegate& aSyncCtrl) const override;
    void _syncDestroyImpl(SyncControlDelegate& aSyncCtrl) const override;
};

SPEMPE_GENERATE_DEFAULT_SYNC_HANDLERS(Avatar, (CREATE, UPDATE, DESTROY));

void Avatar::_syncCreateImpl(SyncControlDelegate& aSyncCtrl) const {
    SPEMPE_SYNC_CREATE_DEFAULT_IMPL(Avatar, aSyncCtrl);
}

void Avatar::_syncUpdateImpl(SyncControlDelegate& aSyncCtrl) const {
    SPEMPE_SYNC_UPDATE_DEFAULT_IMPL(Avatar, aSyncCtrl);
}

void Avatar::_syncDestroyImpl(SyncControlDelegate& aSyncCtrl) const {
    SPEMPE_SYNC_DESTROY_DEFAULT_IMPL(Avatar, aSyncCtrl);
}

using MNetworking = NetworkingManagerInterface;

} // namespace

TEST_F(SPeMPE_SynchronizedTest, BasicFunctionalityTest) {
    {
        SCOPED_TRACE("Configure contexts");

        using namespace hobgoblin::rn;

        constexpr static int BUFFERING_LENGTH = 0;

        // Server context:
        _serverCtx->setToMode(GameContext::Mode::Server);

        auto netwMgr1 = std::make_unique<DefaultNetworkingManager>(_serverCtx->getQAORuntime().nonOwning(), 
                                                                   0, BUFFERING_LENGTH);
        netwMgr1->setToServerMode(RN_Protocol::UDP, "pass", 2, 512, RN_NetworkingStack::Default);
        _serverCtx->attachAndOwnComponent(std::move(netwMgr1));

        // Client context:
        _clientCtx->setToMode(GameContext::Mode::Client);

        auto netwMgr2 = std::make_unique<DefaultNetworkingManager>(_clientCtx->getQAORuntime().nonOwning(), 
                                                                   0, BUFFERING_LENGTH);
        netwMgr2->setToClientMode(RN_Protocol::UDP, "pass", 512, RN_NetworkingStack::Default);
        _clientCtx->attachAndOwnComponent(std::move(netwMgr2));
    }
    {
        SCOPED_TRACE("Establish conection between contexts");

        ASSERT_TRUE(_serverCtx->getComponent<MNetworking>().isServer());
        ASSERT_TRUE(_clientCtx->getComponent<MNetworking>().isClient());

        auto& server = _serverCtx->getComponent<MNetworking>().getServer();
        auto& client = _clientCtx->getComponent<MNetworking>().getClient();
        
        server.setUserData(static_cast<GameContext*>(&*_serverCtx));
        server.start(0);

        client.setUserData(static_cast<GameContext*>(&*_clientCtx));
        client.connectLocal(server);

        _serverCtx->runFor(1);
        _clientCtx->runFor(1);
        _serverCtx->runFor(1);
        _clientCtx->runFor(1);

        ASSERT_EQ(server.getClientConnector(0).getStatus(), hg::RN_ConnectorStatus::Connected);
        ASSERT_EQ(client.getServerConnector().getStatus(),  hg::RN_ConnectorStatus::Connected);
    }
    {
        SCOPED_TRACE("Add a synchronized Avatar instance to server");

        hg::QAO_PCreate<Avatar>(&_serverCtx->getQAORuntime(), 
                                _serverCtx->getComponent<MNetworking>().getRegistryId(),
                                SYNC_ID_NEW);

        _serverCtx->runFor(1);
        _clientCtx->runFor(1);

        auto* dummy = dynamic_cast<Avatar*>(_clientCtx->getQAORuntime().find("Avatar"));
        ASSERT_NE(dummy, nullptr);
        EXPECT_EQ(dummy->getCustomData(), Avatar::VisibleState::CD_INITIAL);
    }
    {
        SCOPED_TRACE("Check state updates");

        auto* master = dynamic_cast<Avatar*>(_serverCtx->getQAORuntime().find("Avatar"));
        ASSERT_NE(master, nullptr);
        master->setCustomData(Avatar::VisibleState::CD_INITIAL + 5);

        _serverCtx->runFor(1);
        _clientCtx->runFor(1);

        auto* dummy = dynamic_cast<Avatar*>(_clientCtx->getQAORuntime().find("Avatar"));
        ASSERT_NE(dummy, nullptr);
        EXPECT_EQ(dummy->getCustomData(), Avatar::VisibleState::CD_INITIAL + 5);
    }
    {
        SCOPED_TRACE("Check destruction");

        auto* master = dynamic_cast<Avatar*>(_serverCtx->getQAORuntime().find("Avatar"));
        ASSERT_NE(master, nullptr);
        hg::QAO_PDestroy(master);

        _serverCtx->runFor(1);
        _clientCtx->runFor(1);

        auto* dummy = dynamic_cast<Avatar*>(_clientCtx->getQAORuntime().find("Avatar"));
        ASSERT_EQ(dummy, nullptr);

        // Clean up drops:
        while (auto* drop = _serverCtx->getQAORuntime().find("AvatarDrop")) {
            hg::QAO_PDestroy(drop);
        }
        while (auto* drop = _clientCtx->getQAORuntime().find("AvatarDrop")) {
            hg::QAO_PDestroy(drop);
        }
    }
    {
        SCOPED_TRACE("Check instant sync. obj. destruction after creation");

        auto* obj = hg::QAO_PCreate<Avatar>(&_serverCtx->getQAORuntime(), 
                                            _serverCtx->getComponent<MNetworking>().getRegistryId(),
                                            SYNC_ID_NEW);
        obj->setCustomData(Avatar::VisibleState::CD_INITIAL + 12);
        hg::QAO_PDestroy(obj);

        _serverCtx->runFor(1);
        _clientCtx->runFor(1);

        auto* clDrop = dynamic_cast<AvatarDrop*>(_clientCtx->getQAORuntime().find("AvatarDrop"));
        ASSERT_NE(clDrop, nullptr);
        EXPECT_EQ(clDrop->customData, Avatar::VisibleState::CD_INITIAL + 12);

        // Clean up drops:
        while (auto* drop = _serverCtx->getQAORuntime().find("AvatarDrop")) {
            hg::QAO_PDestroy(drop);
        }
        while (auto* drop = _clientCtx->getQAORuntime().find("AvatarDrop")) {
            hg::QAO_PDestroy(drop);
        }
    }
    {
        SCOPED_TRACE("Clean up client context");
        _clientCtx.reset();
    }
    {
        SCOPED_TRACE("Clean up server context");
        _serverCtx.reset();
    }
}

// ============================================================================================= //

struct DeactivatingAvatar_VisibleState {
    constexpr static int CD_INITIAL = 0;

    int customData = CD_INITIAL;
    HG_ENABLE_AUTOPACK(DeactivatingAvatar_VisibleState, customData);
};

class DeactivatingAvatar : public SynchronizedObject<DeactivatingAvatar_VisibleState> {
public:
    DeactivatingAvatar(hg::QAO_RuntimeRef aRuntimeRef,
                       RegistryId aRegId,
                       SyncId aSyncId)
        : SyncObjSuper{aRuntimeRef, SPEMPE_TYPEID_SELF, 0, "DeactivatingAvatar", aRegId, aSyncId}
    {
    }

    ~DeactivatingAvatar() override {
        if (isMasterObject()) {
            doSyncDestroy();
        }
    }

    int getCustomData() const {
        return _getCurrentState().customData;
    }

    void setCustomData(int aCustomData) {
        assert(isMasterObject());
        _getCurrentState().customData = aCustomData;
    }

    SyncFilterStatus syncInstruction = SyncFilterStatus::REGULAR_SYNC;

private:
    void _syncCreateImpl(SyncControlDelegate& aSyncCtrl) const override;
    void _syncUpdateImpl(SyncControlDelegate& aSyncCtrl) const override;
    void _syncDestroyImpl(SyncControlDelegate& aSyncCtrl) const override;
};

SPEMPE_GENERATE_DEFAULT_SYNC_HANDLERS(DeactivatingAvatar, (CREATE, UPDATE, DESTROY));

void DeactivatingAvatar::_syncCreateImpl(SyncControlDelegate& aSyncCtrl) const {
    SPEMPE_SYNC_CREATE_DEFAULT_IMPL(DeactivatingAvatar, aSyncCtrl);
}

void DeactivatingAvatar::_syncUpdateImpl(SyncControlDelegate& aSyncCtrl) const {
    aSyncCtrl.filter([this](hg::PZInteger /*aRecepient*/) -> SyncFilterStatus {
                         return this->syncInstruction;
                     });
    SPEMPE_SYNC_UPDATE_DEFAULT_IMPL(DeactivatingAvatar, aSyncCtrl);
}

void DeactivatingAvatar::_syncDestroyImpl(SyncControlDelegate& aSyncCtrl) const {
    SPEMPE_SYNC_DESTROY_DEFAULT_IMPL(DeactivatingAvatar, aSyncCtrl);
}

TEST_F(SPeMPE_SynchronizedTest, DeactivationTest) {
    using namespace hobgoblin::rn;
    {
        SCOPED_TRACE("Configure contexts");

        constexpr static int BUFFERING_LENGTH = 0;

        // Server context:
        _serverCtx->setToMode(GameContext::Mode::Server);

        auto netwMgr1 = std::make_unique<DefaultNetworkingManager>(_serverCtx->getQAORuntime().nonOwning(),
                                                                   0, BUFFERING_LENGTH);
        netwMgr1->setToServerMode(RN_Protocol::UDP, "pass", 2, 512, RN_NetworkingStack::Default);
        _serverCtx->attachAndOwnComponent(std::move(netwMgr1));

        // Client context:
        _clientCtx->setToMode(GameContext::Mode::Client);

        auto netwMgr2 = std::make_unique<DefaultNetworkingManager>(_clientCtx->getQAORuntime().nonOwning(),
                                                                   0, BUFFERING_LENGTH);
        netwMgr2->setToClientMode(RN_Protocol::UDP, "pass", 512, RN_NetworkingStack::Default);
        _clientCtx->attachAndOwnComponent(std::move(netwMgr2));
    }
    {
        SCOPED_TRACE("Establish conection between contexts");

        ASSERT_TRUE(_serverCtx->getComponent<MNetworking>().isServer());
        ASSERT_TRUE(_clientCtx->getComponent<MNetworking>().isClient());

        auto& server = _serverCtx->getComponent<MNetworking>().getServer();
        auto& client = _clientCtx->getComponent<MNetworking>().getClient();

        server.setUserData(static_cast<GameContext*>(&*_serverCtx));
        server.start(0);

        client.setUserData(static_cast<GameContext*>(&*_clientCtx));
        client.connectLocal(server);

        _serverCtx->runFor(1);
        _clientCtx->runFor(1);
        _serverCtx->runFor(1);
        _clientCtx->runFor(1);

        ASSERT_EQ(server.getClientConnector(0).getStatus(), RN_ConnectorStatus::Connected);
        ASSERT_EQ(client.getServerConnector().getStatus(),  RN_ConnectorStatus::Connected);
    }
    {
        SCOPED_TRACE("Add a synchronized DeactivatingAvatar instance to server");

        hg::QAO_PCreate<DeactivatingAvatar>(&_serverCtx->getQAORuntime(),
                                            _serverCtx->getComponent<MNetworking>().getRegistryId(),
                                            SYNC_ID_NEW);

        _serverCtx->runFor(1);
        _clientCtx->runFor(1);

        auto* dummy = dynamic_cast<DeactivatingAvatar*>(_clientCtx->getQAORuntime().find("DeactivatingAvatar"));
        ASSERT_NE(dummy, nullptr);
        EXPECT_EQ(dummy->getCustomData(), DeactivatingAvatar::VisibleState::CD_INITIAL);
    }
    {
        SCOPED_TRACE("Check state updates : REGULAR_SYNC (1)");

        auto* master = dynamic_cast<DeactivatingAvatar*>(_serverCtx->getQAORuntime().find("DeactivatingAvatar"));
        ASSERT_NE(master, nullptr);
        master->setCustomData(DeactivatingAvatar::VisibleState::CD_INITIAL + 5);

        _serverCtx->runFor(1);
        _clientCtx->runFor(1);

        auto* dummy = dynamic_cast<DeactivatingAvatar*>(_clientCtx->getQAORuntime().find("DeactivatingAvatar"));
        ASSERT_NE(dummy, nullptr);
        EXPECT_EQ(dummy->getCustomData(), DeactivatingAvatar::VisibleState::CD_INITIAL + 5);
    }
    {
        SCOPED_TRACE("Check state updates : SKIP (1)");

        auto* master = dynamic_cast<DeactivatingAvatar*>(_serverCtx->getQAORuntime().find("DeactivatingAvatar"));
        ASSERT_NE(master, nullptr);
        master->setCustomData(DeactivatingAvatar::VisibleState::CD_INITIAL + 10);
        master->syncInstruction = SyncFilterStatus::SKIP;

        _serverCtx->runFor(1);
        _clientCtx->runFor(1);

        auto* dummy = dynamic_cast<DeactivatingAvatar*>(_clientCtx->getQAORuntime().find("DeactivatingAvatar"));
        ASSERT_NE(dummy, nullptr);
        EXPECT_EQ(dummy->getCustomData(), DeactivatingAvatar::VisibleState::CD_INITIAL + 5); // old = CD_INITIAL + 5
    }
    {
        SCOPED_TRACE("Check state updates : DEACTIVATE (1)");

        auto* master = dynamic_cast<DeactivatingAvatar*>(_serverCtx->getQAORuntime().find("DeactivatingAvatar"));
        ASSERT_NE(master, nullptr);
        master->setCustomData(DeactivatingAvatar::VisibleState::CD_INITIAL + 15);
        master->syncInstruction = SyncFilterStatus::DEACTIVATE;

        _serverCtx->runFor(1);
        _clientCtx->runFor(1);

        auto* dummy = dynamic_cast<DeactivatingAvatar*>(_clientCtx->getQAORuntime().find("DeactivatingAvatar"));
        ASSERT_NE(dummy, nullptr);
        EXPECT_TRUE(dummy->isDeactivated()); 
    }
    {
        SCOPED_TRACE("Check state updates : DEACTIVATE (2)");

        auto* master = dynamic_cast<DeactivatingAvatar*>(_serverCtx->getQAORuntime().find("DeactivatingAvatar"));
        ASSERT_NE(master, nullptr);
        master->setCustomData(DeactivatingAvatar::VisibleState::CD_INITIAL + 20);
        master->syncInstruction = SyncFilterStatus::DEACTIVATE;

        _serverCtx->runFor(1);
        _clientCtx->runFor(1);

        auto* dummy = dynamic_cast<DeactivatingAvatar*>(_clientCtx->getQAORuntime().find("DeactivatingAvatar"));
        ASSERT_NE(dummy, nullptr);
        EXPECT_TRUE(dummy->isDeactivated());
    }
    {
        SCOPED_TRACE("Check state updates : SKIP (2)");

        auto* master = dynamic_cast<DeactivatingAvatar*>(_serverCtx->getQAORuntime().find("DeactivatingAvatar"));
        ASSERT_NE(master, nullptr);
        master->setCustomData(DeactivatingAvatar::VisibleState::CD_INITIAL + 25);
        master->syncInstruction = SyncFilterStatus::SKIP;

        _serverCtx->runFor(1);
        _clientCtx->runFor(1);

        auto* dummy = dynamic_cast<DeactivatingAvatar*>(_clientCtx->getQAORuntime().find("DeactivatingAvatar"));
        ASSERT_NE(dummy, nullptr);
        EXPECT_TRUE(dummy->isDeactivated());
    }
    {
        SCOPED_TRACE("Check state updates : REGULAR_SYNC (2)");

        auto* master = dynamic_cast<DeactivatingAvatar*>(_serverCtx->getQAORuntime().find("DeactivatingAvatar"));
        ASSERT_NE(master, nullptr);
        master->setCustomData(DeactivatingAvatar::VisibleState::CD_INITIAL + 30);
        master->syncInstruction = SyncFilterStatus::REGULAR_SYNC;

        _serverCtx->runFor(1);
        _clientCtx->runFor(1);

        auto* dummy = dynamic_cast<DeactivatingAvatar*>(_clientCtx->getQAORuntime().find("DeactivatingAvatar"));
        ASSERT_NE(dummy, nullptr);
        ASSERT_FALSE(dummy->isDeactivated());
        EXPECT_EQ(dummy->getCustomData(), DeactivatingAvatar::VisibleState::CD_INITIAL + 30);
    }
    {
        SCOPED_TRACE("Check destruction");

        auto* master = dynamic_cast<DeactivatingAvatar*>(_serverCtx->getQAORuntime().find("DeactivatingAvatar"));
        ASSERT_NE(master, nullptr);
        hg::QAO_PDestroy(master);

        _serverCtx->runFor(1);
        _clientCtx->runFor(1);

        auto* dummy = dynamic_cast<DeactivatingAvatar*>(_clientCtx->getQAORuntime().find("DeactivatingAvatar"));
        ASSERT_EQ(dummy, nullptr);
    }
    {
        SCOPED_TRACE("Clean up client context");
        _clientCtx.reset();
    }
    {
        SCOPED_TRACE("Clean up server context");
        _serverCtx.reset();
    }
}

class SPeMPE_ParametrizedSynchronizedTest : public ::testing::TestWithParam<int> {
protected:
    void SetUp() override {
        hg::RN_IndexHandlers();
    }

    std::optional<GameContext> _serverCtx{GameContext::RuntimeConfig{}};
    std::optional<GameContext> _clientCtx{GameContext::RuntimeConfig{}};
};

SPEMPE_DEFINE_AUTODIFF_STATE(AutodiffDeactivatingAvatar_VisibleState,
    SPEMPE_MEMBER(std::int32_t, i0, 1234),
    SPEMPE_MEMBER(std::int32_t, i1, 1234)
) {
    static constexpr std::int32_t MEMBER_DEFAULT_VALUE = 1234;
};

class AutodiffDeactivatingAvatar
    : public SynchronizedObject<AutodiffDeactivatingAvatar_VisibleState> {
public:
    AutodiffDeactivatingAvatar(hg::QAO_RuntimeRef aRuntimeRef,
                               RegistryId aRegId,
                               SyncId aSyncId)
        : SyncObjSuper{aRuntimeRef, SPEMPE_TYPEID_SELF, 0, "AutodiffDeactivatingAvatar", aRegId, aSyncId}
    {
        if (isMasterObject()) {
            _getCurrentState().initMirror();
        }
    }

    ~AutodiffDeactivatingAvatar() override {
        if (isMasterObject()) {
            doSyncDestroy();
        }
    }

    int getI0() const {
        return _getCurrentState().i0;
    }

    int getI1() const {
        return _getCurrentState().i1;
    }

    void setI0(std::int32_t aValue) {
        _getCurrentState().i0 = aValue;
    }

    void setI1(std::int32_t aValue) {
        _getCurrentState().i1 = aValue;
    }

    void _eventPostUpdate(IfMaster) override {
        _getCurrentState().commit();
    }

    SyncFilterStatus syncInstruction = SyncFilterStatus::REGULAR_SYNC;

private:
    void _syncCreateImpl(SyncControlDelegate& aSyncCtrl) const override;
    void _syncUpdateImpl(SyncControlDelegate& aSyncCtrl) const override;
    void _syncDestroyImpl(SyncControlDelegate& aSyncCtrl) const override;
};

SPEMPE_GENERATE_DEFAULT_SYNC_HANDLERS(AutodiffDeactivatingAvatar, (CREATE, UPDATE, DESTROY));

void AutodiffDeactivatingAvatar::_syncCreateImpl(SyncControlDelegate& aSyncCtrl) const {
    SPEMPE_SYNC_CREATE_DEFAULT_IMPL(AutodiffDeactivatingAvatar, aSyncCtrl);
}

void AutodiffDeactivatingAvatar::_syncUpdateImpl(SyncControlDelegate& aSyncCtrl) const {
    aSyncCtrl.filter([this](hg::PZInteger /*aRecepient*/) -> SyncFilterStatus {
        return this->syncInstruction;
    });
    SPEMPE_SYNC_UPDATE_DEFAULT_IMPL(AutodiffDeactivatingAvatar, aSyncCtrl);
}

void AutodiffDeactivatingAvatar::_syncDestroyImpl(SyncControlDelegate& aSyncCtrl) const {
    SPEMPE_SYNC_DESTROY_DEFAULT_IMPL(AutodiffDeactivatingAvatar, aSyncCtrl);
}

TEST_P(SPeMPE_ParametrizedSynchronizedTest, DeactivationWithAutodiffStateTest) {
    using namespace hobgoblin::rn;
    {
        SCOPED_TRACE("Configure contexts");

        constexpr static int BUFFERING_LENGTH = 0;

        // Server context:
        _serverCtx->setToMode(GameContext::Mode::Server);

        auto netwMgr1 = std::make_unique<DefaultNetworkingManager>(_serverCtx->getQAORuntime().nonOwning(),
                                                                0, BUFFERING_LENGTH);
        netwMgr1->setToServerMode(RN_Protocol::UDP, "pass", 2, 512, RN_NetworkingStack::Default);
        _serverCtx->attachAndOwnComponent(std::move(netwMgr1));

        // Client context:
        _clientCtx->setToMode(GameContext::Mode::Client);

        auto netwMgr2 = std::make_unique<DefaultNetworkingManager>(_clientCtx->getQAORuntime().nonOwning(),
                                                                0, BUFFERING_LENGTH);
        netwMgr2->setToClientMode(RN_Protocol::UDP, "pass", 512, RN_NetworkingStack::Default);
        _clientCtx->attachAndOwnComponent(std::move(netwMgr2));
    }
    {
        SCOPED_TRACE("Establish conection between contexts");

        ASSERT_TRUE(_serverCtx->getComponent<MNetworking>().isServer());
        ASSERT_TRUE(_clientCtx->getComponent<MNetworking>().isClient());

        auto& server = _serverCtx->getComponent<MNetworking>().getServer();
        auto& client = _clientCtx->getComponent<MNetworking>().getClient();

        server.setUserData(static_cast<GameContext*>(&*_serverCtx));
        server.start(0);

        client.setUserData(static_cast<GameContext*>(&*_clientCtx));
        client.connectLocal(server);

        _serverCtx->runFor(1);
        _clientCtx->runFor(1);
        _serverCtx->runFor(1);
        _clientCtx->runFor(1);

        ASSERT_EQ(server.getClientConnector(0).getStatus(), RN_ConnectorStatus::Connected);
        ASSERT_EQ(client.getServerConnector().getStatus(),  RN_ConnectorStatus::Connected);
    }
    {
        SCOPED_TRACE("Add a synchronized AutodiffDeactivatingAvatar instance to server");

        hg::QAO_PCreate<AutodiffDeactivatingAvatar>(&_serverCtx->getQAORuntime(),
                                                    _serverCtx->getComponent<MNetworking>().getRegistryId(),
                                                    SYNC_ID_NEW);

        _serverCtx->runFor(1);
        _clientCtx->runFor(1);

        auto* dummy = dynamic_cast<AutodiffDeactivatingAvatar*>(
            _clientCtx->getQAORuntime().find("AutodiffDeactivatingAvatar")
        );
        ASSERT_NE(dummy, nullptr);
        EXPECT_EQ(dummy->getI0(), AutodiffDeactivatingAvatar::VisibleState::MEMBER_DEFAULT_VALUE);
        EXPECT_EQ(dummy->getI1(), AutodiffDeactivatingAvatar::VisibleState::MEMBER_DEFAULT_VALUE);
    }
    {
        SCOPED_TRACE("Deactivate avatar so state update is not sent");

        auto* master = dynamic_cast<AutodiffDeactivatingAvatar*>(
            _serverCtx->getQAORuntime().find("AutodiffDeactivatingAvatar")
        );
        ASSERT_NE(master, nullptr);
        master->setI1(8888);
        master->syncInstruction = SyncFilterStatus::DEACTIVATE;

        _serverCtx->runFor(1);
        _clientCtx->runFor(1);

        auto* dummy = dynamic_cast<AutodiffDeactivatingAvatar*>(
            _clientCtx->getQAORuntime().find("AutodiffDeactivatingAvatar")
        );
        ASSERT_NE(dummy, nullptr);
        EXPECT_TRUE(dummy->isDeactivated()); 
    }
    {
        SCOPED_TRACE("Run the contexts a few more times");
        const auto emptyFrameCount = GetParam();
        for (int i = 0; i < emptyFrameCount; i += 1) {
            _serverCtx->runFor(1);
            _clientCtx->runFor(1);
        }
    }
    {
        SCOPED_TRACE("Check that state is synced correctly after reactivation");

        auto* master = dynamic_cast<AutodiffDeactivatingAvatar*>(
            _serverCtx->getQAORuntime().find("AutodiffDeactivatingAvatar")
        );
        ASSERT_NE(master, nullptr);
        master->syncInstruction = SyncFilterStatus::REGULAR_SYNC;

        _serverCtx->runFor(1);
        _clientCtx->runFor(1);

        auto* dummy = dynamic_cast<AutodiffDeactivatingAvatar*>(
            _clientCtx->getQAORuntime().find("AutodiffDeactivatingAvatar")
        );
        ASSERT_NE(dummy, nullptr);
        ASSERT_FALSE(dummy->isDeactivated());
        EXPECT_EQ(dummy->getI0(), AutodiffDeactivatingAvatar::VisibleState::MEMBER_DEFAULT_VALUE);
        EXPECT_EQ(dummy->getI1(), 8888);
    }
}

INSTANTIATE_TEST_SUITE_P(
    DeactivationWithAutodiffStateTest,
    SPeMPE_ParametrizedSynchronizedTest,
    ::testing::Values(0, 1, 2, 3, 4)
);

} // namespace spempe
} // namespace jbatnozic
