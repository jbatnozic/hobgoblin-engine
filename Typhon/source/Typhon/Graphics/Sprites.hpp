#ifndef SPRITES_HPP
#define SPRITES_HPP

#include <Hobgoblin/Graphics.hpp>

enum class SpriteId {
    None = -1,

    // Terrain
    CaveFloor = 0,
    CaveWall,

    // Actors
    Player
};

inline
hg::gr::SpriteLoader LoadAllSprites() {
    hg::gr::SpriteLoader loader;
    auto texMain = loader.addTexture(2048, 2048);

#define INT(x) static_cast<int>(x)
    loader
        .loadFromFile(texMain, INT(SpriteId::CaveFloor), 0, "res/CaveFloor.png")
        .loadFromFile(texMain, INT(SpriteId::CaveWall), 0, "res/CaveWall.png")
        .finalize(hg::gr::TexturePackingHeuristic::BestAreaFit);
#undef INT

    return loader;
}

#endif
