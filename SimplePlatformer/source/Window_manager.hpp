#ifndef WINDOW_MANAGER_HPP
#define WINDOW_MANAGER_HPP

#include <SFML/Window.hpp>
#include <SFML/Graphics.hpp>

#include "Object_framework.hpp"

class WindowManager : public GameObject {
public:
    sf::RenderTexture appSurface;
    sf::RenderWindow window;

    WindowManager()
        : GameObject{0, -1000, "WindowManager"}
    {
        window.create(sf::VideoMode(800, 800), "Window");
        appSurface.create(800, 800);

        window.setVerticalSyncEnabled(true);
        //window.setFramerateLimit(60);
    }

    void eventRender() override {
        appSurface.display();
        sf::Sprite sprite{appSurface.getTexture()};
        window.draw(sprite);
        window.display();

        sf::Event event;
        while (window.pollEvent(event)) {
            // Swallow events (TODO)
            /*if (event.type == sf::Event::Closed) {
                window.close();
            }*/
        }
    }
};

#endif // !WINDOW_MANAGER_HPP

