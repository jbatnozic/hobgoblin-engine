#pragma once

#include <Hobgoblin/Common.hpp>
#include <SFML/Graphics.hpp>

#include <functional>
#include <vector>

#include "Game_object_framework.hpp"

class IsometricTester : public GOF_Base {
public:
    static constexpr int SIZE = 8;

    IsometricTester(QAO_Runtime* runtime)
        : GOF_Base(runtime, TYPEID_SELF, 0, "IsometricTester")
        , _building{2, 2, 1, 1, 3, 2}
    {
        // 16x16 grid (row (x) major)
        _blocks.reserve(SIZE * SIZE);
        for (int y = 0; y < SIZE; y += 1) {
            for (int x = 0; x < SIZE; x += 1) {
                _blocks.push_back(Block{x, y, 0, 1, 1, 1});
            }
        }

        // load sprits
        blockTexture.loadFromFile("res/block.png");
        blockSprite.setTexture(blockTexture);
        blockSprite.setOrigin({0.f, 23.f});

        buildingTexture.loadFromFile("res/building.png");
        buildingSprite.setTexture(buildingTexture);
        buildingSprite.setOrigin({7.f, 209.f});
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
    
    Block _building;

    using bdfp_t = std::pair<const Block*, std::function<void(sf::RenderTarget&)>>;

    sf::Texture blockTexture;
    sf::Sprite blockSprite;

    sf::Texture buildingTexture;
    sf::Sprite buildingSprite;

    std::vector<Block> _blocks; // Grid
    std::vector<bdfp_t> _drawBuffer;
};