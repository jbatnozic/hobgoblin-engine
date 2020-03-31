
#define HOBGOBLIN_SHORT_NAMESPACE
#include <Hobgoblin/Graphics/Sprite_manager.hpp>

#include <iostream>

enum Sprites {
    Player, Enemy, Block
};

int main() {
    hg::gr::SpriteManager sm;
    auto tex = sm.addTexture(800, 800);

    sm.loadFromFile(tex, Player, 0, "sprites/player.png");
    sm.loadFromFile(tex, Enemy, 0, "sprites/enemy.png");
    sm.loadFromFile(tex, Block, 0, "sprites/block.png");
    sm.finalize();

    std::cin.get();
    return 0;
}