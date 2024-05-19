// Copyright 2024 Jovan Batnozic. Released under MS-PL licence in Serbia.
// See https://github.com/jbatnozic/Hobgoblin?tab=readme-ov-file#licence

#include <Hobgoblin/Alvin.hpp>
#include <Hobgoblin/ChipmunkPhysics.hpp>
#include <Hobgoblin/Math.hpp>

#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include <iostream>

namespace jbatnozic {
namespace hobgoblin {

namespace {

enum EntityIds {
    EID_PLAYER,
    EID_ENEMY,
    EID_LOOT,
    EID_HEALTH,
    EID_WALL
};

enum EntityCategories {
    CAT_PLAYER = 0x01,
    CAT_ENEMY  = 0x02,
    CAT_LOOT   = 0x04,
    CAT_WALL   = 0x08
};

//! Placer character. Collides with all other entities.
class PlayerInterface : public alvin::EntityBase {
public:
    using EntitySuperclass = alvin::EntityBase;

    static constexpr alvin::EntityTypeId ENTITY_TYPE_ID = EID_PLAYER;

    static constexpr cpBitmask ENTITY_DEFAULT_CATEGORY = CAT_PLAYER;
    static constexpr cpBitmask ENTITY_DEFAULT_MASK     = CAT_ENEMY | CAT_LOOT | CAT_WALL;
};

//! The enemies collide with the player, with walls and with each other.
class EnemyInterface : public alvin::EntityBase {
public:
    using EntitySuperclass = alvin::EntityBase;

    static constexpr alvin::EntityTypeId ENTITY_TYPE_ID = EID_ENEMY;

    static constexpr cpBitmask ENTITY_DEFAULT_CATEGORY = CAT_ENEMY;
    static constexpr cpBitmask ENTITY_DEFAULT_MASK     = CAT_PLAYER | CAT_ENEMY | CAT_WALL;
};

//! Loot is meant to be picked up by the player. It collides only with the
//! player and with walls.
class LootInterface : public alvin::EntityBase {
public:
    using EntitySuperclass = alvin::EntityBase;

    static constexpr alvin::EntityTypeId ENTITY_TYPE_ID = EID_LOOT;

    static constexpr cpBitmask ENTITY_DEFAULT_CATEGORY = CAT_LOOT;
    static constexpr cpBitmask ENTITY_DEFAULT_MASK     = CAT_PLAYER | CAT_ENEMY | CAT_WALL;
};

//! Special case of Loot.
class HealthPickUp : public LootInterface {
public:
    using EntitySuperclass = LootInterface;

    static constexpr alvin::EntityTypeId ENTITY_TYPE_ID = EID_HEALTH;

    // No category and mask 'overrides'
};

//! Walls are immoveable and impassable and don't collide with each other but
//! otherwise collide with everything else.
class WallInterface : public alvin::EntityBase {
public:
    using EntitySuperclass = alvin::EntityBase;

    static constexpr alvin::EntityTypeId ENTITY_TYPE_ID = EID_WALL;

