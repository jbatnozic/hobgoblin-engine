// Copyright 2024 Jovan Batnozic. Released under MS-PL licence in Serbia.
// See https://github.com/jbatnozic/Hobgoblin?tab=readme-ov-file#licence

// clang-format off


#include <cassert>

#include "Collisions.hpp"

namespace {

cpShapeFilter CreateFilter(Collideables::CC selfCategory, Collideables::CC collidesWith) {
    return cpShapeFilterNew(CP_NO_GROUP,
                            static_cast<cpBitmask>(selfCategory),
                            static_cast<cpBitmask>(collidesWith));
}

} // namespace

void Collideables::initCreature(cpShape* shape, ICreature& collideable) {
    cpShapeSetCollisionType(shape, static_cast<cpCollisionType>(CC::Creature));
    cpShapeSetFilter(shape, CreateFilter(CC::Creature, CC::CreatureMask));

    assert(cpShapeGetUserData(shape) == nullptr);
    cpShapeSetUserData(shape, std::addressof(collideable));
}

void Collideables::initInteractible(cpShape* shape, IInteractible& collideable) {
    cpShapeSetCollisionType(shape, static_cast<cpCollisionType>(CC::Interactible));
    cpShapeSetFilter(shape, CreateFilter(CC::Interactible, CC::InteractibleMask));

    assert(cpShapeGetUserData(shape) == nullptr);
    cpShapeSetUserData(shape, std::addressof(collideable));
}

void Collideables::initProjectile(cpShape* shape, IProjectile& collideable) {
    cpShapeSetCollisionType(shape, static_cast<cpCollisionType>(CC::Projectile));
    cpShapeSetFilter(shape, CreateFilter(CC::Projectile, CC::ProjectileMask));

    assert(cpShapeGetUserData(shape) == nullptr);
    cpShapeSetUserData(shape, std::addressof(collideable));
}

void Collideables::initStructure(cpShape* shape, IStructure& collideable) {
    cpShapeSetCollisionType(shape, static_cast<cpCollisionType>(CC::Structure));
    cpShapeSetFilter(shape, CreateFilter(CC::Structure, CC::StructureMask));

    assert(cpShapeGetUserData(shape) == nullptr);
    cpShapeSetUserData(shape, std::addressof(collideable));
}

void Collideables::initTerrain(cpShape* shape, ITerrain& collideable) {
    cpShapeSetCollisionType(shape, static_cast<cpCollisionType>(CC::Terrain));
    cpShapeSetFilter(shape, CreateFilter(CC::Terrain, CC::TerrainMask));

    assert(cpShapeGetUserData(shape) == nullptr);
    cpShapeSetUserData(shape, std::addressof(collideable));
}

void Collideables::installCollisionHandlers(cpSpace* space) {
#define CREATE_HANDLER(_type_a_, _type_b_) \
    populateCollisionHandler<I##_type_a_, I##_type_b_>( \
        cpSpaceAddCollisionHandler(space, CC::_type_a_, CC::_type_b_) \
    )

    // Creature
    CREATE_HANDLER(Creature, Creature);
    CREATE_HANDLER(Creature, Interactible);
    CREATE_HANDLER(Creature, Projectile);
    CREATE_HANDLER(Creature, Structure);
    CREATE_HANDLER(Creature, Terrain);

    // Interactible
    //CREATE_HANDLER(Interactible, Creature);
    CREATE_HANDLER(Interactible, Interactible);
    CREATE_HANDLER(Interactible, Projectile);
    CREATE_HANDLER(Interactible, Structure);
    CREATE_HANDLER(Interactible, Terrain);

    // Projectile
    //CREATE_HANDLER(Projectile, Creature);
    //CREATE_HANDLER(Projectile, Interactible);
    CREATE_HANDLER(Projectile, Projectile);
    CREATE_HANDLER(Projectile, Structure);
    CREATE_HANDLER(Projectile, Terrain);

    // Structure
    ///CREATE_HANDLER(Structure, Creature);
    ///CREATE_HANDLER(Structure, Interactible);
    ///CREATE_HANDLER(Structure, Projectile);
    CREATE_HANDLER(Structure, Structure);
    CREATE_HANDLER(Structure, Terrain);

    // Terrain
    //CREATE_HANDLER(Terrain, Creature);
    //CREATE_HANDLER(Terrain, Interactible);
    //CREATE_HANDLER(Terrain, Projectile);
    //CREATE_HANDLER(Terrain, Structure);
    CREATE_HANDLER(Terrain, Terrain);

#undef CREATE_HANDLER
}

template <class TypeA, class TypeB>
void Collideables::populateCollisionHandler(cpCollisionHandler* handler) {
    handler->beginFunc = [](cpArbiter* arbiter, cpSpace* space, cpDataPointer userData) -> cpBool {
        CP_ARBITER_GET_SHAPES(arbiter, shapeA, shapeB);
        TypeA* A = cpShapeGetUserData(shapeA).get<TypeA>();
        TypeB* B = cpShapeGetUserData(shapeB).get<TypeB>();
        cpBool predicateA = static_cast<cpBool>(A->collisionBegin(B, arbiter));
        cpBool predicateB = static_cast<cpBool>(B->collisionBegin(A, arbiter));
        return (predicateA && predicateB);
    };
    handler->preSolveFunc = [](cpArbiter* arbiter, cpSpace* space, cpDataPointer userData) -> cpBool {
        CP_ARBITER_GET_SHAPES(arbiter, shapeA, shapeB);
        TypeA* A = cpShapeGetUserData(shapeA).get<TypeA>();
        TypeB* B = cpShapeGetUserData(shapeB).get<TypeB>();
        cpBool predicateA = static_cast<cpBool>(A->collisionPreSolve(B, arbiter));
        cpBool predicateB = static_cast<cpBool>(B->collisionPreSolve(A, arbiter));
        return (predicateA && predicateB);
    };
    handler->postSolveFunc = [](cpArbiter* arbiter, cpSpace* space, cpDataPointer userData) -> void {
        CP_ARBITER_GET_SHAPES(arbiter, shapeA, shapeB);
        TypeA* A = cpShapeGetUserData(shapeA).get<TypeA>();
        TypeB* B = cpShapeGetUserData(shapeB).get<TypeB>();
        A->collisionPostSolve(B, arbiter);
        B->collisionPostSolve(A, arbiter);
    };
    handler->separateFunc = [](cpArbiter* arbiter, cpSpace* space, cpDataPointer userData) -> void {
        CP_ARBITER_GET_SHAPES(arbiter, shapeA, shapeB);
        TypeA* A = cpShapeGetUserData(shapeA).get<TypeA>();
        TypeB* B = cpShapeGetUserData(shapeB).get<TypeB>();
        A->collisionSeparate(B, arbiter);
        B->collisionSeparate(A, arbiter);
    };
}

// clang-format on
