
#define HOBGOBLIN_SHORT_NAMESPACE
#include <Hobgoblin/ColDetect.hpp>

#include <cstdint>
#include <gtest/gtest.h>

using namespace hg::cd;

namespace {
constexpr double DOMAIN_W = 400.0;
constexpr double DOMAIN_H = 400.0;
constexpr hg::PZInteger MAX_DOMAIN_DEPTH = 5;
constexpr hg::PZInteger MAX_ENTITIES_PER_NODE = 5;

constexpr std::int32_t TAG1 = 0;
constexpr std::int32_t TAG2 = 1;
constexpr std::int32_t TAG3 = 2;
constexpr std::int32_t TAG4 = 3;
constexpr std::int32_t TAG5 = 4;
}

class SingleThreadedQuadTreeCollisionDomainTest : public ::testing::Test {
public:
    SingleThreadedQuadTreeCollisionDomainTest()
        : _domain(DOMAIN_H, DOMAIN_W, MAX_DOMAIN_DEPTH, MAX_ENTITIES_PER_NODE, 0)
    {
    }

protected:
    QuadTreeCollisionDomain _domain;
};

TEST_F(SingleThreadedQuadTreeCollisionDomainTest, TwoBoxesOverlap) {
    auto box1 = _domain.insertEntity(TAG1, BoundingBox(100, 100, 100, 100), 1);
    auto box2 = _domain.insertEntity(TAG2, BoundingBox(150, 150, 100, 100), 1);

    ASSERT_EQ(_domain.recalcPairs(), 1);

    CollisionPair colPair;
    ASSERT_TRUE(_domain.pairsNext(colPair));
    ASSERT_TRUE((std::get<std::int32_t>(colPair.first) == TAG1 && std::get<std::int32_t>(colPair.second) == TAG2) ||
                (std::get<std::int32_t>(colPair.first) == TAG2 && std::get<std::int32_t>(colPair.second) == TAG1));
}

TEST_F(SingleThreadedQuadTreeCollisionDomainTest, TwoBoxesDoNotOverlap) {
    auto box1 = _domain.insertEntity(TAG1, BoundingBox(100, 100, 100, 100), 1);
    auto box2 = _domain.insertEntity(TAG2, BoundingBox(250, 250, 100, 100), 1);

    ASSERT_EQ(_domain.recalcPairs(), 0);
}

TEST_F(SingleThreadedQuadTreeCollisionDomainTest, TwoBoxesDoNotOverlapBecauseOfGroup) {
    auto box1 = _domain.insertEntity(TAG1, BoundingBox(100, 100, 100, 100), 0x0F);
    auto box2 = _domain.insertEntity(TAG2, BoundingBox(100, 100, 100, 100), 0xF0);

    ASSERT_EQ(_domain.recalcPairs(), 0);
}

TEST_F(SingleThreadedQuadTreeCollisionDomainTest, RemoveEntity) {
    auto box1 = _domain.insertEntity(TAG1, BoundingBox(100, 100, 100, 100), 1);
    auto box2 = _domain.insertEntity(TAG2, BoundingBox(150, 150, 100, 100), 1);

    ASSERT_EQ(_domain.recalcPairs(), 1);

    CollisionPair colPair;
    ASSERT_TRUE(_domain.pairsNext(colPair));
    ASSERT_TRUE((std::get<std::int32_t>(colPair.first) == TAG1 && std::get<std::int32_t>(colPair.second) == TAG2) ||
                (std::get<std::int32_t>(colPair.first) == TAG2 && std::get<std::int32_t>(colPair.second) == TAG1));

    box2.invalidate();
    
    ASSERT_EQ(_domain.recalcPairs(), 0);
}

TEST_F(SingleThreadedQuadTreeCollisionDomainTest, ThreeBoxesOverlap) {
    auto box1 = _domain.insertEntity(TAG1, BoundingBox(100, 100, 100, 100), 1);
    auto box2 = _domain.insertEntity(TAG2, BoundingBox(125, 125, 100, 100), 1);
    auto box3 = _domain.insertEntity(TAG3, BoundingBox(150, 150, 100, 100), 1);

    ASSERT_EQ(_domain.recalcPairs(), 3);
}

