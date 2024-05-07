// Copyright 2024 Jovan Batnozic. Released under MS-PL licence in Serbia.
// See https://github.com/jbatnozic/Hobgoblin?tab=readme-ov-file#licence

// clang-format off

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
    public:
        static constexpr bool ACCEPT_COLLISION = true;
        static constexpr bool REJECT_COLLISION = false;

    private:
        virtual bool collisionBegin(ICreature*, cpArbiter*) const { return ACCEPT_COLLISION; }
        virtual bool collisionPreSolve(ICreature*, cpArbiter*) const { return ACCEPT_COLLISION; }
        virtual void collisionPostSolve(ICreature*, cpArbiter*) {}
        virtual void collisionSeparate(ICreature*, cpArbiter*) {}

        virtual bool collisionBegin(IInteractible*, cpArbiter*) const { return ACCEPT_COLLISION; }
        virtual bool collisionPreSolve(IInteractible*, cpArbiter*) const { return ACCEPT_COLLISION; }
        virtual void collisionPostSolve(IInteractible*, cpArbiter*) {}
        virtual void collisionSeparate(IInteractible*, cpArbiter*) {}

        virtual bool collisionBegin(IProjectile*, cpArbiter*) const { return ACCEPT_COLLISION; }
        virtual bool collisionPreSolve(IProjectile*, cpArbiter*) const { return ACCEPT_COLLISION; }
        virtual void collisionPostSolve(IProjectile*, cpArbiter*) {}
        virtual void collisionSeparate(IProjectile*, cpArbiter*) {}

        virtual bool collisionBegin(IStructure*, cpArbiter*) const { return ACCEPT_COLLISION; }
        virtual bool collisionPreSolve(IStructure*, cpArbiter*) const { return ACCEPT_COLLISION; }
        virtual void collisionPostSolve(IStructure*, cpArbiter*) {}
        virtual void collisionSeparate(IStructure*, cpArbiter*) {}

        virtual bool collisionBegin(ITerrain*, cpArbiter*) const { return ACCEPT_COLLISION; }
        virtual bool collisionPreSolve(ITerrain*, cpArbiter*) const { return ACCEPT_COLLISION; }
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
};

#endif // !COLLISIONS_HPP

// clang-format on
