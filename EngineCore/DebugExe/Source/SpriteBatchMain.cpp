// Copyright 2024 Jovan Batnozic. Released under MS-PL licence in Serbia.
// See https://github.com/jbatnozic/Hobgoblin?tab=readme-ov-file#licence

// clang-format off


#include <Hobgoblin/Graphics.hpp>
#include <SFML/Graphics.hpp>
#include <Hobgoblin/Math.hpp>
#include <Hobgoblin/Utility/Time_utils.hpp>

#include <cmath>
#include <cstdlib>
#include <iostream>

namespace hg = jbatnozic::hobgoblin;

#define SPR_SHIP   0
#define SPR_HAZARD 1

int main()
{
    // Set up window & canvas

    sf::RenderWindow window(sf::VideoMode(800, 800), "HOBGOBLIN");
    window.setFramerateLimit(60);

    hg::gr::MultiViewRenderTargetAdapter mvAdapter{window};
    mvAdapter.setViewCount(4);
    mvAdapter.getView(0).setSize({400.f, 400.f});
    mvAdapter.getView(1).setSize({400.f, 400.f});
    mvAdapter.getView(2).setSize({400.f, 400.f});
    mvAdapter.getView(3).setSize({400.f, 400.f});
    mvAdapter.getView(0).setViewport({0.f,  0.f,  0.5f, 0.5f});
    mvAdapter.getView(2).setViewport({0.5f, 0.f,  0.5f, 0.5f});
    mvAdapter.getView(1).setViewport({0.5f, 0.5f, 0.5f, 0.5f});
    mvAdapter.getView(3).setViewport({0.f,  0.5f, 0.5f, 0.5f});
    mvAdapter.getView(0).setCenter({200.f, 200.f});
    mvAdapter.getView(1).setCenter({600.f, 200.f});
    mvAdapter.getView(2).setCenter({600.f, 600.f});
    mvAdapter.getView(3).setCenter({200.f, 600.f});

    hg::gr::DrawBatcher batcher{mvAdapter};

    // Set up sprites

    using hg::gr::SpriteLoader;
    using hg::gr::SUBSPRITE_APPEND;
    
    hg::gr::SpriteLoader loader;
    loader
        .startTexture(1024, 1024)
        .addFromFile(SPR_SHIP, SUBSPRITE_APPEND, "C:\\Users\\Jovan\\Desktop\\Ship.png")
        .addFromFile(SPR_SHIP, SUBSPRITE_APPEND, "C:\\Users\\Jovan\\Desktop\\hazard3.jpg")
        .addFromFile(SPR_HAZARD, SUBSPRITE_APPEND, "C:\\Users\\Jovan\\Desktop\\hazard3.jpg")
        .addFromFile(SPR_HAZARD, SUBSPRITE_APPEND, "C:\\Users\\Jovan\\Desktop\\Ship.png")
        .build(hg::gr::TexturePackingHeuristic::BestAreaFit);

    auto msprShip = loader.getBlueprint(SPR_SHIP).multispr();
    msprShip.setPosition({150.f, 150.f});
    msprShip.setScale({2.0, 2.f});
    msprShip.setOrigin({32.f, 32.f});

    auto msprHazard = loader.getBlueprint(SPR_HAZARD).multispr();
    msprHazard.setPosition({250.f, 150.f});
    msprHazard.setOrigin({32.f, 32.f});
    msprHazard.setScale({2.0, 2.f});
    msprHazard.setColor(sf::Color(255, 255, 255, 127));

    // Main loop

    float rot = 0.f;

    sf::RectangleShape rectsh;
    rectsh.setSize({400.f, 400.f});

    while (window.isOpen()) {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed) {
                window.close();
            }

            if (event.type == sf::Event::KeyPressed) {
                if (event.key.code == sf::Keyboard::Num0) {
                    msprShip.selectSubsprite(0);
                    msprHazard.selectSubsprite(0);
                }
                if (event.key.code == sf::Keyboard::Num1) {
                    msprShip.selectSubsprite(1);
                    msprHazard.selectSubsprite(1);
                }
            }
        }

        window.clear();

        hg::util::Stopwatch sw;
        for (int i = 0; i < 1; i += 1) {
            rectsh.setPosition({0.f, 0.f});
            rectsh.setFillColor(hg::gr::Color::DarkGreen);
            batcher.draw(rectsh);

            rectsh.setPosition({400.f, 0.f});
            rectsh.setFillColor(hg::gr::Color::DarkRed);
            batcher.draw(rectsh);

            rectsh.setPosition({0.f, 400.f});
            rectsh.setFillColor(hg::gr::Color::Gold);
            batcher.draw(rectsh);

            rectsh.setPosition({400.f, 400.f});
            rectsh.setFillColor(hg::gr::Color::DarkBlue);
            batcher.draw(rectsh);

            msprShip.setPosition(
                {
                    400.f + (float)std::cos(rot) * 300.f,
                    400.f + (float)std::sin(rot) * 300.f,
                });
            rot += static_cast<float>(hg::math::PI / 360.0);

            msprShip.setRotation(msprShip.getRotation() - 1.f);
            msprHazard.setRotation(msprHazard.getRotation() - 2.f);

            //sprite1.setPosition({static_cast<float>(std::rand() % 800), static_cast<float>(std::rand() % 800)});
            //sprite2.setPosition({static_cast<float>(std::rand() % 800), static_cast<float>(std::rand() % 800)});

            batcher.draw(msprShip);
            batcher.draw(msprHazard);
        }
        std::cout << sw.getElapsedTime<std::chrono::microseconds>().count() << "us\n";


        batcher.flush();
        window.display();
    }

    return 0;
}

// clang-format on
