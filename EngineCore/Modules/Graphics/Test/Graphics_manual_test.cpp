
#define HOBGOBLIN_SHORT_NAMESPACE
#include <Hobgoblin/Graphics.hpp>

#include <chrono>
#include <thread>
#include <iostream>

int main(int argc, char* argv[]) {
    hg::gr::RenderWindow window{};
    window.create(hg::gr::VideoMode{400, 400}, "Window");
    //window.setFramerateLimit(60);
    window.setVerticalSyncEnabled(true);

    {
        window.setViewCount(2);

        window.setView(hg::gr::View{{100.f, 200.f}, {200, 400}}, 0); // weird order of parameters
        window.getView(0).setViewport({0.f, 0.f, 0.5f, 1.f}); // left size
        window.getView(0).setEnabled(true);

        window.setView(hg::gr::View{{300.f, 200.f}, {200, 400}}, 1);
        window.getView(1).setViewport({0.5f, 0.f, 0.5f, 1.f}); // right side
        window.getView(1).setEnabled(true);
    }

    hg::gr::Texture texture;
    texture.loadFromFile("C:\\Users\\Jovan-PC\\Desktop\\Screenshot_1.png");

    hg::gr::Sprite sprite{texture};
    sprite.setPosition(200.f, 200.f);

    hg::gr::CircleShape circle{64.f};
    circle.setOrigin(64.f, 64.f);
    circle.setPosition(200.f, 200.f);
    circle.setFillColor(hg::gr::COLOR_DARK_ORANGE.withAlpha(125));

    while (window.isOpen()) {
        hg::gr::Event ev;
        while (window.pollEvent(ev)) {}

        window.clear(hg::gr::COLOR_BLACK);
        window.draw(sprite);
        window.draw(circle);
        window.display();

        sprite.setRotation(sprite.getRotation() + 1.0);
    }

    //std::this_thread::sleep_for(std::chrono::seconds{3});
}
