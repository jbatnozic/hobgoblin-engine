#ifndef COLLISIONS_HPP
#define COLLISIONS_HPP

#include <Hobgoblin/ChipmunkPhysics.hpp>

class Collideables {
public:
    // Collision categories
    #define COLLIDES_WITH_EVERYTHING CP_ALL_CATEGORIES
    enum CC : cpBitmask {
        __Creature,
        __Interactible,
        __Projectile,
        __Structure,
        __Terrain,

        Creature     = (1 << __Creature),
        Interactible = (1 << __Interactible),
        Projectile   = (1 << __Projectile),
        Structure    = (1 << __Structure),
        Terrain      = (1 << __Terrain),

        CreatureMask     = COLLIDES_WITH_EVERYTHING,
        InteractibleMask = COLLIDES_WITH_EVERYTHING,
        ProjectileMask   = Creature | Terrain | Structure,
        StructureMask    = COLLIDES_WITH_EVERYTHING,
        TerrainMask      = COLLIDES_WITH_EVERYTHING,
    };
    #undef COLLIDES_WITH_EVERYTHING
    
    // Collideable interfaces
    class ICreature;
    class IInteractible;
    class IProjectile;
    class IStructure;
    class ITerrain;

    class __IBase {
    private:
        virtual cpBool collisionBegin(ICreature*, cpArbiter*) { return cpTrue; }
        virtual cpBool collisionPreSolve(ICreature*, cpArbiter*) { return cpTrue; }
        virtual void collisionPostSolve(ICreature*, cpArbiter*) {}
        virtual void collisionSeparate(ICreature*, cpArbiter*) {}

        virtual cpBool collisionBegin(IInteractible*, cpArbiter*) { return cpTrue; }
        virtual cpBool collisionPreSolve(IInteractible*, cpArbiter*) { return cpTrue; }
        virtual void collisionPostSolve(IInteractible*, cpArbiter*) {}
        virtual void collisionSeparate(IInteractible*, cpArbiter*) {}

        virtual cpBool collisionBegin(IProjectile*, cpArbiter*) { return cpTrue; }
        virtual cpBool collisionPreSolve(IProjectile*, cpArbiter*) { return cpTrue; }
        virtual void collisionPostSolve(IProjectile*, cpArbiter*) {}
        virtual void collisionSeparate(IProjectile*, cpArbiter*) {}

        virtual cpBool collisionBegin(IStructure*, cpArbiter*) { return cpTrue; }
        virtual cpBool collisionPreSolve(IStructure*, cpArbiter*) { return cpTrue; }
        virtual void collisionPostSolve(IStructure*, cpArbiter*) {}
        virtual void collisionSeparate(IStructure*, cpArbiter*) {}

        virtual cpBool collisionBegin(ITerrain*, cpArbiter*) { return cpTrue; }
        virtual cpBool collisionPreSolve(ITerrain*, cpArbiter*) { return cpTrue; }
        virtual void collisionPostSolve(ITerrain*, cpArbiter*) {}
        virtual void collisionSeparate(ITerrain*, cpArbiter*) {}

        friend class Collideables;
    };

    class ICreature : public __IBase {
    public:
    };

    class IInteractible : public __IBase {
    public:
    };

    class IProjectile : public __IBase {
    public:
    };

    class IStructure : public __IBase {
    public:
    };

    class ITerrain : public __IBase {
    public:
    };

    // Functions for setting up physics shapes as collideables
    static void initCreature(cpShape* shape, ICreature& collideable);
    static void initInteractible(cpShape* shape, IInteractible& collideable);
    static void initProjectile(cpShape* shape, IProjectile& collideable);
    static void initStructure(cpShape* shape, IStructure& collideable);
    static void initTerrain(cpShape* shape, ITerrain& collideable);

    // Space init
    static void installCollisionHandlers(cpSpace* space);

private:
    template <class TypeA, class TypeB>
    static void populateCollisionHandler(cpCollisionHandler* handler);

    static_assert(sizeof(cpCollisionType) == sizeof(cpBitmask)); // Because they are used interchangably
};

#endif // !COLLISIONS_HPP

