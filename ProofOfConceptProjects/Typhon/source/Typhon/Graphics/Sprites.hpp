// Copyright 2024 Jovan Batnozic. Released under MS-PL licence in Serbia.
// See https://github.com/jbatnozic/Hobgoblin?tab=readme-ov-file#licence

// clang-format off

#ifndef SPRITES_HPP
#define SPRITES_HPP

#include <Hobgoblin/Graphics.hpp>

enum class SpriteId {
    None = -1,

    // Terrain
    CaveFloor = 0,
    CaveWall,
    EnergyShield,
    Space,

    // Actors
    Player,
    Ship,

    // Other
    Plasma,
    WhiteGlow
};

inline
hg::gr::SpriteLoader LoadAllSprites() {
    hg::gr::SpriteLoader loader;
    auto texMain = loader.addTexture(2048, 2048);

#define INT(x) static_cast<int>(x)
    loader
        .loadFromFile(texMain, INT(SpriteId::CaveFloor), 0, "res/CaveFloor.png")
        .loadFromFile(texMain, INT(SpriteId::CaveWall), 0, "res/CaveWall.png")
        .loadFromFile(texMain, INT(SpriteId::Ship), 0, "res/Ship.png")
        .loadFromFile(texMain, INT(SpriteId::EnergyShield), 0, "res/EnergyShield.png")
        .loadFromFile(texMain, INT(SpriteId::Space), 0, "res/Space.png")
        .loadFromFile(texMain, INT(SpriteId::Plasma), 0, "res/Plasma.png")
        .loadFromFile(texMain, INT(SpriteId::WhiteGlow), 0, "res/WhiteGlow.png")
        .finalize(hg::gr::TexturePackingHeuristic::BestAreaFit);
#undef INT

    return loader;
}

#endif

// clang-format on
