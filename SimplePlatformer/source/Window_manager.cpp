
#include "Window_manager.hpp"

#include <algorithm>

WindowManager::WindowManager(QAO_RuntimeRef runtimeRef)
    : GOF_Base{runtimeRef, TYPEID_SELF, -1000, "WindowManager"}
    , _windowAdapter{_window}
    , _mainRenderTextureAdapter{_mainRenderTexture}
{
    _window.create(sf::VideoMode(800, 800), "Window");
    // vSync and Framerate limiter perform near identically as far as
    // time correctness is concerned, but the game is usually smoother
    // with vSync. TODO Make it a toggle in the settings.
    //_window.setVerticalSyncEnabled(true);
    _window.setFramerateLimit(60);

    _mainRenderTexture.create(800, 800);
}

WindowManager::WindowManager(QAO_RuntimeRef runtimeRef, sf::Vector2u windowSize, const sf::String& windowTitle,
                             sf::Vector2u mainRenderTargetSize)
    : WindowManager{runtimeRef}
{
    _window.create(sf::VideoMode{windowSize.x, windowSize.y}, windowTitle);
    _mainRenderTexture.create(mainRenderTargetSize.x, mainRenderTargetSize.y);
}

sf::RenderWindow& WindowManager::getWindow() {
    return _window;
}

sf::RenderTexture& WindowManager::getMainRenderTexture() {
    return _mainRenderTexture;
}

hg::gr::Brush WindowManager::getBrush() {
    if (getRuntime()->getCurrentEvent() == QAO_Event::DrawGUI) {
        return hg::gr::Brush{&_windowAdapter};
    }
    else {
        return hg::gr::Brush{&_mainRenderTextureAdapter};
    }
}

void WindowManager::drawMainRenderTexture(DrawPosition drawPosition) {
    _mainRenderTexture.display();
    sf::Sprite mrtSprite{_mainRenderTexture.getTexture()};
    const sf::Vector2u mrtSize = _mainRenderTexture.getSize();
    const sf::Vector2u winSize = _window.getSize();

    switch (drawPosition) {
    case DrawPosition::Stretch:
        mrtSprite.setScale({static_cast<float>(winSize.x) / mrtSize.x,
                            static_cast<float>(winSize.y) / mrtSize.y});
        break;

    case DrawPosition::Fill:
    case DrawPosition::Fit:
        {
            float scale;
            if (drawPosition == DrawPosition::Fill) {
                scale = std::max(static_cast<float>(winSize.x) / mrtSize.x,
                                 static_cast<float>(winSize.y) / mrtSize.y);
            }
            else {
                scale = std::min(static_cast<float>(winSize.x) / mrtSize.x,
                                 static_cast<float>(winSize.y) / mrtSize.y);
            }
            mrtSprite.setScale({scale, scale});
        }
        // TODO SWITCH_FALLTHROUGH;

    case DrawPosition::Centre:
        mrtSprite.setOrigin({static_cast<float>(mrtSize.x) / 2.f,
                             static_cast<float>(mrtSize.y) / 2.f});
        mrtSprite.setPosition({static_cast<float>(winSize.x) / 2.f,
                               static_cast<float>(winSize.y) / 2.f});
        break;

    default:
        assert(0 && "Unreachable");
    }

    _window.draw(mrtSprite);
}

void WindowManager::eventPostUpdate() {
    _mainRenderTexture.clear(sf::Color::White);
}

void WindowManager::eventRender() {
    drawMainRenderTexture(DrawPosition::Fill);
    _window.display();

    sf::Event event;
    while (_window.pollEvent(event)) {
        // Swallow events (TODO: Add event listeners)
        if (event.type == sf::Event::Resized)
        {
            // update the view to the new size of the window
            sf::FloatRect visibleArea(0, 0, event.size.width, event.size.height);
            _window.setView(sf::View(visibleArea));
        }
        /*if (event.type == sf::Event::Closed) {
            window.close();
        }*/
    }
}

