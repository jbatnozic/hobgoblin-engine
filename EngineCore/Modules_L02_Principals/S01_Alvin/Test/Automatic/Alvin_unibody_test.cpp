// Copyright 2024 Jovan Batnozic. Released under MS-PL licence in Serbia.
// See https://github.com/jbatnozic/Hobgoblin?tab=readme-ov-file#licence

#include <Hobgoblin/Alvin.hpp>
#include <Hobgoblin/ChipmunkPhysics.hpp>

#include <gtest/gtest.h>

#include <stdexcept>

namespace jbatnozic {
namespace hobgoblin {

namespace {
class DummyInterface : public alvin::EntityBase {
public:
    using EntitySuperclass = alvin::EntityBase;

    static constexpr alvin::EntityTypeId ENTITY_TYPE_ID = 0;

    static constexpr cpBitmask ENTITY_DEFAULT_CATEGORY = CP_ALL_CATEGORIES;
    static constexpr cpBitmask ENTITY_DEFAULT_MASK     = CP_ALL_CATEGORIES;
};

class DummyEntity : public DummyInterface {};

alvin::CollisionDelegate CreateSimpleCollisionDelegate() {
    return alvin::CollisionDelegateBuilder{}
        .addInteraction<alvin::EntityBase>(alvin::COLLISION_CONTACT,
                                           [](alvin::EntityBase&, const alvin::CollisionData&) {
                                               return alvin::Decision::ACCEPT_COLLISION;
                                           })
        .finalize();
}

void InitBasicUnibody(alvin::Unibody& aUnibody) {
    aUnibody.init(
        []() {
            return CreateSimpleCollisionDelegate();
        },
        []() {
            return alvin::Body::createKinematic();
        },
        [&aUnibody]() {
            return alvin::Shape::createCircle(aUnibody, 32.0, cpvzero);
        });
}
} // namespace

TEST(AlvinUnibodyTest, EmptyUnibody) {
    alvin::Unibody unibody;
    EXPECT_FALSE(unibody.isInitialized());

    {
        alvin::Unibody unibody2{std::move(unibody)};
        EXPECT_FALSE(unibody2.isInitialized());
        EXPECT_FALSE(unibody.isInitialized());
    }
}

TEST(AlvinUnibodyTest, DelegateFactoryThrows_UnibodyRemainsUninitialized) {
    static const std::string ERROR_MESSAGE = "placeholder";

    alvin::Unibody unibody;

    bool didThrow = false;

    try {
        unibody.init(
            []() -> alvin::CollisionDelegate {
                throw std::runtime_error{ERROR_MESSAGE};
            },
            []() {
                return alvin::Body::createKinematic();
            },
            [&unibody]() {
                return alvin::Shape::createCircle(unibody, 32.0, cpvzero);
            });
    } catch (const std::runtime_error& ex) {
        EXPECT_EQ(ex.what(), ERROR_MESSAGE);
        didThrow = true;
    }

    EXPECT_TRUE(didThrow);
    EXPECT_FALSE(unibody.isInitialized());
}

TEST(AlvinUnibodyTest, BodyFactoryThrows_UnibodyRemainsUninitialized) {
    static const std::string ERROR_MESSAGE = "placeholder";

    alvin::Unibody unibody;

    bool didThrow = false;

    try {
        unibody.init(
            []() {
                return CreateSimpleCollisionDelegate();
            },
            []() -> alvin::Body {
                throw std::runtime_error{ERROR_MESSAGE};
            },
            [&unibody]() {
                return alvin::Shape::createCircle(unibody, 32.0, cpvzero);
            });
    } catch (const std::runtime_error& ex) {
        EXPECT_EQ(ex.what(), ERROR_MESSAGE);
        didThrow = true;
    }

    EXPECT_TRUE(didThrow);
    EXPECT_FALSE(unibody.isInitialized());
}

TEST(AlvinUnibodyTest, ShapeFactoryThrows_UnibodyRemainsUninitialized) {
    static const std::string ERROR_MESSAGE = "placeholder";

    alvin::Unibody unibody;

    bool didThrow = false;

    try {
        unibody.init(
            []() {
                return CreateSimpleCollisionDelegate();
            },
            []() {
                return alvin::Body::createKinematic();
            },
            []() -> alvin::Shape {
                throw std::runtime_error{ERROR_MESSAGE};
            });
    } catch (const std::runtime_error& ex) {
        EXPECT_EQ(ex.what(), ERROR_MESSAGE);
        didThrow = true;
    }

    EXPECT_TRUE(didThrow);
    EXPECT_FALSE(unibody.isInitialized());
}

TEST(AlvinUnibodyTest, MoveConstruction) {
    alvin::Unibody unibody;
    EXPECT_FALSE(unibody.isInitialized());

    InitBasicUnibody(unibody);
    EXPECT_TRUE(unibody.isInitialized());

    cpBody*  body  = unibody.body;
    cpShape* shape = unibody.shape;

    {
        alvin::Unibody unibody2{std::move(unibody)};
        EXPECT_TRUE(unibody2.isInitialized());
        EXPECT_FALSE(unibody.isInitialized());

        EXPECT_EQ(static_cast<cpBody*>(unibody2.body), body);
        EXPECT_EQ(static_cast<cpShape*>(unibody2.shape), shape);
    }
}

TEST(AlvinUnibodyTest, MoveAssignment) {
    alvin::Unibody unibody;
    EXPECT_FALSE(unibody.isInitialized());

    InitBasicUnibody(unibody);
    EXPECT_TRUE(unibody.isInitialized());

    cpBody*  body  = unibody.body;
    cpShape* shape = unibody.shape;

    {
        alvin::Unibody unibody2;
        unibody2 = std::move(unibody);
        EXPECT_TRUE(unibody2.isInitialized());
        EXPECT_FALSE(unibody.isInitialized());

        EXPECT_EQ(static_cast<cpBody*>(unibody2.body), body);
        EXPECT_EQ(static_cast<cpShape*>(unibody2.shape), shape);
    }
}

TEST(AlvinUnibodyTest, CannotMoveUnibodyWithBoundDelegate) {
    DummyEntity    entity;
    alvin::Unibody unibody;

    InitBasicUnibody(unibody);
    unibody.bindDelegate(entity);

    {
        EXPECT_THROW({ alvin::Unibody{std::move(unibody)}; }, TracedLogicError);
        EXPECT_TRUE(unibody.isInitialized());
    }
    {
        alvin::Unibody unibody2;
        EXPECT_THROW({ unibody2 = std::move(unibody); }, TracedLogicError);
        EXPECT_TRUE(unibody.isInitialized());
    }
}

} // namespace hobgoblin
} // namespace jbatnozic
