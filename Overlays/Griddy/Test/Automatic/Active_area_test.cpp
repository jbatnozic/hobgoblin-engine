// Copyright 2024 Jovan Batnozic. Released under MS-PL licence in Serbia.
// See https://github.com/jbatnozic/Hobgoblin?tab=readme-ov-file#licence

#include <Griddy/World/Active_area.hpp>

#include <gmock/gmock.h>
#include <gtest/gtest.h>

namespace jbatnozic {
namespace griddy {
namespace detail {

TEST(ActiveAreaTest, UpdateChunkList_EmptyToEmpty_NoChanges) {
    std::vector<ChunkId> currentList;
    std::vector<ChunkId> newList;

    auto changes = UpdateChunkList(currentList, newList);

    EXPECT_EQ(currentList.size(), 0);
    EXPECT_EQ(changes.size(), 0);
}

TEST(ActiveAreaTest, UpdateChunkList_NonEmptyToEmpty_NegativeChange) {
    std::vector<ChunkId> currentList{
        {1, 1}
    };
    std::vector<ChunkId> newList;

    auto changes = UpdateChunkList(currentList, newList);

    EXPECT_EQ(currentList.size(), 0);
    ASSERT_EQ(changes.size(), 1);
    EXPECT_EQ(changes[0].chunkId, ChunkId(1, 1));
    EXPECT_EQ(changes[0].usageDelta, -1);
}

TEST(ActiveAreaTest, UpdateChunkList_EmptyToNonEmpty_PositiveChange) {
    std::vector<ChunkId> currentList;
    std::vector<ChunkId> newList{
        {1, 1}
    };

    auto changes = UpdateChunkList(currentList, newList);

    ASSERT_EQ(currentList.size(), 1);
    EXPECT_EQ(currentList[0], ChunkId(1, 1));
    ASSERT_EQ(changes.size(), 1);
    EXPECT_EQ(changes[0].chunkId, ChunkId(1, 1));
    EXPECT_EQ(changes[0].usageDelta, +1);
    EXPECT_EQ(changes[0].loadPriority, 0);
}

TEST(ActiveAreaTest, UpdateChunkList_EmptyToNonEmpty_PositiveChange_CustomPriority) {
    std::vector<ChunkId> currentList;
    std::vector<ChunkId> newList{
        {1, 1}
    };

    auto changes = UpdateChunkList(currentList, newList, [newList](const ChunkId aChunkId) {
        EXPECT_EQ(aChunkId, ChunkId(1, 1));
        return 5;
    });

    ASSERT_EQ(currentList.size(), 1);
    EXPECT_EQ(currentList[0], ChunkId(1, 1));
    ASSERT_EQ(changes.size(), 1);
    EXPECT_EQ(changes[0].chunkId, ChunkId(1, 1));
    EXPECT_EQ(changes[0].usageDelta, +1);
    EXPECT_EQ(changes[0].loadPriority, 5);
}

TEST(ActiveAreaTest, UpdateChunkList_SmallerToLarger_PositiveChange) {
    std::vector<ChunkId> currentList{
        {1, 1}
    };
    std::vector<ChunkId> newList{
        {1, 1},
        {1, 2}
    };

    auto changes = UpdateChunkList(currentList, newList);

    EXPECT_THAT(currentList, testing::ElementsAreArray(newList));
    ASSERT_EQ(changes.size(), 1);
    EXPECT_EQ(changes[0].chunkId, ChunkId(1, 2));
    EXPECT_EQ(changes[0].usageDelta, +1);
    EXPECT_EQ(changes[0].loadPriority, 0);
}

TEST(ActiveAreaTest, UpdateChunkList_LargerToSmaller_NegativeChange) {
    std::vector<ChunkId> currentList{
        {1, 1},
        {1, 2},
        {1, 3},
        {1, 4}
    };
    std::vector<ChunkId> newList{
        {1, 1},
        {1, 2}
    };

    auto changes = UpdateChunkList(currentList, newList);

    EXPECT_THAT(currentList, testing::ElementsAreArray(newList));
    ASSERT_EQ(changes.size(), 2);
    EXPECT_EQ(changes[0].chunkId, ChunkId(1, 3));
    EXPECT_EQ(changes[0].usageDelta, -1);
    EXPECT_EQ(changes[1].chunkId, ChunkId(1, 4));
    EXPECT_EQ(changes[1].usageDelta, -1);
}

TEST(ActiveAreaTest, UpdateChunkList_NonEmptyWithDuplicatesToEmpty_NegativeChange) {
    std::vector<ChunkId> currentList{
        {1, 1},
        {1, 1}
    };
    std::vector<ChunkId> newList{};

    auto changes = UpdateChunkList(currentList, newList);

    EXPECT_THAT(currentList, testing::ElementsAreArray(newList));
    ASSERT_EQ(changes.size(), 2);
    EXPECT_EQ(changes[0].chunkId, ChunkId(1, 1));
    EXPECT_EQ(changes[0].usageDelta, -1);
    EXPECT_EQ(changes[1].chunkId, ChunkId(1, 1));
    EXPECT_EQ(changes[1].usageDelta, -1);
}

TEST(ActiveAreaTest, UpdateChunkList_NonEmptyToNonEmpty_Overlap) {
    std::vector<ChunkId> currentList{
        {1, 1},
        {1, 2},
        {1, 3}
    };
    std::vector<ChunkId> newList{
        {1, 3},
        {1, 4},
        {1, 5}
    };

    auto changes = UpdateChunkList(currentList, newList);

    EXPECT_THAT(currentList, testing::ElementsAreArray(newList));
    ASSERT_EQ(changes.size(), 4);

    EXPECT_EQ(changes[0].chunkId, ChunkId(1, 1));
    EXPECT_EQ(changes[0].usageDelta, -1);

    EXPECT_EQ(changes[1].chunkId, ChunkId(1, 2));
    EXPECT_EQ(changes[1].usageDelta, -1);

    EXPECT_EQ(changes[2].chunkId, ChunkId(1, 4));
    EXPECT_EQ(changes[2].usageDelta, +1);
    EXPECT_EQ(changes[2].loadPriority, 0);

    EXPECT_EQ(changes[3].chunkId, ChunkId(1, 5));
    EXPECT_EQ(changes[3].usageDelta, +1);
    EXPECT_EQ(changes[3].loadPriority, 1);
}

} // namespace detail
} // namespace griddy
}
