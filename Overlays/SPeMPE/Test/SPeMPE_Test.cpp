
#include <SPeMPE/SPeMPE.hpp>

#include <gtest/gtest.h>

#include <cstdint>
#include <memory>
#include <utility>

namespace jbatnozic {
namespace spempe {

class SPeMPE_Test : public ::testing::Test {
protected:
    GameContext _gameCtx{{}};
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

        _gameCtx.attachComponent(dummy);
        ASSERT_EQ(_gameCtx.getComponent<DummyInterface>().getData(), 0x12345678);

        _gameCtx.detachComponent(dummy);
        ASSERT_THROW(_gameCtx.getComponent<DummyInterface>(), hg::TracedLogicError);
    }
    {
        SCOPED_TRACE("Context owns component");

        auto dummy = std::make_unique<DummyComponent>();
        dummy->data = 0x12345678;

        _gameCtx.attachAndOwnComponent(std::move(dummy));
        ASSERT_EQ(_gameCtx.getComponent<DummyInterface>().getData(), 0x12345678);

        _gameCtx.detachComponent(_gameCtx.getComponent<DummyInterface>());
        ASSERT_THROW(_gameCtx.getComponent<DummyInterface>(), hg::TracedLogicError);
    }
}

TEST_F(SPeMPE_Test, ChildContextTest) {
    ASSERT_FALSE(_gameCtx.hasChildContext());
    _gameCtx.attachChildContext(std::make_unique<GameContext>(GameContext::RuntimeConfig{}));
    ASSERT_THROW(
        _gameCtx.attachChildContext(std::make_unique<GameContext>(GameContext::RuntimeConfig{})),
        hg::TracedLogicError
    );

    auto cc = _gameCtx.detachChildContext();
    ASSERT_NE(cc, nullptr);
    ASSERT_EQ(_gameCtx.detachChildContext(), nullptr);
    ASSERT_FALSE(_gameCtx.hasChildContext());

    _gameCtx.attachChildContext(std::move(cc));
    ASSERT_FALSE(_gameCtx.isChildContextJoinable());

    // This will force the child context to run what's basically a busy wait loop...
    // It's only a test, I don't care.
    ASSERT_NO_THROW(_gameCtx.startChildContext(-1));

    ASSERT_TRUE(_gameCtx.isChildContextJoinable());
    ASSERT_EQ(_gameCtx.stopAndJoinChildContext(), 0);

    _gameCtx.detachChildContext();
    ASSERT_FALSE(_gameCtx.hasChildContext());

    ASSERT_THROW(
        _gameCtx.isChildContextJoinable(),
        hg::TracedLogicError
    );
}

///////////////////////////////////////////////////////////////////////////
// SYNCHRONIZATION TEST                                                  //
///////////////////////////////////////////////////////////////////////////

TEST_F(SPeMPE_Test, SynchronizationTest) {
    ASSERT_TRUE(1);
}

} // namespace spempe
} // namespace jbatnozic