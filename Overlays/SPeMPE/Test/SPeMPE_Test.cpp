
#include <SPeMPE/SPeMPE.hpp>

#include <Hobgoblin/Utility/Autopack.hpp>

#include <gtest/gtest.h>

#include <cstdint>
#include <memory>
#include <optional>
#include <utility>

namespace jbatnozic {
namespace spempe {

class SPeMPE_Test : public ::testing::Test {
protected:
    void SetUp() override {
        hg::RN_IndexHandlers();
    }

    std::optional<GameContext> _gameCtx{GameContext::RuntimeConfig{}};
};

namespace {
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
           SynchronizedObjectRegistry& aSyncObjReg,
           SyncId aSyncId)
        : SyncObjSuper{aRuntimeRef, SPEMPE_TYPEID_SELF, 0, "Avatar", aSyncObjReg, aSyncId}
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

protected:
    // Make sure that state scheduling and company work properly even when eventUpdate()
    // is overriden, as long as SPEMPE_SYNCOBJ_BEGIN_EVENT_UPDATE_OVERRIDE() is called 
    // at the start:
    void eventUpdate() override {
        SPEMPE_SYNCOBJ_BEGIN_EVENT_UPDATE_OVERRIDE();

        // Make sure nothing is executed after destruction:
        // (SPEMPE_SYNCOBJ_BEGIN_EVENT_UPDATE_OVERRIDE() returns from current method
        // if it deletes self.)
        setName(getName());
    }

private:
    void _syncCreateImpl(hg::RN_NodeInterface& aNode,
                         std::vector<hg::PZInteger>& aRecepients) const override;

    void _syncUpdateImpl(hg::RN_NodeInterface& aNode,
                         std::vector<hg::PZInteger>& aRecepients) const override;

    void _syncDestroyImpl(hg::RN_NodeInterface& aNode,
                          std::vector<hg::PZInteger>& aRecepients) const override;
};

SPEMPE_GENERATE_DEFAULT_SYNC_HANDLERS(Avatar, (CREATE, UPDATE, DESTROY));

void Avatar::_syncCreateImpl(hg::RN_NodeInterface& aNode,
                             std::vector<hg::PZInteger>& aRecepients) const {
    SPEMPE_SYNC_CREATE_DEFAULT_IMPL(Avatar, aNode, aRecepients);
}

void Avatar::_syncUpdateImpl(hg::RN_NodeInterface& aNode,
                             std::vector<hg::PZInteger>& aRecepients) const {
    SPEMPE_SYNC_UPDATE_DEFAULT_IMPL(Avatar, aNode, aRecepients);
}

void Avatar::_syncDestroyImpl(hg::RN_NodeInterface& aNode,
                              std::vector<hg::PZInteger>& aRecepients) const {
    SPEMPE_SYNC_DESTROY_DEFAULT_IMPL(Avatar, aNode, aRecepients);
}

using MNetworking = NetworkingManagerInterface;

} // namespace

