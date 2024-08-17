#pragma once

#include "Engine.hpp"

#include <Hobgoblin/Math/Vector.hpp>
#include <Hobgoblin/Graphics.hpp>

class CharacterRenderer {
public:
    enum class Mode {
        CRAWL,
        FLING,
        HUNKER
    };

    CharacterRenderer(spe::GameContext& aCtx, hg::gr::Color aColor);

    void setSize(hg::PZInteger aSize);
    void setPosition(hg::math::Vector2f aPosition);

    void update();
    void draw(hg::gr::Canvas& aCanvas);

private:
    spe::GameContext& _ctx;

    float _frameCounter = 0.f;

    hg::math::Vector2f _position; 
    hg::gr::Color      _color;

    hg::gr::Multisprite _body;
    hg::gr::Multisprite _legs;
    hg::gr::Multisprite _biceps;
    hg::gr::Multisprite _forearm;
    hg::gr::Multisprite _openHand;
    hg::gr::Multisprite _closedHand;
};