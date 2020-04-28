#ifndef SPRITES_HPP
#define SPRITES_HPP

#include <Hobgoblin/Graphics.hpp>

enum class SpriteId {
    None = -1,

    // Terrain
    BlockEmpty = 0,
    BlockWall,

    // Actors
    Player
};

inline
hg::gr::SpriteLoader LoadAllSprites() {
    hg::gr::SpriteLoader loader;
    auto texMain = loader.addTexture(2048, 2048);

    return loader;
}

#endif
