
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

void Collideables::initProjectile(cpShape* shape, IProjectile& collideable) {
    cpShapeSetCollisionType(shape, static_cast<cpCollisionType>(CC::Projectile));
    cpShapeSetFilter(shape, CreateFilter(CC::Projectile, CC::ProjectileMask));

    assert(cpShapeGetUserData(shape) == nullptr);
    cpShapeSetUserData(shape, std::addressof(collideable));
}

void Collideables::initTerrain(cpShape* shape) {
    cpShapeSetCollisionType(shape, static_cast<cpCollisionType>(CC::Terrain));
    cpShapeSetFilter(shape, CreateFilter(CC::Terrain, CC::TerrainMask));
}