#pragma once

#include <Hobgoblin/Common.hpp>
#include <SFML/Graphics.hpp>

#include <functional>
#include <vector>

#include "Object_framework.hpp"

class IsometricTester : public GameObject {
public:
    static constexpr int SIZE = 16;

    IsometricTester()
        : GameObject(0, 0, "IsometricTester")
    {
        // 16x16 grid (row (x) major)
        _blocks.reserve(SIZE * SIZE);
        for (int y = 0; y < SIZE; y += 1) {
            for (int x = 0; x < SIZE; x += 1) {
                _blocks.push_back(Block{x, SIZE - y - 1, 0, 1, 1, 1});
            }
        }

        // load sprite
        blockTexture.loadFromFile("res/block.png");
        blockSprite.setTexture(blockTexture);
    }

    void eventDraw1() override;

    void eventDraw2() override;

private:
    struct Block {
        int x, y, z;
        int xSize, ySize, zSize;

        Block(int x, int y, int z, int xSize, int ySize, int zSize) 
            : x{x}, y{y}, z{z}, xSize{xSize}, ySize{ySize}, zSize{zSize}
        {
        }
    };
    
    using bdfp_t = std::pair<const Block*, std::function<void(sf::RenderTarget&)>>;

    sf::Texture blockTexture;
    sf::Sprite blockSprite;

    std::vector<Block> _blocks; // Grid
    std::vector<bdfp_t> _drawBuffer;
};