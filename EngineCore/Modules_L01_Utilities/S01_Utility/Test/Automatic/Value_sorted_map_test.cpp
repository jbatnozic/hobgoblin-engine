// Copyright 2024 Jovan Batnozic. Released under MS-PL licence in Serbia.
// See https://github.com/jbatnozic/Hobgoblin?tab=readme-ov-file#licence

#include <Hobgoblin/Common.hpp>
#include <Hobgoblin/Utility/Value_sorted_map.hpp>

#include <string>

#include <gtest/gtest.h>

namespace jbatnozic {
namespace hobgoblin {
namespace util {

TEST(ValueSortedMapTest, EmptyMap) {
    ValueSortedMap<std::string, int> map;
    EXPECT_TRUE(map.empty());
    EXPECT_EQ(map.size(), 0);
    EXPECT_EQ(map.begin(), map.end());
}

TEST(ValueSortedMapTest, OneValueInsertedThenErased) {
    ValueSortedMap<std::string, int> map;
    {
        const auto pair = map.insert(std::make_pair("key", 5));
        EXPECT_EQ(pair.first, map.begin());
        EXPECT_TRUE(pair.second);
    }

    EXPECT_FALSE(map.empty());
    EXPECT_EQ(map.size(), 1);
    ASSERT_NE(map.begin(), map.end());

    {
        auto& pair = *map.begin();
        EXPECT_EQ(pair.first, "key");
        EXPECT_EQ(pair.second, 5);
    }

    const auto iter = map.erase(map.begin());
    EXPECT_EQ(iter, map.end());

    EXPECT_TRUE(map.empty());
    EXPECT_EQ(map.size(), 0);
    EXPECT_EQ(map.begin(), map.end());
}

TEST(ValueSortedMapTest, OneValue_ValueWithSameKeyInsertedAgain_NoInsertion) {
    ValueSortedMap<std::string, int> map;
    {
        const auto pair = map.insert(std::make_pair("key", 5));
        EXPECT_EQ(pair.first, map.begin());
        EXPECT_TRUE(pair.second);
    }
    {
        const auto pair = map.insert(std::make_pair("key", 8));
        EXPECT_EQ(pair.first, map.end());
        EXPECT_FALSE(pair.second);
    }
    {
        auto& pair = *map.begin();
        EXPECT_EQ(pair.first, "key");
        EXPECT_EQ(pair.second, 5);
    }
}

TEST(ValueSortedMapTest, FourValues) {
    ValueSortedMap<std::string, int> map;
    map.insert(std::make_pair("key5", 5));
    map.insert(std::make_pair("key1", 1));
    map.insert(std::make_pair("key6", 6));
    map.insert(std::make_pair("key4", 4));

    EXPECT_FALSE(map.empty());
    EXPECT_EQ(map.size(), 4);
    ASSERT_NE(map.begin(), map.end());

    auto iter = map.begin();

    EXPECT_EQ(iter->first, "key1");
    EXPECT_EQ(iter->second, 1);

    ++iter;

    EXPECT_EQ(iter->first, "key4");
    EXPECT_EQ(iter->second, 4);

    ++iter;

    EXPECT_EQ(iter->first, "key5");
    EXPECT_EQ(iter->second, 5);

    iter = std::next(iter);

    EXPECT_EQ(iter->first, "key6");
    EXPECT_EQ(iter->second, 6);

    iter = std::next(iter);

    ASSERT_EQ(iter, map.end());
}

TEST(ValueSortedMapTest, ThreeValues_Find) {
    ValueSortedMap<std::string, int> map;
    map.insert(std::make_pair("key5", 5));
    map.insert(std::make_pair("key1", 1));
    map.insert(std::make_pair("key6", 6));

    {
        const auto iter = map.find("key8");
        EXPECT_EQ(iter, map.end());
    }
    {
        const auto iter = map.find("key1");
        EXPECT_EQ(iter, map.begin());
        EXPECT_EQ(iter->first, "key1");
        EXPECT_EQ(iter->second, 1);
    }
    {
        const auto iter = map.find("key5");
        EXPECT_EQ(iter, map.begin() + 1);
        EXPECT_EQ(iter->first, "key5");
        EXPECT_EQ(iter->second, 5);
    }
}

} // namespace util
} // namespace hobgoblin
} // namespace jbatnozic
