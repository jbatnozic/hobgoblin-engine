// Copyright 2024 Jovan Batnozic. Released under MS-PL licence in Serbia.
// See https://github.com/jbatnozic/Hobgoblin?tab=readme-ov-file#licence

// #include <Hobgoblin/Utility/Packet.hpp>
#include <Hobgoblin/Utility/Pakket.hpp>

#include <array>
#include <cstdint>
#include <gtest/gtest.h>

namespace jbatnozic {
namespace hobgoblin {
namespace util {

using Packet = Pakket;

template <class T>
void TestPacketWithType(T aValue, std::size_t aExpectedSize) {
    Packet packet;
    {
        SCOPED_TRACE("Insert and extract with operator>> (good)");

        packet << aValue;
        EXPECT_EQ(packet.getRemainingDataSize(), aExpectedSize);
        T temp;
        packet >> temp;
        EXPECT_EQ(temp, aValue);
        EXPECT_TRUE(packet);
    }
    {
        SCOPED_TRACE("Insert and extract with nothrow operator>> (good)");

        packet << aValue;
        EXPECT_EQ(packet.getRemainingDataSize(), aExpectedSize);
        T temp;
        packet.noThrow() >> temp;
        EXPECT_EQ(temp, aValue);
        EXPECT_TRUE(packet);
    }
    {
        SCOPED_TRACE("Insert and extract with extract() (good)");

        packet << aValue;
        EXPECT_EQ(packet.getRemainingDataSize(), aExpectedSize);
        auto temp = packet.template extract<T>();
        EXPECT_EQ(temp, aValue);
        EXPECT_TRUE(packet);
    }
    {
        SCOPED_TRACE("Insert and extract with extractNoThrow() (good)");

        packet << aValue;
        EXPECT_EQ(packet.getRemainingDataSize(), aExpectedSize);
        auto temp = packet.template extractNoThrow<T>();
        EXPECT_EQ(temp, aValue);
        EXPECT_TRUE(packet);
    }
    EXPECT_EQ(packet.getDataSize(), 4 * aExpectedSize);
    {
        SCOPED_TRACE("Do not insert and extract with operator>> (failed)");

        EXPECT_EQ(packet.getRemainingDataSize(), 0);
        T temp;
        EXPECT_THROW(packet >> temp, StreamExtractError);
        EXPECT_FALSE(packet);
    }
    {
        SCOPED_TRACE("Do not insert and extract with nothrow operator>> (failed)");

        EXPECT_EQ(packet.getRemainingDataSize(), 0);
        T temp;
        (void)(packet.noThrow() >> temp);
        EXPECT_FALSE(packet);
    }
    {
        SCOPED_TRACE("Do not insert and extract with extract() (failed)");

        EXPECT_EQ(packet.getRemainingDataSize(), 0);
        EXPECT_THROW(packet.template extract<T>(), StreamExtractError);
        EXPECT_FALSE(packet);
    }
    {
        SCOPED_TRACE("Do not insert and extract with extractNoThrow() (failed)");

        EXPECT_EQ(packet.getRemainingDataSize(), 0);
        (void)packet.template extractNoThrow<T>();
        EXPECT_FALSE(packet);
    }
}

TEST(HGUtilPacketTest, TestInt8) {
    TestPacketWithType<std::int8_t>(123, sizeof(std::int8_t));
}

TEST(HGUtilPacketTest, TestUInt8) {
    TestPacketWithType<std::uint8_t>(123, sizeof(std::uint8_t));
}

TEST(HGUtilPacketTest, TestInt16) {
    TestPacketWithType<std::int16_t>(1337, sizeof(std::int16_t));
}

TEST(HGUtilPacketTest, TestUInt16) {
    TestPacketWithType<std::uint16_t>(1337, sizeof(std::uint16_t));
}

TEST(HGUtilPacketTest, TestInt32) {
    TestPacketWithType<std::int32_t>(1'000'000, sizeof(std::int32_t));
}

TEST(HGUtilPacketTest, TestUInt32) {
    TestPacketWithType<std::uint32_t>(1'000'000, sizeof(std::uint32_t));
}

TEST(HGUtilPacketTest, TestInt64) {
    TestPacketWithType<std::int64_t>(1'000'000'000'000, sizeof(std::int64_t));
}

TEST(HGUtilPacketTest, TestUInt64) {
    TestPacketWithType<std::uint64_t>(1'000'000'000'000, sizeof(std::uint64_t));
}

TEST(HGUtilPacketTest, TestFloat) {
    TestPacketWithType<float>(123.456f, sizeof(float));
}

TEST(HGUtilPacketTest, TestDouble) {
    TestPacketWithType<double>(987.654, sizeof(double));
}

TEST(HGUtilPacketTest, TestStdString) {
    TestPacketWithType<std::string>("ChuckNorris", sizeof(std::uint32_t) + 11);
}

TEST(HGUtilPacketTest, TestUnicodeString) {
    TestPacketWithType<UnicodeString>(HG_UNISTR("!@#$%^&*()šđćč"), sizeof(std::uint32_t) + 18);
}

#if 0 // TODO
TEST(HGUtilPacketTest, TestInsertingAnotherPacket) {
    Packet basePacket;
    Packet otherPacket;

    // Pack:

    otherPacket << std::int32_t{808} << std::int32_t{123};
    basePacket << otherPacket << std::int16_t{1337};

    // Unpack:

    Packet       testPacket;
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
#endif

TEST(HGUtilPacketTest, TestNoThrowAdapter) {
    Packet        packet;
    std::int32_t  i = 5;
    std::int32_t& ri{i};
    packet.noThrow() >> ri;
}

TEST(HGUtilPacketTest, TestExtractBytes) {
    const std::array<std::uint8_t, 4> testData     = {1, 4, 27, 100};
    const auto                        testDataSize = stopz(testData.size() * sizeof(std::uint8_t));

    Packet packet;
    packet.appendBytes(testData.data(), testDataSize);
    const auto* p = packet.extractBytes(testDataSize);
    ASSERT_NE(p, nullptr);
    EXPECT_EQ(packet.getRemainingDataSize(), 0);
    EXPECT_EQ(std::memcmp(testData.data(), p, testDataSize), 0);
    EXPECT_TRUE(packet);

    EXPECT_EQ(packet.extractBytes(0), nullptr);

    EXPECT_THROW(packet.extractBytes(1), StreamExtractError);
    EXPECT_FALSE(packet);
}

TEST(HGUtilPacketTest, TestExtractBytesNoThrow) {
    const std::array<std::uint8_t, 4> testData     = {1, 4, 27, 100};
    const auto                        testDataSize = stopz(testData.size() * sizeof(std::uint8_t));

    Packet packet;
    packet.appendBytes(testData.data(), testDataSize);
    const auto* p = packet.extractBytesNoThrow(testDataSize);
    ASSERT_NE(p, nullptr);
    EXPECT_EQ(packet.getRemainingDataSize(), 0);
    EXPECT_EQ(std::memcmp(testData.data(), p, testDataSize), 0);
    EXPECT_TRUE(packet);

    EXPECT_EQ(packet.extractBytesNoThrow(0), nullptr);

    EXPECT_EQ(packet.extractBytesNoThrow(1), nullptr);
    EXPECT_FALSE(packet);
}

namespace {
//! A custom type that has properly implemented packet support:
//! - It is default-constructible
//! - It has overloaded operator<<
//! - It has overloaded operator>> that DOES NOT THROW on failure
struct MyCustomType {
    MyCustomType() = default;
    MyCustomType(std::int32_t aI, std::string aS)
        : i{aI}
        , s{std::move(aS)} {}

    // Packets need to work with move-only types too
    MyCustomType(const MyCustomType&)            = delete;
    MyCustomType& operator=(const MyCustomType&) = delete;
    MyCustomType(MyCustomType&&)                 = default;
    MyCustomType& operator=(MyCustomType&&)      = default;

    std::int32_t i = 0;
    std::string  s = "";
};

bool operator==(const MyCustomType& aLhs, const MyCustomType& aRhs) {
    return (aLhs.i == aRhs.i && aLhs.s == aRhs.s);
}

OutputStream& operator<<(OutputStreamExtender& aPacket, const MyCustomType& aData) {
    aPacket->noThrow() << aData.i << aData.s;
    return *aPacket;
}

InputStream& operator>>(InputStreamExtender& aPacket, MyCustomType& aData) {
    aPacket->noThrow() >> aData.i >> aData.s;
    return *aPacket;
}
} // namespace

TEST(HGUtilPacketTest, TestMyCustomType) {
    auto       data         = MyCustomType{23, "love"};
    const auto expectedSize = stopz(sizeof(data.i) + sizeof(std::uint32_t) + 4 * sizeof(char));
    TestPacketWithType<MyCustomType>(std::move(data), expectedSize);
}

TEST(HGUtilPacketTest, TestMyCustomType_Derived) {
    class Derived : public MyCustomType {
    public:
        using MyCustomType::MyCustomType;
    };

    auto       data         = Derived{23, "love"};
    const auto expectedSize = stopz(sizeof(data.i) + sizeof(std::uint32_t) + 4 * sizeof(char));
    TestPacketWithType<Derived>(std::move(data), expectedSize);
}

} // namespace util
} // namespace hobgoblin
} // namespace jbatnozic
