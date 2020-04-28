#pragma once

#include <Hobgoblin/Graphics.hpp>

#include <vector>

#include "GameObjects/Framework/Game_object_framework.hpp"

class LightingManager : public GOF_NonstateObject {
public:
    LightingManager(QAO_RuntimeRef runtimeRef);

    void eventUpdate() override;
    void eventDraw1() override;

private:
    struct Cell {
        float intensity;
        bool isWall;
    };

    std::vector<Cell> _cellMatrix;
    hg::PZInteger _width, _height;
    float _cellResolution;

    void resetLights();
    void renderLight(float lightX, float lightY, float intensity);
    Cell& cellAt(hg::PZInteger x, hg::PZInteger y);
    const Cell& cellAt(hg::PZInteger x, hg::PZInteger y) const;
    float factor(hg::PZInteger cellX, hg::PZInteger cellY, float lightX, float lightY, float intensity) const;
};
