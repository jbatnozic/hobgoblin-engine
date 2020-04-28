
#include <gtest/gtest.h>

#define HOBGOBLIN_SHORT_NAMESPACE
#include <Hobgoblin/Utility/Any_ptr.hpp>
#include <Hobgoblin/Utility/Autopack.hpp>
#include <Hobgoblin/Utility/Packet.hpp>
#include <Hobgoblin/Utility/Math.hpp>

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

TEST(PacketTest, AppendPacketToPacket) {
    PacketBase basePacket;
    Packet hgPacket;

    // Pack:

    hgPacket << std::int32_t{808} << std::int32_t{123};
    basePacket << hgPacket << std::int16_t{1337};

    // Unpack:

    Packet testPacket;
    std::int32_t val808, val123;
    std::int16_t guardValue;

    basePacket >> testPacket >> guardValue;

    ASSERT_TRUE(basePacket);
    ASSERT_TRUE(basePacket.endOfPacket());
    ASSERT_EQ(guardValue, 1337);

    testPacket >> val808 >> val123;
    ASSERT_TRUE(testPacket.endOfPacket());
    ASSERT_EQ(val808, 808);
    ASSERT_EQ(val123, 123);
}

TEST(MathTest, IntegralCeilDivTest) {
    ASSERT_EQ(IntegralCeilDiv(0, 1), 0);

    ASSERT_EQ(IntegralCeilDiv(4, 5), 1);
    ASSERT_EQ(IntegralCeilDiv(5, 5), 1);
    ASSERT_EQ(IntegralCeilDiv(12, 5), 3);
    ASSERT_EQ(IntegralCeilDiv(15, 5), 3);

    ASSERT_EQ(IntegralCeilDiv(-6, 6), -1);
    ASSERT_EQ(IntegralCeilDiv(6, -6), -1);
    ASSERT_EQ(IntegralCeilDiv(-6, -6), 1);

    ASSERT_EQ(IntegralCeilDiv(-6, 4), -1);
}