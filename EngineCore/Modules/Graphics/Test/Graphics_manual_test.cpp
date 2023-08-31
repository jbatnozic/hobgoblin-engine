
#define HOBGOBLIN_SHORT_NAMESPACE
#include <Hobgoblin/Graphics.hpp>

#include <chrono>
#include <thread>
#include <iostream>

int main(int argc, char* argv[]) {
    hg::gr::RenderWindow window{};
    window.create(hg::gr::VideoMode{400, 400}, "Window");
    window.setFramerateLimit(60);

    hg::gr::Sprite sprite{};

    while (window.isOpen()) {
        hg::gr::Event ev;
        while (window.pollEvent(ev)) {}

        window.draw(sprite);
        window.display();
    }

    //std::this_thread::sleep_for(std::chrono::seconds{3});
}