    static constexpr cpBitmask ENTITY_DEFAULT_CATEGORY = EID_WALL;
    static constexpr cpBitmask ENTITY_DEFAULT_MASK     = CAT_PLAYER | CAT_ENEMY | CAT_LOOT | CAT_WALL;
};

void Init(alvin::MainCollisionDispatcher aDispatcher, NeverNull<cpSpace*> aSpace) {
    aDispatcher.registerEntityType<PlayerInterface>();
    aDispatcher.registerEntityType<EnemyInterface>();
    aDispatcher.registerEntityType<LootInterface>();
    aDispatcher.registerEntityType<HealthPickUp>();
    aDispatcher.registerEntityType<WallInterface>();
    aDispatcher.configureSpace(aSpace);
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

class CollisionCallback : public CollisionCallbackInterface {
public:
    MOCK_METHOD(alvin::Decision,
                onContact,
                (NeverNull<const alvin::EntityBase*>, NeverNull<const cpShape*>));
    MOCK_METHOD(alvin::Decision,
                onPreSolve,
                (NeverNull<const alvin::EntityBase*>, NeverNull<const cpShape*>));
    MOCK_METHOD(void, onPostSolve, (NeverNull<const alvin::EntityBase*>, NeverNull<const cpShape*>));
    MOCK_METHOD(void, onSeparate, (NeverNull<const alvin::EntityBase*>, NeverNull<const cpShape*>));
};

NeverNull<cpShape*> GetOtherShape(NeverNull<cpArbiter*> aArbiter, cpShape* aMyShape) {
    CP_ARBITER_GET_SHAPES(aArbiter, shape1, shape2);
    if (shape1 == aMyShape) {
        return shape2;
    }
    return shape1;
}

class Player : public PlayerInterface {
public:
    Player(NeverNull<cpSpace*> aSpace, math::Vector2d aPosition)
        : _colDelegate{_initColDelegate()}
        , _body{alvin::Body::createDynamic(100.0, cpMomentForCircle(100.0, 0.0, 16.0, cpvzero))}
        //, _body{alvin::Body::createKinematic()}
        , _shape{alvin::Shape::createCircle(_body, 16.0, cpvzero)} {
        _colDelegate.bind(*this, _shape);
        cpSpaceAddBody(aSpace, _body);
        setPosition(aPosition);
        cpSpaceAddShape(aSpace, _shape);
    }

    void setPosition(math::Vector2d aPosition) {
        cpBodySetPosition(_body, cpv(aPosition.x, aPosition.y));
    }

    testing::StrictMock<CollisionCallback>& getColllisionCallback() {
        return _colCallback;
    }

    NeverNull<const cpShape*> getShape() const {
        return _shape;
    }

private:
    testing::StrictMock<CollisionCallback> _colCallback;

    alvin::CollisionDelegate _colDelegate;
    alvin::Body              _body;
    alvin::Shape             _shape;

    alvin::CollisionDelegate _initColDelegate() {
        return alvin::CollisionDelegateBuilder{}
            .addInteraction<WallInterface>(
                alvin::COLLISION_CONTACT,
                [this](WallInterface&        aWall,
                       NeverNull<cpArbiter*> aArbiter,
                       NeverNull<cpSpace*>   aSpace,
                       PZInteger             aOrder) -> alvin::Decision {
                    return _colCallback.onContact(&aWall, GetOtherShape(aArbiter, _shape));
                })
            .addInteraction<WallInterface>(
                alvin::COLLISION_PRE_SOLVE,
                [this](WallInterface&        aWall,
                       NeverNull<cpArbiter*> aArbiter,
                       NeverNull<cpSpace*>   aSpace,
                       PZInteger             aOrder) -> alvin::Decision {
                    return _colCallback.onPreSolve(&aWall, GetOtherShape(aArbiter, _shape));
                })
            .addInteraction<WallInterface>(alvin::COLLISION_POST_SOLVE,
                                           [this](WallInterface&        aWall,
                                                  NeverNull<cpArbiter*> aArbiter,
                                                  NeverNull<cpSpace*>   aSpace,
                                                  PZInteger             aOrder) {
                                               _colCallback.onPostSolve(&aWall,
                                                                        GetOtherShape(aArbiter, _shape));
                                           })
            .addInteraction<WallInterface>(alvin::COLLISION_SEPARATION,
                                           [this](WallInterface&        aWall,
                                                  NeverNull<cpArbiter*> aArbiter,
                                                  NeverNull<cpSpace*>   aSpace,
                                                  PZInteger             aOrder) {
                                               _colCallback.onSeparate(&aWall,
                                                                       GetOtherShape(aArbiter, _shape));
                                           })
            .finalize();
    }
};

class Wall : public WallInterface {
public:
    Wall(NeverNull<cpSpace*> aSpace, math::Vector2d aPosition)
        : _colDelegate{_initColDelegate()}
        , _body{alvin::Body::createStatic()}
        , _shape{alvin::Shape::createBox(_body, 32.0, 32.0, 0.0)} {
        _colDelegate.bind(*this, _shape);
        cpSpaceAddBody(aSpace, _body);
        setPosition(aPosition);
        cpSpaceAddShape(aSpace, _shape);
    }

    void setPosition(math::Vector2d aPosition) {
        cpBodySetPosition(_body, cpv(aPosition.x, aPosition.y));
    }

    testing::StrictMock<CollisionCallback>& getColllisionCallback() {
        return _colCallback;
    }

    NeverNull<const cpShape*> getShape() const {
        return _shape;
    }

private:
    testing::StrictMock<CollisionCallback> _colCallback;

    alvin::CollisionDelegate _colDelegate;
    alvin::Body              _body;
    alvin::Shape             _shape;

    alvin::CollisionDelegate _initColDelegate() {
        return alvin::CollisionDelegateBuilder{}
            .addInteraction<PlayerInterface>(
                alvin::COLLISION_CONTACT,
                [this](PlayerInterface&      aPlayer,
                       NeverNull<cpArbiter*> aArbiter,
                       NeverNull<cpSpace*>   aSpace,
                       PZInteger             aOrder) -> alvin::Decision {
                    return _colCallback.onContact(&aPlayer, GetOtherShape(aArbiter, _shape));
                })
            .addInteraction<PlayerInterface>(
                alvin::COLLISION_PRE_SOLVE,
                [this](PlayerInterface&      aPlayer,
                       NeverNull<cpArbiter*> aArbiter,
                       NeverNull<cpSpace*>   aSpace,
                       PZInteger             aOrder) -> alvin::Decision {
                    return _colCallback.onPreSolve(&aPlayer, GetOtherShape(aArbiter, _shape));
                })
            .addInteraction<PlayerInterface>(
                alvin::COLLISION_POST_SOLVE,
                [this](PlayerInterface&      aPlayer,
                       NeverNull<cpArbiter*> aArbiter,
                       NeverNull<cpSpace*>   aSpace,
                       PZInteger             aOrder) {
                    _colCallback.onPostSolve(&aPlayer, GetOtherShape(aArbiter, _shape));
                })
            .addInteraction<PlayerInterface>(
                alvin::COLLISION_SEPARATION,
                [this](PlayerInterface&      aPlayer,
                       NeverNull<cpArbiter*> aArbiter,
                       NeverNull<cpSpace*>   aSpace,
                       PZInteger             aOrder) {
                    _colCallback.onSeparate(&aPlayer, GetOtherShape(aArbiter, _shape));
                })
            .finalize();
    }
};

constexpr auto ACCEPT_COLLISION = alvin::Decision::ACCEPT_COLLISION;
constexpr auto REJECT_COLLISION = alvin::Decision::REJECT_COLLISION;

} // namespace

using ::testing::Eq;
using ::testing::Mock;
using ::testing::Return;

class AlvinCollisionTest : public ::testing::Test {
public:
    void SetUp() override {
        Init(_colDispatcher, _space);
    }

