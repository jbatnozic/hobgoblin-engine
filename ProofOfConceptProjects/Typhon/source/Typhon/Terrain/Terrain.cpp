
#include "Terrain.hpp"

const Terrain::TypeProperties& Terrain::getTypeProperties(TypeId typeId) {
    return getInstance().typeProperties[static_cast<std::size_t>(typeId)];
}

void Terrain::initializeSingleton() {
    getInstance();
}

Terrain::Terrain() {
    defineTerrainTypes();
}

const Terrain& Terrain::getInstance() {
    static Terrain singleton;
    return singleton;
}