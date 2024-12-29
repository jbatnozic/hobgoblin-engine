// Copyright 2024 Jovan Batnozic. Released under MS-PL licence in Serbia.
// See https://github.com/jbatnozic/Hobgoblin?tab=readme-ov-file#licence

#include <Hobgoblin/Alvin.hpp>
#include <Hobgoblin/ChipmunkPhysics.hpp>
#include <Hobgoblin/Math.hpp>

#include <gtest/gtest.h>

#include <array>
#include <cmath>
#include <optional>

namespace jbatnozic {
namespace hobgoblin {

namespace {

enum EntityIds {
    EID_BLOCK,
};

enum EntityCategories {
    CAT_BLOCK = 0x01,
};

class BlockInterface : public alvin::EntityBase {
public:
    using EntitySuperclass = alvin::EntityBase;

    static constexpr alvin::EntityTypeId ENTITY_TYPE_ID = EID_BLOCK;

    static constexpr cpBitmask ENTITY_DEFAULT_CATEGORY = CAT_BLOCK;
    static constexpr cpBitmask ENTITY_DEFAULT_MASK     = CP_ALL_CATEGORIES;
};

void Init(alvin::MainCollisionDispatcher& aDispatcher, NeverNull<cpSpace*> aSpace) {
    aDispatcher.registerEntityType<BlockInterface>();
    aDispatcher.bind(aSpace);
}

///////////////////////////////////////////////////////////////////////////////

class CollisionCallbackInterface {
public:
    virtual ~CollisionCallbackInterface() = default;

    virtual alvin::Decision onContact(NeverNull<const alvin::EntityBase*> aEntity,
                                      NeverNull<const cpShape*>           aOtherShape)   = 0;
    virtual alvin::Decision onPreSolve(NeverNull<const alvin::EntityBase*> aEntity,
                                       NeverNull<const cpShape*>           aOtherShape)  = 0;
    virtual void            onPostSolve(NeverNull<const alvin::EntityBase*> aEntity,
                                        NeverNull<const cpShape*>           aOtherShape) = 0;
    virtual void            onSeparate(NeverNull<const alvin::EntityBase*> aEntity,
                                       NeverNull<const cpShape*>           aOtherShape)  = 0;
};

class Block : public BlockInterface {
public:
    static constexpr cpFloat WIDTH  = 32.0;
    static constexpr cpFloat HEIGHT = 32.0;

    Block(NeverNull<cpSpace*> aSpace, math::Vector2d aPosition)
        : _unibody{[this]() {
                       return _initColDelegate();
                   },
                   [this]() {
                       return alvin::Body::createDynamic(100.0, cpMomentForBox(100.0, WIDTH, HEIGHT));
                   },
                   [this]() {
                       return alvin::Shape::createBox(_unibody, WIDTH, HEIGHT);
                   }} {
        _unibody.bindDelegate(*this);
        _unibody.addToSpace(aSpace, aPosition);
    }

    void setPosition(math::Vector2d aPosition) {
        cpBodySetPosition(_unibody, cpv(aPosition.x, aPosition.y));
    }

    alvin::Shape& getShape() {
        return _unibody.shape;
    }

private:
    alvin::Unibody _unibody;

    alvin::CollisionDelegate _initColDelegate() {
        return alvin::CollisionDelegateBuilder{}.finalize();
    }
};

} // namespace

class AlvinQueriesTest : public ::testing::Test {
public:
    void SetUp() override {
        Init(_colDispatcher, _space);
    }