    void DoSpaceStep() {
        _space.step(1.0 / 60.0);
    }

protected:
    alvin::Space                   _space;
    alvin::MainCollisionDispatcher _colDispatcher;
};

#define CONTACT    (0x01 << alvin::detail::USAGE_COL_BEGIN)
#define PRE_SOLVE  (0x01 << alvin::detail::USAGE_COL_PRESOLVE)
#define POST_SOLVE (0x01 << alvin::detail::USAGE_COL_POSTSOLVE)
#define SEPARATE   (0x01 << alvin::detail::USAGE_COL_SEPARATE)

template <class taEntity>
class TestDecorator : public taEntity {
public:
    using taEntity::taEntity;

    template <class taOther>
    void expectCollisionWith(const taOther&  aOther,
                             int             aCollisionTypeMask,
                             alvin::Decision aContactDecision  = ACCEPT_COLLISION,
                             alvin::Decision aPreSolveDecision = ACCEPT_COLLISION) {
        auto& cb = this->getColllisionCallback();

        if (aCollisionTypeMask & CONTACT) {
            EXPECT_CALL(cb, onContact(Eq(&aOther), Eq(aOther.getShape())))
                .WillOnce(Return(aContactDecision));
        }
        if (aCollisionTypeMask & PRE_SOLVE) {
            EXPECT_CALL(cb, onPreSolve(Eq(&aOther), Eq(aOther.getShape())))
                .WillOnce(Return(aPreSolveDecision));
        }
        if (aCollisionTypeMask & POST_SOLVE) {
            EXPECT_CALL(cb, onPostSolve(Eq(&aOther), Eq(aOther.getShape())));
        }
        if (aCollisionTypeMask & SEPARATE) {
            EXPECT_CALL(cb, onSeparate(Eq(&aOther), Eq(aOther.getShape())));
        }
    }
};

// NeverNull<const alvin::EntityBase*> playerPtr      = &player;
// NeverNull<const cpShape*>           playerShapePtr = player.getShape();
// NeverNull<const alvin::EntityBase*> wallPtr        = &wall;
// NeverNull<const cpShape*>           wallShapePtr   = wall.getShape();

// std::cout << "player = " << std::hex << reinterpret_cast<std::uintptr_t>(playerPtr.get()) << '\n';
// std::cout << "player.shape = " << std::hex << reinterpret_cast<std::uintptr_t>(playerShapePtr.get())
//           << '\n';
// std::cout << "wall = " << std::hex << reinterpret_cast<std::uintptr_t>(wallPtr.get()) << '\n';
// std::cout << "wall.shape = " << std::hex << reinterpret_cast<std::uintptr_t>(wallShapePtr.get())
//           << '\n';

TEST_F(AlvinCollisionTest, PlayerAndWall_PlayerCollidesWithWall_ThenMovesAway) {
    TestDecorator<Player> player{
        _space,
        {16.0, 16.0}
    };
    TestDecorator<Wall> wall{
        _space,
        {100.0, 16.0}
    };

    DoSpaceStep(); // First step

    player.expectCollisionWith(wall, CONTACT | PRE_SOLVE | POST_SOLVE);
    wall.expectCollisionWith(player, CONTACT | PRE_SOLVE | POST_SOLVE);
    player.setPosition({90.0, 16.0}); // Bodies come in contact
    DoSpaceStep();
    Mock::VerifyAndClearExpectations(&player.getColllisionCallback());
    Mock::VerifyAndClearExpectations(&wall.getColllisionCallback());

    player.expectCollisionWith(wall, SEPARATE);
    wall.expectCollisionWith(player, SEPARATE);
    player.setPosition({16.0, 16.0}); // Bodies separate
    DoSpaceStep();
    Mock::VerifyAndClearExpectations(&player.getColllisionCallback());
    Mock::VerifyAndClearExpectations(&wall.getColllisionCallback());
}

TEST_F(AlvinCollisionTest, PlayerAndWall_PlayerCollidesWithWall_BothDestroyedWhileInContact) {
    TestDecorator<Player> player{
        _space,
        {16.0, 16.0}
    };
    TestDecorator<Wall> wall{
        _space,
        {100.0, 16.0}
    };

    DoSpaceStep(); // First step

    player.expectCollisionWith(wall, CONTACT | PRE_SOLVE | POST_SOLVE);
    wall.expectCollisionWith(player, CONTACT | PRE_SOLVE | POST_SOLVE);
    player.setPosition({90.0, 16.0}); // Bodies come in contact
    DoSpaceStep();
    Mock::VerifyAndClearExpectations(&player.getColllisionCallback());
    Mock::VerifyAndClearExpectations(&wall.getColllisionCallback());

    player.expectCollisionWith(wall, SEPARATE);
    wall.expectCollisionWith(player, SEPARATE);
}

TEST_F(AlvinCollisionTest, PlayerAndWall_PlayerCollidesWithWall_PlayerRejectsCollisionInContact) {
    TestDecorator<Player> player{
        _space,
        {16.0, 16.0}
    };
    TestDecorator<Wall> wall{
        _space,
        {100.0, 16.0}
    };

    DoSpaceStep(); // First step

    player.expectCollisionWith(wall, CONTACT, REJECT_COLLISION);
    wall.expectCollisionWith(player, CONTACT);
    player.setPosition({90.0, 16.0}); // Bodies come in contact
    DoSpaceStep();
    Mock::VerifyAndClearExpectations(&player.getColllisionCallback());
    Mock::VerifyAndClearExpectations(&wall.getColllisionCallback());

    DoSpaceStep(); // We simulate more steps but the collision isn't processed

    // Objects are notified of the separation anyway
    player.expectCollisionWith(wall, SEPARATE);
    wall.expectCollisionWith(player, SEPARATE);
}

TEST_F(AlvinCollisionTest, PlayerAndWall_PlayerCollidesWithWall_WallRejectsCollisionInPreSolve) {
    TestDecorator<Player> player{
        _space,
        {16.0, 16.0}
    };
    TestDecorator<Wall> wall{
        _space,
        {100.0, 16.0}
    };

    DoSpaceStep(); // First step

    player.expectCollisionWith(wall, CONTACT | PRE_SOLVE);
    wall.expectCollisionWith(player, CONTACT | PRE_SOLVE, ACCEPT_COLLISION, REJECT_COLLISION);
    player.setPosition({90.0, 16.0}); // Bodies come in contact
    DoSpaceStep();
    Mock::VerifyAndClearExpectations(&player.getColllisionCallback());
    Mock::VerifyAndClearExpectations(&wall.getColllisionCallback());

    player.expectCollisionWith(wall, PRE_SOLVE | POST_SOLVE);
    wall.expectCollisionWith(player, PRE_SOLVE | POST_SOLVE); // In 2nd step we'll accept the collision
    DoSpaceStep();
    Mock::VerifyAndClearExpectations(&player.getColllisionCallback());
    Mock::VerifyAndClearExpectations(&wall.getColllisionCallback());

    // Objects are notified of the separation anyway
    player.expectCollisionWith(wall, SEPARATE);
    wall.expectCollisionWith(player, SEPARATE);
}

} // namespace hobgoblin
} // namespace jbatnozic
