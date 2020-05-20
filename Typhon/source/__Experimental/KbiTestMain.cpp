
#include <Hobgoblin/Graphics/Builtin_fonts.hpp>
#include <SFML/Graphics.hpp>
#include <SFML/Window.hpp>

#include "Keyboard_input.hpp"

int main()
{
    sf::RenderWindow window(sf::VideoMode(320, 320), "SFML works!");
    window.setFramerateLimit(60);

    KbInputTracker kbi;
    kbi.setInputStringMaxLength(20);

    sf::RectangleShape rect;
    rect.setSize({30.f, 30.f});

    while (window.isOpen())
    {
        kbi.prepForEvents();

        sf::Event event;
        while (window.pollEvent(event))
        {
            if (event.type == sf::Event::Closed) {
                window.close();
            }
            else if (event.type == sf::Event::KeyPressed) {
                kbi.onKeyEvent(event);
            }
            else if (event.type == sf::Event::KeyReleased) {
                kbi.onKeyEvent(event);
            }
            else if (event.type == sf::Event::TextEntered) {
                kbi.onTextEvent(event);
            }
        }

        if (kbi.keyPressed(KbKey::C, KbMode::Edge)) {
            kbi.clearKey(KbKey::A);
        }

        window.clear();
        
        rect.setFillColor(sf::Color::Green);

        if (kbi.keyPressed(KbKey::A)) {
            rect.setPosition({10.f, 10.f});
            window.draw(rect);
        }

        if (kbi.keyPressed(KbKey::A, KbMode::Edge)) {
            rect.setPosition({10.f, 42.f});
            window.draw(rect);
        }

        if (kbi.keyPressed(KbKey::A, KbMode::Direct)) {
            rect.setPosition({10.f, 74.f});
            window.draw(rect);
        }

        if (kbi.keyPressed(KbKey::A, KbMode::Repeat)) {
            rect.setPosition({10.f, 106.f});
            window.draw(rect);
        }

        rect.setFillColor(sf::Color::Red);

        if (kbi.keyReleased(KbKey::A)) {
            rect.setPosition({42.f, 10.f});
            window.draw(rect);
        }

        if (kbi.keyReleased(KbKey::A, KbMode::Edge)) {
            rect.setPosition({42.f, 42.f});
            window.draw(rect);
        }

        if (kbi.keyReleased(KbKey::A, KbMode::Direct)) {
            rect.setPosition({42.f, 74.f});
            window.draw(rect);
        }

        if (kbi.keyReleased(KbKey::A, KbMode::Repeat)) {
            rect.setPosition({42.f, 106.f});
            window.draw(rect);
        }

        sf::Text text;
        text.setString(kbi.inputString);
        text.setFont(hg::gr::BuiltInFonts::getFont(hg::gr::BuiltInFonts::TitilliumRegular));
        text.setFillColor(sf::Color::White);
        text.setCharacterSize(16);
        text.setPosition(10, 150);

        window.draw(text);
        window.display();
    }

    return 0;
}