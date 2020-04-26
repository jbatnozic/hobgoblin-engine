
#include "Window_manager.hpp"

#include <algorithm>

WindowManager::WindowManager(QAO_RuntimeRef runtimeRef)
    : GOF_Base{runtimeRef, TYPEID_SELF, -1000, "WindowManager"}
    , _windowAdapter{_window}
    , _mainRenderTextureAdapter{_mainRenderTexture}
{
}

WindowManager::WindowManager(QAO_RuntimeRef runtimeRef, sf::Vector2u windowSize, const sf::String& windowTitle,
                             sf::Vector2u mainRenderTargetSize)
    : WindowManager{runtimeRef}
{
    _window.create(sf::VideoMode{windowSize.x, windowSize.y}, windowTitle);
    _mainRenderTexture.create(mainRenderTargetSize.x, mainRenderTargetSize.y);
}

void WindowManager::create() {
    _window.create(sf::VideoMode(800, 800), "Window");
    // vSync and Framerate limiter perform near identically as far as
    // time correctness is concerned, but the game is usually smoother
    // with vSync. TODO Make it a toggle in the settings.
    //_window.setVerticalSyncEnabled(true);
    _window.setFramerateLimit(60);

    _mainRenderTexture.create(800, 800);
    _mainRenderTextureAdapter.setViewCount(1);
    _mainRenderTextureAdapter.getView(0).setSize({800, 800});
    _mainRenderTextureAdapter.getView(0).setCenter({400, 400});
    _mainRenderTextureAdapter.getView(0).setViewport({0, 0, 1, 1});
}

sf::RenderWindow& WindowManager::getWindow() {
    return _window;
}

sf::RenderTexture& WindowManager::getMainRenderTexture() {
    return _mainRenderTexture;
}

hg::gr::Brush WindowManager::getBrush() {
    return hg::gr::Brush{&getCanvas()};
}

hg::gr::Canvas& WindowManager::getCanvas() {
    if (getRuntime()->getCurrentEvent() == QAO_Event::DrawGUI) {
        return _windowAdapter;
    }
    else {
        return _mainRenderTextureAdapter;
    }
}

KbInputTracker& WindowManager::getKeyboardInput() {
    return _kbi;
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
    _mainRenderTexture.clear(hg::gr::Color::Khaki);
}

void WindowManager::eventRender() {
    _window.clear(sf::Color::Black);
    drawMainRenderTexture(DrawPosition::Centre);
    _window.display();

    _kbi.prepForEvents();

    sf::Event ev;
    while (_window.pollEvent(ev)) {
        switch (ev.type) {
        case sf::Event::Closed:
            // TODO
            break;

        case sf::Event::KeyPressed:
        case sf::Event::KeyReleased:
            _kbi.onKeyEvent(ev);
            break;

        case sf::Event::Resized:
            {
                sf::FloatRect visibleArea(0, 0, ev.size.width, ev.size.height);
                _window.setView(sf::View(visibleArea));
            }
            break;
        
        case sf::Event::TextEntered:
            _kbi.onTextEvent(ev);
            break;

        default: (void)0;
        }
    }
}

// Views:

void WindowManager::setViewCount(hg::PZInteger viewCount) {
    _mainRenderTextureAdapter.setViewCount(viewCount);
}

hg::PZInteger WindowManager::getViewCount() const noexcept {
    return _mainRenderTextureAdapter.getViewCount();
}

sf::View& WindowManager::getView(hg::PZInteger viewIndex) {
    return _mainRenderTextureAdapter.getView(viewIndex);
}

const sf::View& WindowManager::getView(hg::PZInteger viewIndex) const {
    return _mainRenderTextureAdapter.getView(viewIndex);
}

