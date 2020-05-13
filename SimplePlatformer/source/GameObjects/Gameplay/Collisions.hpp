#ifndef COLLISIONS_HPP
#define COLLISIONS_HPP

#include <Hobgoblin/ChipmunkPhysics.hpp>

#include <iostream>

class Collideables {
public:
    // Collision categories
    #define COLLIDES_WITH_EVERYTHING CP_ALL_CATEGORIES
    enum CC : cpBitmask {
        _Creature,
        _Projectile,
        _Terrain,

        Creature   = (1 << _Creature),
        Projectile = (1 << _Projectile),
        Terrain    = (1 << _Terrain),

        CreatureMask   = COLLIDES_WITH_EVERYTHING,
        ProjectileMask = Creature | Terrain,
        TerrainMask    = COLLIDES_WITH_EVERYTHING,
    };
    #undef COLLIDES_WITH_EVERYTHING
    
    // Collideable interfaces
    class ICreature {
    public:
        // TODO
    };

    class IProjectile {
    public:
        // TODO
    };

    // Functions for setting up physics shapes as collideables
    static void initCreature(cpShape* shape, ICreature& collideable);
    static void initProjectile(cpShape* shape, IProjectile& collideable);
    static void initTerrain(cpShape* shape);

    // Space init
    static void initPhysicsSpace(cpSpace* space) {
        auto* handler = addHandler(space, CC::Creature, CC::Projectile);
        handler->beginFunc = [](cpArbiter* arbiter, cpSpace* space, cpDataPointer userData) -> cpBool {
            /*cpBody* bodyA;
            cpBody* bodyB;
            cpArbiterGetBodies(arbiter, &bodyA, &bodyB);
            auto filter = cpShapeFilterNew(0, 1, 2);
            cpShapeSetFilter(shape, cpShapeFilterNew(CP_NO_GROUP, 0, 0));*/
            std::cout << "hit\n";
            return cpTrue;
        };
    }

private:
    static cpCollisionHandler* addHandler(cpSpace* space, CC categoryA, CC categoryB) {
        return cpSpaceAddCollisionHandler(space, 
                                          static_cast<cpCollisionType>(categoryA),
                                          static_cast<cpCollisionType>(categoryB));
    }

    static_assert(sizeof(cpCollisionType) == sizeof(cpBitmask)); // Because they are used interchangably
};

#endif // !COLLISIONS_HPP

