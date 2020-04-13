#ifndef WINDOW_MANAGER_HPP
#define WINDOW_MANAGER_HPP

#include <iostream>

#include <Hobgoblin/Graphics.hpp>
#include <SFML/Window.hpp>
#include <SFML/Graphics.hpp>

#include "Game_object_framework.hpp"

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

    sf::RenderWindow& getWindow();
    sf::RenderTexture& getMainRenderTexture();
    hg::gr::Brush getBrush();

    void eventPostUpdate();
    void eventRender();

    void drawMainRenderTexture(DrawPosition drawPosition);

private:
    sf::RenderWindow _window;
    sf::RenderTexture _mainRenderTexture;
    hg::gr::CanvasAdapter _windowAdapter;
    hg::gr::MultiViewRenderTargetAdapter _mainRenderTextureAdapter;
};

#endif // !WINDOW_MANAGER_HPP

