
#define HOBGOBLIN_SHORT_NAMESPACE
#include <Hobgoblin/Window.hpp>

#include <chrono>
#include <thread>
#include <iostream>

int main(int argc, char* argv[]) {
    hg::win::Window window{};
    window.create(hg::win::VideoMode{400, 400}, "Window");

    while (window.isOpen()) {
        hg::win::Event ev;
        while (window.pollEvent(ev)) {}

        window.display();

        std::this_thread::sleep_for(std::chrono::milliseconds{10});
    }
}
