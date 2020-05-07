#ifndef WINDOW_MANAGER_HPP
#define WINDOW_MANAGER_HPP

#include <iostream>

#include <Hobgoblin/Graphics.hpp>
#include <SFML/Window.hpp>
#include <SFML/Graphics.hpp>

#include "GameObjects/Framework/Game_object_framework.hpp"
#include "Utility/Keyboard_input.hpp"

enum class DrawPosition {
    Fill, 
    Fit, 
    Stretch, 
    Centre
};

class WindowManager : public GOF_Base {
public:
    WindowManager(QAO_RuntimeRef runtimeRef);
    WindowManager(QAO_RuntimeRef runtimeRef, sf::Vector2u windowSize, const sf::String& windowTitle,
                  sf::Vector2u mainRenderTargetSize);

    void create(); // TODO Temp.

    sf::RenderWindow& getWindow();
    sf::RenderTexture& getMainRenderTexture();
    hg::gr::Brush getBrush();
    hg::gr::Canvas& getCanvas();

    void eventPostUpdate();
    void eventRender();

    void drawMainRenderTexture(DrawPosition drawPosition);

    KbInputTracker& getKeyboardInput();

    // Views:
    void setViewCount(hg::PZInteger viewCount);
    hg::PZInteger getViewCount() const noexcept;
    sf::View& getView(hg::PZInteger viewIndex = 0);
    const sf::View& getView(hg::PZInteger viewIndex = 0) const;

    sf::Vector2f getMousePos(hg::PZInteger viewIndex = 0) const {
        auto pixelPos = sf::Mouse::getPosition(_window);
        return _window.mapPixelToCoords(pixelPos, getView(viewIndex));
    }

private:
    sf::RenderWindow _window;
    sf::RenderTexture _mainRenderTexture;
    hg::gr::CanvasAdapter _windowAdapter;
    hg::gr::MultiViewRenderTargetAdapter _mainRenderTextureAdapter;
    KbInputTracker _kbi;
};

#endif // !WINDOW_MANAGER_HPP

