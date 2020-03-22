
#include <gtest/gtest.h>

#define HOBGOBLIN_SHORT_NAMESPACE
#include <Hobgoblin/Utility/Any_ptr.hpp>
#include <Hobgoblin/Utility/Autopack.hpp>
#include <Hobgoblin/Utility/Packet.hpp>

using namespace hg::util;

TEST(AnyPtrTest, IntPtr) {
    AnyPtr p;
    int dummy;

    p.reset(&dummy);

    ASSERT_EQ(p.get<int>(), &dummy);
}

TEST(AnyPtrTest, VoidPtr) {
    AnyPtr p;
    void* pVoid;
    pVoid = &pVoid;

    p.reset(pVoid);

    ASSERT_EQ(p.get<void>(), pVoid);
}

TEST(AnyPtrTest, TypeMismatchFailsAssert) {
    AnyPtr p;
    int dummy;

    p.reset(&dummy);

    EXPECT_DEATH_IF_SUPPORTED(p.get<double>(), ".*");
}

TEST(AnyPtrTest, TypeMismatchThrows) {
    AnyPtr p;
    int dummy;

    p.reset(&dummy);

    EXPECT_THROW(p.getOrThrow<double>(), TracedLogicError);
}

TEST(AnyPtrTest, ConstPtr) {
    AnyPtr p;
    const int constDummy = 5;

    p.reset(&constDummy);

    ASSERT_EQ(p.get<const int>(), &constDummy);
}

TEST(AnyPtrTest, ConstPtrTypeMismatchThrows) {
    AnyPtr p;
    const int constDummy = 5;

    p.reset(&constDummy);

    ASSERT_THROW(p.getOrThrow<int>(), TracedLogicError);
}

struct AutopackPublicTest {
    int i;
    float f;
    HG_ENABLE_AUTOPACK(AutopackPublicTest, i, f);
};

TEST(AutopackTest, AutopackPublicMembers) {
    Packet packet;

    AutopackPublicTest t1{123, 8.0};
    packet << t1;

    AutopackPublicTest t2;
    packet >> t2;

    ASSERT_EQ(t1.i, t2.i);
    ASSERT_EQ(t1.f, t2.f);
}

struct AutopackPrivateTest {
public:
    AutopackPrivateTest(int i = 0, float f = 0.f) : i{i}, f{f} {}

    int getI() const {
        return i;
    }

    float getF() const {
        return f;
    }

private:
    int i;
    float f;
    HG_ENABLE_AUTOPACK(AutopackPrivateTest, i, f);
};

TEST(AutopackTest, AutopackPrivateMembers) {
    Packet packet;

    AutopackPrivateTest t1{123, 8.0};
    packet << t1;

    AutopackPrivateTest t2;
    packet >> t2;

    ASSERT_EQ(t1.getI(), t2.getI());
    ASSERT_EQ(t1.getF(), t2.getF());
}