TEST_F(SingleThreadedQuadTreeCollisionDomainTest, FourBoxesOverlap) {
    auto box1 = _domain.insertEntity(TAG1, BoundingBox(100, 100, 100, 100), 1);
    auto box2 = _domain.insertEntity(TAG2, BoundingBox(125, 125, 100, 100), 1);
    auto box3 = _domain.insertEntity(TAG3, BoundingBox(150, 150, 100, 100), 1);
    auto box4 = _domain.insertEntity(TAG4, BoundingBox(175, 175, 100, 100), 1);

    ASSERT_EQ(_domain.recalcPairs(), 6);
}

///////////////////////////////////////////////////////////////////////////////

class MultiThreadedQuadTreeCollisionDomainTest : public ::testing::Test {
public:
    MultiThreadedQuadTreeCollisionDomainTest()
        : _domain(DOMAIN_H, DOMAIN_W, MAX_DOMAIN_DEPTH, MAX_ENTITIES_PER_NODE, 4)
    {
    }

protected:
    QuadTreeCollisionDomain _domain;
};

TEST_F(MultiThreadedQuadTreeCollisionDomainTest, TwoBoxesOverlap) {
    auto box1 = _domain.insertEntity(TAG1, BoundingBox(100, 100, 100, 100), 1);
    auto box2 = _domain.insertEntity(TAG2, BoundingBox(150, 150, 100, 100), 1);

    ASSERT_EQ(_domain.recalcPairs(), 1);

    CollisionPair colPair;
    ASSERT_TRUE(_domain.pairsNext(colPair));
    ASSERT_TRUE((std::get<std::int32_t>(colPair.first) == TAG1 && std::get<std::int32_t>(colPair.second) == TAG2) ||
                (std::get<std::int32_t>(colPair.first) == TAG2 && std::get<std::int32_t>(colPair.second) == TAG1));
}

TEST_F(MultiThreadedQuadTreeCollisionDomainTest, TwoBoxesDoNotOverlap) {
    auto box1 = _domain.insertEntity(TAG1, BoundingBox(100, 100, 100, 100), 1);
    auto box2 = _domain.insertEntity(TAG2, BoundingBox(250, 250, 100, 100), 1);

    ASSERT_EQ(_domain.recalcPairs(), 0);
}

TEST_F(MultiThreadedQuadTreeCollisionDomainTest, TwoBoxesDoNotOverlapBecauseOfGroup) {
    auto box1 = _domain.insertEntity(TAG1, BoundingBox(100, 100, 100, 100), 0x0F);
    auto box2 = _domain.insertEntity(TAG2, BoundingBox(100, 100, 100, 100), 0xF0);

    ASSERT_EQ(_domain.recalcPairs(), 0);
}

TEST_F(MultiThreadedQuadTreeCollisionDomainTest, RemoveEntity) {
    auto box1 = _domain.insertEntity(TAG1, BoundingBox(100, 100, 100, 100), 1);
    auto box2 = _domain.insertEntity(TAG2, BoundingBox(150, 150, 100, 100), 1);

    ASSERT_EQ(_domain.recalcPairs(), 1);

    CollisionPair colPair;
    ASSERT_TRUE(_domain.pairsNext(colPair));
    ASSERT_TRUE((std::get<std::int32_t>(colPair.first) == TAG1 && std::get<std::int32_t>(colPair.second) == TAG2) ||
                (std::get<std::int32_t>(colPair.first) == TAG2 && std::get<std::int32_t>(colPair.second) == TAG1));

    box2.invalidate();

    ASSERT_EQ(_domain.recalcPairs(), 0);
}

TEST_F(MultiThreadedQuadTreeCollisionDomainTest, ThreeBoxesOverlap) {
    auto box1 = _domain.insertEntity(TAG1, BoundingBox(100, 100, 100, 100), 1);
    auto box2 = _domain.insertEntity(TAG2, BoundingBox(125, 125, 100, 100), 1);
    auto box3 = _domain.insertEntity(TAG3, BoundingBox(150, 150, 100, 100), 1);

    ASSERT_EQ(_domain.recalcPairs(), 3);
}

TEST_F(MultiThreadedQuadTreeCollisionDomainTest, FourBoxesOverlap) {
    auto box1 = _domain.insertEntity(TAG1, BoundingBox(100, 100, 100, 100), 1);
    auto box2 = _domain.insertEntity(TAG2, BoundingBox(125, 125, 100, 100), 1);
    auto box3 = _domain.insertEntity(TAG3, BoundingBox(150, 150, 100, 100), 1);
    auto box4 = _domain.insertEntity(TAG4, BoundingBox(175, 175, 100, 100), 1);

    ASSERT_EQ(_domain.recalcPairs(), 6);
}