// Copyright 2024 Jovan Batnozic. Released under MS-PL licence in Serbia.
// See https://github.com/jbatnozic/Hobgoblin?tab=readme-ov-file#licence

#define HOBGOBLIN_SHORT_NAMESPACE
#include <Hobgoblin/Alvin.hpp>
#include <Hobgoblin/ChipmunkPhysics.hpp>
#include <Hobgoblin/Graphics.hpp>
#include <Hobgoblin/Math.hpp>

#include <iostream>
#include <list>

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
    static constexpr cpBitmask ENTITY_DEFAULT_MASK     = CAT_PLAYER | CAT_ENEMY | CAT_LOOT | CAT_WALL;
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
class HealthPickUpInterface : public LootInterface {
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

void Init(alvin::MainCollisionDispatcher& aDispatcher, NeverNull<cpSpace*> aSpace) {
    aDispatcher.registerEntityType<PlayerInterface>();
    aDispatcher.registerEntityType<EnemyInterface>();
    aDispatcher.registerEntityType<LootInterface>();
    aDispatcher.registerEntityType<HealthPickUpInterface>();
    aDispatcher.registerEntityType<WallInterface>();
    aDispatcher.configureSpace(aSpace);
}

///////////////////////////////////////////////////////////////////////////////

NeverNull<cpShape*> GetOtherShape(NeverNull<cpArbiter*> aArbiter, cpShape* aMyShape) {
    CP_ARBITER_GET_SHAPES(aArbiter, shape1, shape2);
    if (shape1 == aMyShape) {
        return shape2;
    }
    return shape1;
}

class Player : public PlayerInterface {
public:
    Player(NeverNull<cpSpace*> aSpace,
           math::Vector2d      aPosition,
           cpGroup             aGroup,
           cpBitmask           aCategory,
           cpBitmask           aCollidesWith)
        : _colDelegate{_initColDelegate()}
        , _body{alvin::Body::createDynamic(100.0, cpMomentForCircle(100.0, 0.0, 16.0, cpvzero))}
        , _shape{alvin::Shape::createCircle(_body, 16.0, cpvzero)} {
        _colDelegate.bind(*this, _shape, aGroup, aCategory, aCollidesWith);
        cpSpaceAddBody(aSpace, _body);
        setPosition(aPosition);
        cpSpaceAddShape(aSpace, _shape);
    }

    Player(NeverNull<cpSpace*> aSpace, math::Vector2d aPosition)
        : Player{aSpace,
        aPosition,
        CP_NO_GROUP,
        PlayerInterface::ENTITY_DEFAULT_CATEGORY,
        PlayerInterface::ENTITY_DEFAULT_MASK} {}

    void setPosition(math::Vector2d aPosition) {
        cpBodySetPosition(_body, cpv(aPosition.x, aPosition.y));
    }

    NeverNull<const cpShape*> getShape() const {
        return _shape;
    }

    int getHealth() const {
        return _health;
    }

private:
    alvin::CollisionDelegate _colDelegate;
    alvin::Body              _body;
    alvin::Shape             _shape;

    int _health = 100;

    alvin::CollisionDelegate _initColDelegate() {
        auto builder = alvin::CollisionDelegateBuilder{};
        return builder.finalize();
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

    NeverNull<const cpShape*> getShape() const {
        return _shape;
    }

private:
    alvin::CollisionDelegate _colDelegate;
    alvin::Body              _body;
    alvin::Shape             _shape;

    alvin::CollisionDelegate _initColDelegate() {
        auto builder = alvin::CollisionDelegateBuilder{};
        return builder.finalize();
    }
};

constexpr auto ACCEPT_COLLISION = alvin::Decision::ACCEPT_COLLISION;
constexpr auto REJECT_COLLISION = alvin::Decision::REJECT_COLLISION;

#define CONTACT    (0x01 << alvin::detail::USAGE_COL_BEGIN)
#define PRE_SOLVE  (0x01 << alvin::detail::USAGE_COL_PRESOLVE)
#define POST_SOLVE (0x01 << alvin::detail::USAGE_COL_POSTSOLVE)
#define SEPARATE   (0x01 << alvin::detail::USAGE_COL_SEPARATE)

} // namespace hobgoblin
} // namespace jbatnozic

namespace alvin = hg::alvin;

std::list<Wall> SetUpArenaEdges(hg::NeverNull<cpSpace*> aSpace, hg::PZInteger aWidth, hg::PZInteger aHeight) {
    for (hg::PZInteger y = 0; y < aHeight; y += 1) {
        for (hg::PZInteger x = 0; x < aWidth; x += 1) {
            if (!(x == 0 || y == 0 || x == aWidth - 1 || y == aHeight - 1)) {
                continue;
            }
            // TODO: 
        }
    }
}

int main(int argc, char* argv[]) {
    hg::gr::RenderWindow window;
    window.setFramerateLimit(60);

    alvin::Space space;

    std::list<Player> players;

    while (window.isOpen()) {
        hg::win::Event ev;
        while (window.pollEvent(ev)) {
            ev.visit([&window](const hg::win::Event::Closed&) {
                window.close();
            });
        }

        space.step(1.0 / 60.0);

        window.clear();
        window.display();
    }

    return EXIT_SUCCESS;
}