    static constexpr cpFloat EPSILON = 0.001;

protected:
    alvin::Space                   _space;
    alvin::MainCollisionDispatcher _colDispatcher;
};

TEST_F(AlvinQueriesTest, OneShape_PointQuery_PointOutsideOfShape_ShapeFound) {
    Block block{
        _space,
        {Block::WIDTH * 1.5, Block::HEIGHT * 1.5}
    };

    int                                  queryCallCount = 0;
    std::optional<alvin::PointQueryInfo> queryInfo      = std::nullopt;
    _space.runPointQuery(cpv(0.0, 0.0), 1000.0, [&](const alvin::PointQueryInfo& aQueryInfo) {
        queryCallCount += 1;
        queryInfo = aQueryInfo;
    });
    ASSERT_EQ(queryCallCount, 1);
    EXPECT_EQ(queryInfo->shape, block.getShape());
    EXPECT_EQ(&(queryInfo->delegate->getEntity()), static_cast<alvin::EntityBase*>(&block));
    EXPECT_NEAR(queryInfo->closestPoint.x, Block::WIDTH, EPSILON);
    EXPECT_NEAR(queryInfo->closestPoint.y, Block::HEIGHT, EPSILON);
    EXPECT_NEAR(queryInfo->distance, Block::WIDTH * sqrt(2.0), EPSILON);
}

TEST_F(AlvinQueriesTest, OneShape_PointQuery_PointOutsideOfShape_ShapeTooFarAway) {
    Block block{
        _space,
        {Block::WIDTH * 1.5, Block::HEIGHT * 1.5}
    };

    int                                  queryCallCount = 0;
    std::optional<alvin::PointQueryInfo> queryInfo      = std::nullopt;
    _space.runPointQuery(cpv(0.0, 0.0), 5.0, [&](const alvin::PointQueryInfo& aQueryInfo) {
        queryCallCount += 1;
        queryInfo = aQueryInfo;
    });
    ASSERT_EQ(queryCallCount, 0);
}

TEST_F(AlvinQueriesTest, OneShape_PointQuery_PointOutsideOfShape_ShapeFilteredOut) {
    Block block{
        _space,
        {Block::WIDTH * 1.5, Block::HEIGHT * 1.5}
    };

    int                                  queryCallCount = 0;
    std::optional<alvin::PointQueryInfo> queryInfo      = std::nullopt;
    _space.runPointQuery(cpv(0.0, 0.0),
                         1000.0,
                         cpShapeFilterNew(CP_NO_GROUP, 0, 0),
                         [&](const alvin::PointQueryInfo& aQueryInfo) {
                             queryCallCount += 1;
                             queryInfo = aQueryInfo;
                         });
    ASSERT_EQ(queryCallCount, 0);
}

TEST_F(AlvinQueriesTest, OneShape_PointQuery_PointInsideOfShape_ShapeFound) {
    Block block{
        _space,
        {Block::WIDTH * 1.5, Block::HEIGHT * 1.5}
    };

    int                                  queryCallCount = 0;
    std::optional<alvin::PointQueryInfo> queryInfo      = std::nullopt;
    _space.runPointQuery(cpv(Block::WIDTH * 1.5, Block::HEIGHT * 1.5),
                         1000.0,
                         [&](const alvin::PointQueryInfo& aQueryInfo) {
                             queryCallCount += 1;
                             queryInfo = aQueryInfo;
                         });
    ASSERT_EQ(queryCallCount, 1);
    EXPECT_EQ(queryInfo->shape, block.getShape());
    EXPECT_EQ(&(queryInfo->delegate->getEntity()), static_cast<alvin::EntityBase*>(&block));
    EXPECT_NEAR(queryInfo->distance, -Block::WIDTH / 2.0, EPSILON);
}

TEST_F(AlvinQueriesTest, OneShape_RaycastQuery_RayPassesByShape_ShapeNotFound) {
    Block block{
        _space,
        {Block::WIDTH * 1.5, Block::HEIGHT * 0.75}
    };

    int                                    queryCallCount = 0;
    std::optional<alvin::RaycastQueryInfo> queryInfo      = std::nullopt;
    _space.runRaycastQuery(cpv(0.0, 0.0),
                           cpv(100.0, 0.0),
                           0.0,
                           [&](const alvin::RaycastQueryInfo& aQueryInfo) {
                               queryCallCount += 1;
                               queryInfo = aQueryInfo;
                           });
    ASSERT_EQ(queryCallCount, 0);
}

TEST_F(AlvinQueriesTest, ThreeShapes_RaycastQuery_RayPassesThroughTwoShapes_BothShapesFound) {
    Block block1{
        _space,
        {Block::WIDTH * 1.0, Block::HEIGHT * 0.5}
    };
    Block block2{
        _space,
        {Block::WIDTH * 2.0, Block::HEIGHT * 0.75}
    };
    Block block3{
        _space,
        {Block::WIDTH * 1.5, Block::HEIGHT * 3.0}
    };

    int                                  queryCallCount = 0;
    std::vector<alvin::RaycastQueryInfo> queryInfos     = {};
    _space.runRaycastQuery(cpv(0.0, 0.0),
                           cpv(100.0, Block::HEIGHT),
                           0.0,
                           [&](const alvin::RaycastQueryInfo& aQueryInfo) {
                               queryCallCount += 1;
                               queryInfos.push_back(aQueryInfo);
                           });
    EXPECT_EQ(queryCallCount, 2);
    EXPECT_TRUE(std::find_if(queryInfos.begin(),
                             queryInfos.end(),
                             [&](const alvin::RaycastQueryInfo& aQueryInfo) {
                                 return &(aQueryInfo.delegate->getEntity()) ==
                                        static_cast<alvin::EntityBase*>(&block1);
                             }) != queryInfos.end());
    EXPECT_TRUE(std::find_if(queryInfos.begin(),
                             queryInfos.end(),
                             [&](const alvin::RaycastQueryInfo& aQueryInfo) {
                                 return &(aQueryInfo.delegate->getEntity()) ==
                                        static_cast<alvin::EntityBase*>(&block2);
                             }) != queryInfos.end());
    EXPECT_TRUE(std::find_if(queryInfos.begin(),
                             queryInfos.end(),
                             [&](const alvin::RaycastQueryInfo& aQueryInfo) {
                                 return &(aQueryInfo.delegate->getEntity()) ==
                                        static_cast<alvin::EntityBase*>(&block3);
                             }) == queryInfos.end());
}

TEST_F(AlvinQueriesTest, ThreeShapes_ClosestToRaycastQuery_RayPassesThroughTwoShapes_FirstShapeFound) {
    Block block1{
        _space,
        {Block::WIDTH * 1.0, Block::HEIGHT * 0.5}
    };
    Block block2{
        _space,
        {Block::WIDTH * 2.0, Block::HEIGHT * 0.75}
    };
    Block block3{
        _space,
        {Block::WIDTH * 1.5, Block::HEIGHT * 3.0}
    };

    const auto queryInfo =
        _space.runClosestToRaycastQuery(cpv(0.0, 0.0), cpv(100.0, Block::HEIGHT), 0.0);

    EXPECT_EQ(&(queryInfo.delegate->getEntity()), static_cast<alvin::EntityBase*>(&block1));
}

TEST_F(AlvinQueriesTest, OneShape_DirectedRaycastQuery_RayPassesByShapeButHasRadius_ShapeFound) {
    Block block{
        _space,
        {Block::WIDTH * 1.5, Block::HEIGHT * 0.75}  // origin.y = 24.0, top.y = 8.0
    };

    int                                    queryCallCount = 0;
    std::optional<alvin::RaycastQueryInfo> queryInfo      = std::nullopt;
    _space.runRaycastQuery(cpv(0.0, 0.0),
                           cpv(100.0, 0.0),
                           16.0,
                           [&](const alvin::RaycastQueryInfo& aQueryInfo) {
                               queryCallCount += 1;
                               queryInfo = aQueryInfo;
                           });
    ASSERT_EQ(queryCallCount, 1);
    EXPECT_EQ(&(queryInfo->delegate->getEntity()), static_cast<alvin::EntityBase*>(&block));
    EXPECT_NEAR(queryInfo->closestPoint.x, 32.0, EPSILON);
    EXPECT_NEAR(queryInfo->closestPoint.y, 8.0, EPSILON);
}

TEST_F(AlvinQueriesTest, FourShapes_DirectedRaycastQuery_ShapesFound) {
    Block blockNorth{
        _space,
        {Block::WIDTH * 1.5, Block::HEIGHT * 0.5}
    };
    Block blockWest{
        _space,
        {Block::WIDTH * 0.5, Block::HEIGHT * 1.5}
    };
    Block blockSouth{
        _space,
        {Block::WIDTH * 1.5, Block::HEIGHT * 2.5}
    };
    Block blockEast{
        _space,
        {Block::WIDTH * 2.5, Block::HEIGHT * 1.5}
    };

    const auto rayOrigin = cpv(Block::WIDTH * 1.5, Block::HEIGHT * 1.5);
    using namespace math::angle_literals;

    {
        SCOPED_TRACE("North");
        int                                    queryCallCount = 0;
        std::optional<alvin::RaycastQueryInfo> queryInfo      = std::nullopt;
        _space.runDirectedRaycastQuery(rayOrigin,
                                       90.0_deg,
                                       100.0,
                                       0.0,
                                       [&](const alvin::RaycastQueryInfo& aQueryInfo) {
                                           queryCallCount += 1;
                                           queryInfo = aQueryInfo;
                                       });
        ASSERT_EQ(queryCallCount, 1);
        EXPECT_EQ(&(queryInfo->delegate->getEntity()), static_cast<alvin::EntityBase*>(&blockNorth));
        EXPECT_NEAR(queryInfo->normalizedDistance, 16.0 / 100.0, EPSILON);
    }
    {
        SCOPED_TRACE("West");
        int                                    queryCallCount = 0;
        std::optional<alvin::RaycastQueryInfo> queryInfo      = std::nullopt;
        _space.runDirectedRaycastQuery(rayOrigin,
                                       180.0_deg,
                                       100.0,
                                       0.0,
                                       [&](const alvin::RaycastQueryInfo& aQueryInfo) {
                                           queryCallCount += 1;
                                           queryInfo = aQueryInfo;
                                       });
        ASSERT_EQ(queryCallCount, 1);
        EXPECT_EQ(&(queryInfo->delegate->getEntity()), static_cast<alvin::EntityBase*>(&blockWest));
        EXPECT_NEAR(queryInfo->normalizedDistance, 16.0 / 100.0, EPSILON);
    }
    {
        SCOPED_TRACE("South");
        int                                    queryCallCount = 0;
        std::optional<alvin::RaycastQueryInfo> queryInfo      = std::nullopt;
        _space.runDirectedRaycastQuery(rayOrigin,
                                       270.0_deg,
                                       100.0,
                                       0.0,
                                       [&](const alvin::RaycastQueryInfo& aQueryInfo) {
                                           queryCallCount += 1;
                                           queryInfo = aQueryInfo;
                                       });
        ASSERT_EQ(queryCallCount, 1);
        EXPECT_EQ(&(queryInfo->delegate->getEntity()), static_cast<alvin::EntityBase*>(&blockSouth));
        EXPECT_NEAR(queryInfo->normalizedDistance, 16.0 / 100.0, EPSILON);
    }
    {
        SCOPED_TRACE("East");
        int                                    queryCallCount = 0;
        std::optional<alvin::RaycastQueryInfo> queryInfo      = std::nullopt;
        _space.runDirectedRaycastQuery(rayOrigin,
                                       0.0_deg,
                                       100.0,
                                       0.0,
                                       [&](const alvin::RaycastQueryInfo& aQueryInfo) {
                                           queryCallCount += 1;
                                           queryInfo = aQueryInfo;
                                       });
        ASSERT_EQ(queryCallCount, 1);
        EXPECT_EQ(&(queryInfo->delegate->getEntity()), static_cast<alvin::EntityBase*>(&blockEast));
        EXPECT_NEAR(queryInfo->normalizedDistance, 16.0 / 100.0, EPSILON);
    }
}

TEST_F(AlvinQueriesTest, FourShapes_BboxQuery_ThreeShapesFound) {
    Block blockNorth{
        _space,
        {Block::WIDTH * 1.5, Block::HEIGHT * 0.5}
    };
    Block blockWest{
        _space,
        {Block::WIDTH * 0.5, Block::HEIGHT * 1.5}
    };
    Block blockSouth{
        _space,
        {Block::WIDTH * 1.5, Block::HEIGHT * 2.5}
    };
    Block blockEast{
        _space,
        {Block::WIDTH * 2.5, Block::HEIGHT * 1.5}
    };

    int                               queryCallCount = 0;
    std::vector<alvin::BboxQueryInfo> queryInfos     = {};
    _space.runFastBboxQuery(cpBB{.l = 0.0, .b = 0.0, .r = 100.0, .t = Block::HEIGHT * 1.5},
                            [&](const alvin::BboxQueryInfo& aQueryInfo) {
                                queryCallCount += 1;
                                queryInfos.push_back(aQueryInfo);
                            });
    EXPECT_EQ(queryCallCount, 3);

    EXPECT_TRUE(
        std::find_if(queryInfos.begin(), queryInfos.end(), [&](const alvin::BboxQueryInfo& aQueryInfo) {
            return &(aQueryInfo.delegate->getEntity()) == static_cast<alvin::EntityBase*>(&blockNorth);
        }) != queryInfos.end());

    EXPECT_TRUE(
        std::find_if(queryInfos.begin(), queryInfos.end(), [&](const alvin::BboxQueryInfo& aQueryInfo) {
            return &(aQueryInfo.delegate->getEntity()) == static_cast<alvin::EntityBase*>(&blockWest);
        }) != queryInfos.end());

    EXPECT_TRUE(
        std::find_if(queryInfos.begin(), queryInfos.end(), [&](const alvin::BboxQueryInfo& aQueryInfo) {
            return &(aQueryInfo.delegate->getEntity()) == static_cast<alvin::EntityBase*>(&blockEast);
        }) != queryInfos.end());

    EXPECT_TRUE(
        std::find_if(queryInfos.begin(), queryInfos.end(), [&](const alvin::BboxQueryInfo& aQueryInfo) {
            return &(aQueryInfo.delegate->getEntity()) == static_cast<alvin::EntityBase*>(&blockSouth);
        }) == queryInfos.end());
}

TEST_F(AlvinQueriesTest, FiveShapes_TriangleShapeQuery_ThreeShapesFound) {
    Block block1{
        _space,
        {0.0, 0.0}
    }; // Origin, won't intersect the triangle
    Block block2{
        _space,
        {1.0 * 32.0, 3.5 * 32.0}
    }; // Will intersect the bottom corner of the triangle
    Block block3{
        _space,
        {3.0 * 32.0, 2.5 * 32.0}
    }; // Will intersect the hypotenuse
    Block block4{
        _space,
        {3.0 * 32.0, 1.0 * 32.0}
    }; // Will intersect the top edge
    Block block5{
        _space,
        {5.0 * 32.0, 2.5 * 32.0}
    }; // Will be a bit too far away from the hypotenuse

    std::array<cpVect, 3> vertices = {cpv(0.0, 2.5 * 32.0), cpv(0.0, 0.0), cpv(5.0 * 32.0, 0.0)};
    alvin::Shape          triangle{cpPolyShapeNew(NULL, 3, vertices.data(), cpTransformIdentity, 0.0)};
    cpShapeUpdate(triangle, cpTransformNew(1.0, 0.0, 0.0, 1.0, 32.0, 32.0));
    // About affine transforms:
    // https://developer.apple.com/documentation/corefoundation/cgaffinetransform

    int                 queryCallCount = 0;
    std::vector<Block*> foundBlocks    = {};
    _space.runShapeQuery(triangle, [&](const alvin::ShapeQueryInfo& aQueryInfo) {
        queryCallCount += 1;
        foundBlocks.push_back(static_cast<Block*>(&(aQueryInfo.delegate->getEntity())));
    });

    EXPECT_EQ(queryCallCount, 3);

    // clang-format off
    EXPECT_TRUE(
        std::find_if(foundBlocks.begin(), foundBlocks.end(), [&](Block* aBlock) {
        return aBlock == &block1;
    }) == foundBlocks.end());

    EXPECT_TRUE(
        std::find_if(foundBlocks.begin(), foundBlocks.end(), [&](Block* aBlock) {
        return aBlock == &block2;
    }) != foundBlocks.end());

    EXPECT_TRUE(
        std::find_if(foundBlocks.begin(), foundBlocks.end(), [&](Block* aBlock) {
        return aBlock == &block3;
    }) != foundBlocks.end());

    EXPECT_TRUE(
        std::find_if(foundBlocks.begin(), foundBlocks.end(), [&](Block* aBlock) {
        return aBlock == &block4;
    }) != foundBlocks.end());

    EXPECT_TRUE(
        std::find_if(foundBlocks.begin(), foundBlocks.end(), [&](Block* aBlock) {
        return aBlock == &block5;
    }) == foundBlocks.end());
    // clang-format on
}

} // namespace hobgoblin
} // namespace jbatnozic
