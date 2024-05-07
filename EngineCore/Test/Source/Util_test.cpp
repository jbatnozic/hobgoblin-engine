// Copyright 2024 Jovan Batnozic. Released under MS-PL licence in Serbia.
// See https://github.com/jbatnozic/Hobgoblin?tab=readme-ov-file#licence

// clang-format off


#include <gtest/gtest.h>

#define HOBGOBLIN_SHORT_NAMESPACE
#include <Hobgoblin/Common.hpp>
#include <Hobgoblin/Utility/Any_ptr.hpp>
#include <Hobgoblin/Utility/Autopack.hpp>
#include <Hobgoblin/Utility/Dynamic_bitset.hpp>
#include <Hobgoblin/Utility/Packet.hpp>
#include <Hobgoblin/Math.hpp>

using hg::PZInteger;
using namespace hg::util;
using namespace hg::math;

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

    EXPECT_THROW(p.getOrThrow<double>(), hg::TracedLogicError);
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

    ASSERT_THROW(p.getOrThrow<int>(), hg::TracedLogicError);
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

TEST(DynamicBitsetTest, DynamicBitsetTest) {
    {
        SCOPED_TRACE("Check that initial values are all zero");
        DynamicBitset bitset;
        for (PZInteger i = 0; i < 200; i += 1) {
            EXPECT_EQ(bitset.getBit(i), false);
        }
    }
    {
        SCOPED_TRACE("Set and clear check; part 1");
        DynamicBitset bitset;
        bitset.setBit(5);
        bitset.setBit(29);
        for (PZInteger i = 0; i < 5; i += 1) {
            EXPECT_EQ(bitset.getBit(i), false) << " with i=" << i;
        }
        EXPECT_EQ(bitset.getBit(5), true);
        for (PZInteger i = 6; i < 29; i += 1) {
            EXPECT_EQ(bitset.getBit(i), false) << " with i=" << i;
        }
        EXPECT_EQ(bitset.getBit(29), true);
        for (PZInteger i = 30; i < 200; i += 1) {
            EXPECT_EQ(bitset.getBit(i), false) << " with i=" << i;
        }
        bitset.clearBit(5);
        bitset.clearBit(29);
        for (PZInteger i = 0; i < 200; i += 1) {
            EXPECT_EQ(bitset.getBit(i), false);
        }
    }
    {
        SCOPED_TRACE("Set and clear check; part 2");
        DynamicBitset bitset;
        bitset.setBit(200);
        for (PZInteger i = 0; i < 200; i += 1) {
            EXPECT_EQ(bitset.getBit(i), false) << " with i=" << i;
        }
        EXPECT_EQ(bitset.getBit(200), true);
    }
}

// clang-format on
