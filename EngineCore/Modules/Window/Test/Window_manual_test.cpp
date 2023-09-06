
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
        while (window.pollEvent(ev)) {
            ev.visit(
                [](hg::win::Event::Closed&) {
                    std::cout << "Event: closed\n";
                },
                [](hg::win::Event::GainedFocus&) {
                    std::cout << "Event: gained focus\n";
                },
                [](hg::win::Event::LostFocus&) {
                    std::cout << "Event: lost focus\n";
                },
                [](hg::win::Event::KeyPressed& aEventData) {
                    std::cout << "Event: key pressed (" 
                              << *hg::in::InputToString(aEventData.virtualKey) << " | " 
                              << *hg::in::InputToString(aEventData.physicalKey) << ")\n";
                },
                [](hg::win::Event::KeyReleased& aEventData) {
                    std::cout << "Event: key released (" 
                              << *hg::in::InputToString(aEventData.virtualKey) << " | " 
                              << *hg::in::InputToString(aEventData.physicalKey) << ")\n";
                },
                [](hg::win::Event::MouseButtonPressed& aEventData) {
                    std::cout << "Event: mouse button pressed ("
                              << *hg::in::InputToString(aEventData.button) << ")\n";
                },
                [](hg::win::Event::MouseButtonReleased& aEventData) {
                    std::cout << "Event: mouse button released ("
                        << *hg::in::InputToString(aEventData.button) << ")\n";
                },
                [](hg::win::Event::MouseEntered&) {
                    std::cout << "Event: mouse entered\n";
                },
                [](hg::win::Event::MouseLeft&) {
                    std::cout << "Event: mouse left\n";
                },
                [](hg::win::Event::MouseMoved& aEventData) {
                    std::cout << "Event: mouse moved to (" << aEventData.x << ", " << aEventData.y << ")\n";
                },
                [](hg::win::Event::Resized& aEventData) {
                    std::cout << "Event: window resized to (" << aEventData.width << ", " << aEventData.height << ")\n";
                },
                [](hg::win::Event::TextEntered& aEventData) {
                    std::cout << "Event: text entered (UTF-32 code point = " << aEventData.unicode << ")\n";
                }
            );
        }

        window.display();

        std::this_thread::sleep_for(std::chrono::milliseconds{10});
    }
}
