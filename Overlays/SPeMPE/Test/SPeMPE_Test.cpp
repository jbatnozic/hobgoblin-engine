
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
    // Make sure that state scheduling and company work properly even when _eventUpdate()
    // is overriden, as long as SPEMPE_SYNCOBJ_BEGIN_EVENT_UPDATE_OVERRIDE() is called 
    // at the start:
    void _eventUpdate() override {
        SPEMPE_SYNCOBJ_BEGIN_EVENT_UPDATE_OVERRIDE();

        // Make sure nothing is executed after destruction:
        // (SPEMPE_SYNCOBJ_BEGIN_EVENT_UPDATE_OVERRIDE() returns from current method
        // if it deletes self.)
        setName(getName());
    }

    void _syncCreateImpl(SyncDetails& aSyncDetails) const override;
    void _syncUpdateImpl(SyncDetails& aSyncDetails) const override;
    void _syncDestroyImpl(SyncDetails& aSyncDetails) const override;
};

SPEMPE_GENERATE_DEFAULT_SYNC_HANDLERS(Avatar, (CREATE, UPDATE, DESTROY));

void Avatar::_syncCreateImpl(SyncDetails& aSyncDetails) const {
    SPEMPE_SYNC_CREATE_DEFAULT_IMPL(Avatar, aSyncDetails);
}

void Avatar::_syncUpdateImpl(SyncDetails& aSyncDetails) const {
    SPEMPE_SYNC_UPDATE_DEFAULT_IMPL(Avatar, aSyncDetails);
}

void Avatar::_syncDestroyImpl(SyncDetails& aSyncDetails) const {
    SPEMPE_SYNC_DESTROY_DEFAULT_IMPL(Avatar, aSyncDetails);
}

using MNetworking = NetworkingManagerInterface;

} // namespace

TEST_F(SPeMPE_SynchronizedTest, BasicFunctionalityTest) {
    {
        SCOPED_TRACE("Configure contexts");

        constexpr static int BUFFERING_LENGTH = 0;

        // Server context:
        _serverCtx->setToMode(GameContext::Mode::Server);

        auto netwMgr1 = std::make_unique<NetworkingManagerOne>(_serverCtx->getQAORuntime().nonOwning(), 
                                                               0, BUFFERING_LENGTH);
        netwMgr1->setToMode(MNetworking::Mode::Server);
        _serverCtx->attachAndOwnComponent(std::move(netwMgr1));

        // Client context:
        _clientCtx->setToMode(GameContext::Mode::Client);

        auto netwMgr2 = std::make_unique<NetworkingManagerOne>(_clientCtx->getQAORuntime().nonOwning(), 
                                                               0, BUFFERING_LENGTH);
        netwMgr2->setToMode(MNetworking::Mode::Client);
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

} // namespace spempe
} // namespace jbatnozic