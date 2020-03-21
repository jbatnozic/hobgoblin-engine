#ifndef WINDOW_MANAGER_HPP
#define WINDOW_MANAGER_HPP

#include <iostream>

#include <SFML/Window.hpp>
#include <SFML/Graphics.hpp>

#include "Game_object_framework.hpp"

class WindowManager : public GOF_Base {
public:
    sf::RenderTexture appSurface;
    sf::RenderWindow window;
    
    WindowManager(QAO_Runtime* runtime)
        : GOF_Base{runtime, TYPEID_SELF, -1000, "WindowManager"}
    {
        window.create(sf::VideoMode(800, 800), "Window");
        // vSync and Framerate limiter perform near identically as far as
        // time correctness is concerned, but the game is usually smoother
        // with vSync. TODO Make it a toggle in the settings.
        window.setVerticalSyncEnabled(true);
        // window.setFramerateLimit(60);

        appSurface.create(800, 800);
    }

    void eventPostUpdate() {
        appSurface.clear();
    }

    void eventRender() override {
        appSurface.display();
        sf::Sprite sprite{appSurface.getTexture()};
        window.draw(sprite);
        window.display();

        sf::Event event;
        while (window.pollEvent(event)) {
            // Swallow events (TODO: Add event listeners)
            /*if (event.type == sf::Event::Closed) {
                window.close();
            }*/
        }
    }
};

#endif // !WINDOW_MANAGER_HPP

