#pragma once

#include "Engine.hpp"

#include <Hobgoblin/Math/Vector.hpp>
#include <Hobgoblin/Graphics.hpp>

class CharacterRenderer {
public:
    CharacterRenderer(spe::GameContext& aCtx);

    void init(hg::gr::Color aColor);

    void setSize(hg::PZInteger aSize);
    void setPosition();

private:
    spe::GameContext& _ctx;

    hg::math::Vector2f _position; 
    hg::gr::Color      _color;

    hg::gr::Multisprite _body;
    hg::gr::Multisprite _legs;
    hg::gr::Multisprite _biceps;
    hg::gr::Multisprite _forearm;
    hg::gr::Multisprite _openHand;
    hg::gr::Multisprite _closedHand;
};