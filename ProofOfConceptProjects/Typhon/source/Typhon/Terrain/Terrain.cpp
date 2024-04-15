// Copyright 2024 Jovan Batnozic. Released under MS-PL licence in Serbia.
// See https://github.com/jbatnozic/Hobgoblin?tab=readme-ov-file#licence

// clang-format off


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

// clang-format on
