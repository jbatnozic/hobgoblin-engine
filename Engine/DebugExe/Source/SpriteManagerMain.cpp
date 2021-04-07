
#define HOBGOBLIN_SHORT_NAMESPACE
#include <Hobgoblin/Graphics/Sprite_loader.hpp>
#include <Hobgoblin/Graphics/Texture_packing.hpp>
#include <Hobgoblin/Utility/Exceptions.hpp>
#include <SFML/Graphics.hpp>

#include <filesystem>
#include <iostream>

enum Sprites {
    Player, Enemy, Block, Misc, Dir
};

using hg::gr::AUTO_INDEX;

#define SPRITE_LOADER_TEST

int main() {
    std::filesystem::path path("asdf");

#ifdef SPRITE_LOADER_TEST
#define WIDTH 800
#define HEIGHT 800

    hg::gr::SpriteLoader sprLd;
    auto tex = sprLd.addTexture(WIDTH, HEIGHT);

    try {
        sprLd
            .loadFromFile(tex, Player, 0, "sprites/player.png")
            .loadFromFile(tex, Enemy, 0, "sprites/enemy.png")
            .loadFromFile(tex, Block, 0, "sprites/block.png")
            .loadFromFile(tex, Misc, AUTO_INDEX, "sprites/block.png")
            .loadFromFile(tex, Misc, AUTO_INDEX, "sprites/block.png")
            .loadFromFile(tex, Misc, AUTO_INDEX, "sprites/Brick.png")
            .loadFromFile(tex, Misc, AUTO_INDEX, "sprites/Btn_UnDrop.png")
            .loadFromFile(tex, Misc, AUTO_INDEX, "sprites/obj_window_0.png")
            .loadFromFile(tex, Misc, AUTO_INDEX, "sprites/spr_ctrl_camera_foot_0.png")
            .loadFromFile(tex, Misc, AUTO_INDEX, "sprites/spr_enemy_brain_0.png")
            .loadFromFile(tex, Misc, AUTO_INDEX, "sprites/spr_item_consumable_default_0.png")
            .loadFromFile(tex, Misc, AUTO_INDEX, "sprites/spr_tom_0.png")
            .loadFromDirectory(tex, Dir, "sprites")
            .finalize(hg::gr::TexturePackingHeuristic::BestAreaFit);
    }
    catch (std::exception& ex) {
        std::cout << "CAUGHT EXCEPTION: " << ex.what() << '\n';
        std::cin.get();
        return EXIT_FAILURE;
    }

    sf::RenderWindow window(sf::VideoMode(WIDTH, HEIGHT), "Sprite loader");

    while (window.isOpen()) {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed) {
                window.close();
            }
        }

        window.clear();

        sf::Sprite texSprite;
        texSprite.setTexture(sprLd.getTexture(tex));
        texSprite.setTextureRect({0, 0, WIDTH, HEIGHT});
        window.draw(texSprite);

        window.display();
    }

    std::cin.get();
#endif
#ifdef TEXTURE_PACKING_TEST
#define WIDTH 600
#define HEIGHT 600

    sf::RenderWindow window(sf::VideoMode(WIDTH, HEIGHT), "Texture packer");
    sf::Texture texture{};
    texture.create(WIDTH, HEIGHT);

    sf::Image img0;
    sf::Image img1;
    sf::Image img2;
    sf::Image img3;
    sf::Image img4;
    sf::Image img5;
    sf::Image img6;
    sf::Image img7;
    sf::Image img8;
    sf::Image img9;

    img0.loadFromFile("sprites/Block.png");
    img1.loadFromFile("sprites/Brick.png");
    img2.loadFromFile("sprites/Btn_UnDrop.png");
    img3.loadFromFile("sprites/Enemy.png");
    img4.loadFromFile("sprites/obj_window_0.png");
    img5.loadFromFile("sprites/Player.png");
    img6.loadFromFile("sprites/spr_ctrl_camera_foot_0.png");
    img7.loadFromFile("sprites/spr_enemy_brain_0.png");
    img8.loadFromFile("sprites/spr_item_consumable_default_0.png");
    img9.loadFromFile("sprites/spr_tom_0.png");

    std::vector<sf::Image*> imageAddresses;
    imageAddresses.push_back(&img0);
    imageAddresses.push_back(&img1);
    imageAddresses.push_back(&img2);
    imageAddresses.push_back(&img3);
    imageAddresses.push_back(&img4);
    imageAddresses.push_back(&img5);
    imageAddresses.push_back(&img6);
    imageAddresses.push_back(&img7);
    imageAddresses.push_back(&img8);
    imageAddresses.push_back(&img9);

    try {
        float occupancy;
        auto results = hg::gr::PackTexture(texture, imageAddresses,
                                           hg::gr::TexturePackingHeuristic::BestAreaFit, &occupancy);

        std::cout << "Occupancy = " << occupancy << '\n';

        sf::Sprite texSprite;
        texSprite.setTexture(texture);
        texSprite.setTextureRect({0, 0, WIDTH, HEIGHT});

        while (window.isOpen()) {
            sf::Event event;
            while (window.pollEvent(event)) {
                if (event.type == sf::Event::Closed) {
                    window.close();
                }
            }

            window.clear();

            window.draw(texSprite);

            window.display();
        }
    }
    catch (std::exception& ex) {
        std::cout << "CAUGHT EXCEPTION: " << ex.what() << '\n';
        std::cin.get();
    }
#endif

    return 0;
}