TEST_F(SPeMPE_Test, SynchronizationTest) {
    std::optional<GameContext> clientCtx{GameContext::RuntimeConfig{}};

    {
        SCOPED_TRACE("Configure contexts");

        // Server context:
        _gameCtx->setToMode(GameContext::Mode::Server);

        auto netwMgr1 = std::make_unique<NetworkingManagerOne>(_gameCtx->getQAORuntime().nonOwning(), 0);
        netwMgr1->setToMode(MNetworking::Mode::Server);
        _gameCtx->attachAndOwnComponent(std::move(netwMgr1));

        // Client context:
        clientCtx->setToMode(GameContext::Mode::Client);

        auto netwMgr2 = std::make_unique<NetworkingManagerOne>(clientCtx->getQAORuntime().nonOwning(), 0);
        netwMgr2->setToMode(MNetworking::Mode::Client);
        clientCtx->attachAndOwnComponent(std::move(netwMgr2));
    }
    {
        SCOPED_TRACE("Establish conection between contexts");

        ASSERT_TRUE(_gameCtx->getComponent<MNetworking>().isServer());
        ASSERT_TRUE(clientCtx->getComponent<MNetworking>().isClient());

        auto& server = _gameCtx->getComponent<MNetworking>().getServer();
        auto& client = clientCtx->getComponent<MNetworking>().getClient();
        
        server.setUserData(static_cast<GameContext*>(&*_gameCtx));
        server.start(0);

        client.setUserData(static_cast<GameContext*>(&*clientCtx));
        client.connectLocal(server);

        _gameCtx->runFor(1);
        clientCtx->runFor(1);
    }
    {
        SCOPED_TRACE("Add a synchronized Avatar instance to server");

        hg::QAO_PCreate<Avatar>(&_gameCtx->getQAORuntime(), 
                                _gameCtx->getComponent<MNetworking>().getSyncObjReg(),
                                SYNC_ID_NEW);

        _gameCtx->runFor(1);
        clientCtx->runFor(1);

        auto* dummy = dynamic_cast<Avatar*>(clientCtx->getQAORuntime().find("Avatar"));
        ASSERT_NE(dummy, nullptr);
        EXPECT_EQ(dummy->getCustomData(), Avatar::VisibleState::CD_INITIAL);
    }
    {
        SCOPED_TRACE("Check state updates");

        auto* master = dynamic_cast<Avatar*>(_gameCtx->getQAORuntime().find("Avatar"));
        ASSERT_NE(master, nullptr);
        master->setCustomData(Avatar::VisibleState::CD_INITIAL + 5);

        _gameCtx->runFor(1);
        clientCtx->runFor(1);

        auto* dummy = dynamic_cast<Avatar*>(clientCtx->getQAORuntime().find("Avatar"));
        ASSERT_NE(dummy, nullptr);
        EXPECT_EQ(dummy->getCustomData(), Avatar::VisibleState::CD_INITIAL + 5);
    }
    {
        SCOPED_TRACE("Check destruction");

        auto* master = dynamic_cast<Avatar*>(_gameCtx->getQAORuntime().find("Avatar"));
        ASSERT_NE(master, nullptr);
        hg::QAO_PDestroy(master);

        _gameCtx->runFor(1);
        clientCtx->runFor(1);

        auto* dummy = dynamic_cast<Avatar*>(clientCtx->getQAORuntime().find("Avatar"));
        ASSERT_EQ(dummy, nullptr);

        // Clean up drops:
        while (auto* drop = _gameCtx->getQAORuntime().find("AvatarDrop")) {
            hg::QAO_PDestroy(drop);
        }
        while (auto* drop = clientCtx->getQAORuntime().find("AvatarDrop")) {
            hg::QAO_PDestroy(drop);
        }
    }
    {
        SCOPED_TRACE("Check instant sync. obj. destruction after creation");

        auto* obj = hg::QAO_PCreate<Avatar>(&_gameCtx->getQAORuntime(), 
                                            _gameCtx->getComponent<MNetworking>().getSyncObjReg(),
                                            SYNC_ID_NEW);
        obj->setCustomData(Avatar::VisibleState::CD_INITIAL + 12);
        hg::QAO_PDestroy(obj);

        _gameCtx->runFor(1);
        clientCtx->runFor(1);

        auto* clDrop = dynamic_cast<AvatarDrop*>(clientCtx->getQAORuntime().find("AvatarDrop"));
        ASSERT_NE(clDrop, nullptr);
        EXPECT_EQ(clDrop->customData, Avatar::VisibleState::CD_INITIAL + 12);

        // Clean up drops:
        while (auto* drop = _gameCtx->getQAORuntime().find("AvatarDrop")) {
            hg::QAO_PDestroy(drop);
        }
        while (auto* drop = clientCtx->getQAORuntime().find("AvatarDrop")) {
            hg::QAO_PDestroy(drop);
        }
    }
    {
        SCOPED_TRACE("Clean up client context");
        clientCtx.reset();
    }
    {
        SCOPED_TRACE("Clean up server context");
        _gameCtx.reset();
    }
}

} // namespace spempe
} // namespace jbatnozic