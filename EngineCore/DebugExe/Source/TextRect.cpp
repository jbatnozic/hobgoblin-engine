// Copyright 2024 Jovan Batnozic. Released under MS-PL licence in Serbia.
// See https://github.com/jbatnozic/Hobgoblin?tab=readme-ov-file#licence

// clang-format off


#include <SFML/Graphics.hpp>
#include <Hobgoblin/Private/Max_rects_bin_pack.hpp> // namespace rbp

#include <algorithm>
#include <random>
#include <vector>
#include <iostream>

#define WIDTH  800
#define HEIGHT 800

std::random_device dev;
std::mt19937 rng(dev());

int Random(int min_including, int max_excluding) {
    std::uniform_int_distribution<std::mt19937::result_type> dist(min_including, max_excluding);
    return dist(rng);
}

class Sprite {
public:
    Sprite(int width, int height, sf::Color color)
        : _color{color}
    {
        _size.width = width;
        _size.height = height;
    }

    void addIntoPacker(rbp::MaxRectsBinPack& packer) {
        _rect = packer.Insert(_size.width, _size.height, false, rbp::MaxRectsBinPack::RectBestAreaFit);
        if (_rect.width * _rect.height == 0) {
            std::cout << "Couldn't fit rectangle\n";
        }
    }

    void drawSelf(sf::RenderTarget& target) {
        sf::RectangleShape rect;
        rect.setFillColor(_color);
        rect.setPosition({float(_rect.x), float(_rect.y)});
        rect.setSize({float(_rect.width), float(_rect.height)});
        target.draw(rect);
    }

    int getArea() const {
        return _size.width * _size.height;
    }

private:
    rbp::RectSize _size;
    rbp::Rect _rect;
    sf::Color _color;
};

std::vector<Sprite> MakeSprites() {
    static constexpr int AMOUNT = 125;
    std::vector<Sprite> rv;
    rv.reserve(std::size_t(AMOUNT));

    for (int i = 0; i < AMOUNT; i += 1) {
        const sf::Color col{(sf::Uint8)Random(1, 256), (sf::Uint8)Random(1, 256), (sf::Uint8)Random(1, 256)};
        rv.push_back(Sprite{Random(1, 16) * 8, Random(1, 16) * 8, col});
    }

    std::sort(rv.begin(), rv.end(), [](const Sprite& a, const Sprite& b) {
        return a.getArea() > b.getArea();
    });
    return rv;
}

int main() {

    sf::RenderWindow window(sf::VideoMode(800, 800), "Texture packer");
    rbp::MaxRectsBinPack packer;
    packer.Init(WIDTH, HEIGHT);

    std::vector<Sprite> sprites = MakeSprites();
    for (auto& sprite : sprites) {
        sprite.addIntoPacker(packer);
    }

    while (window.isOpen()) {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed) {
                window.close();
            }
        }

        window.clear();
        for (auto& sprite : sprites) {
            sprite.drawSelf(window);
        }
        window.display();
    }

    return 0;
}

// clang-format on
