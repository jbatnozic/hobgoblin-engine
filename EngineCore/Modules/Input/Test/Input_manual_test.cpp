
#include <Hobgoblin/Input.hpp>

#include <SFML/Window.hpp>

#include <chrono>
#include <iostream>
#include <thread>

namespace in = jbatnozic::hobgoblin::in;

int main(int argc, char* argv[]) {
    while (true) {
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Scan::VolumeDown)) {
            std::cout << "SFML VolumeDown\n";
        }

        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Scan::VolumeUp)) {
            std::cout << "SFML VolumeUp\n";
        }

        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Scan::VolumeMute)) {
            std::cout << "SFML VolumeMute\n";
        }

        {
            if (in::CheckPressedPK(*in::StringToInput("PHYSICALKEY:SPACE"))) {
                std::cout << "Space pressed\n";
            }
            if (in::CheckPressedPK(*in::StringToInput("PhysicalKey:Enter"))) {
                std::cout << "Enter pressed\n";
            }
        }

        const auto vk = in::DetectPressedVK();
        if (vk) {
            const auto string = in::InputToString(*vk);
            std::cout << "Detected pressed virtual key: " << (string ? *string : "<unknown>") << '\n';
        }

        const auto pk = in::DetectPressedPK();
        if (pk) {
            const auto string = in::InputToString(*pk);
            std::cout << "Detected pressed physical key: " << (string ? *string : "<unknown>") << '\n';
        }

        std::this_thread::sleep_for(std::chrono::milliseconds{1});
    }
}
