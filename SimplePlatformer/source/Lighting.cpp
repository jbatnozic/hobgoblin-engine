
#include <Hobgoblin/Utility/Math.hpp>

#include <algorithm>
#include <cmath>
#include <cstdlib>
#include <iostream>

#include "Global_program_state.hpp"
#include "Lighting.hpp"

namespace {
using hg::util::Clamp;
using hg::util::Sqr;

template <class T>
T Dist_(T x1, T y1, T x2, T y2) {
     return std::sqrt(Sqr(x2 - x1) + Sqr(y2 - y1));
}

} // namespace

LightingManager::LightingManager(QAO_RuntimeRef runtimeRef)
    : GOF_NonstateObject(runtimeRef, TYPEID_SELF, 0, "LightingManager")
{
    // TODO Temp.
    _width = 20;
    _height = 20;
    _cellResolution = 32.f;

    _cellMatrix.resize(hg::pztos(_width * _height));

    for (auto& cell : _cellMatrix) {
        cell.isWall = ((std::rand() % 100) < 25);
    }

    resetLights();
}

void LightingManager::eventUpdate() {
    resetLights();

    auto pos = sf::Mouse::getPosition(global().windowMgr.getWindow());
    renderLight(Clamp(pos.x, 0, 639), Clamp(pos.y, 0, 639), 3000.f);
}

void LightingManager::eventDraw1() {
    sf::RectangleShape rect;
    rect.setSize({_cellResolution, _cellResolution});
    for (hg::PZInteger y = 0; y < _height; y += 1) {
        for (hg::PZInteger x = 0; x < _width; x += 1) {
            rect.setPosition({x * _cellResolution, y * _cellResolution});

            sf::Color col = cellAt(x, y).isWall ? sf::Color::Red : sf::Color::Cyan;
            col.r *= std::max(0.05f, cellAt(x, y).intensity);
            col.g *= std::max(0.05f, cellAt(x, y).intensity);
            col.b *= std::max(0.05f, cellAt(x, y).intensity);
            rect.setFillColor(col);

            global().windowMgr.getMainRenderTexture().draw(rect);
        }
    }
}

void LightingManager::resetLights() {
    for (auto& cell : _cellMatrix) {
        cell.intensity = 0.f;
    }
}

void LightingManager::renderLight(float lightX, float lightY, float intensity) {
    const int radius = 8; // TODO

    const int startX = static_cast<int>(lightX / _cellResolution);
    const int startY = static_cast<int>(lightY / _cellResolution);

    // Centre
    if (Clamp(startX, 0, _width - 1) == startX && Clamp(startY, 0, _height - 1) == startY) {
        cellAt(startX, startY).intensity = std::min(1.f, intensity);
    }

    for (int offset = 1; offset <= radius; offset += 1) {
        // Upper row
        {
            const int y = startY - offset;
            if (Clamp(y, 0, _height - 1) == y) {
                for (int x = startX - offset; x <= startX + offset; x += 1) {
                    if (x == startX) {
                        int __break = 5;
                    }

                    if (Clamp(x, 0, _width - 1) == x) {
                        cellAt(x, y).intensity = std::min(1.f, intensity * factor(x, y, lightX, lightY));
                    }
                }
            }
        }

        // Lower row
        {
            const int y = startY + offset;
            if (Clamp(y, 0, _height - 1) == y) {
                for (int x = startX - offset; x <= startX + offset; x += 1) {
                    if (Clamp(x, 0, _width - 1) == x) {
                        cellAt(x, y).intensity = std::min(1.f, intensity * factor(x, y, lightX, lightY));
                    }
                }
            }
        }

        // Left column
        {
            const int x = startX - offset;
            if (Clamp(x, 0, _width - 1) == x) {
                for (int y = startY - offset + 1; y < startY + offset; y += 1) {
                    if (Clamp(y, 0, _height - 1) == y) {
                        cellAt(x, y).intensity = std::min(1.f, intensity * factor(x, y, lightX, lightY));
                    }
                }
            }
        }

        // Right column
        {
            const int x = startX + offset;
            if (Clamp(x, 0, _width - 1) == x) {
                for (int y = startY - offset + 1; y < startY + offset; y += 1) {
                    if (Clamp(y, 0, _height - 1) == y) {
                        cellAt(x, y).intensity = std::min(1.f, intensity * factor(x, y, lightX, lightY));
                    }
                }
            }
        }
    }
}

LightingManager::Cell& LightingManager::cellAt(hg::PZInteger x, hg::PZInteger y) {
    return _cellMatrix.at(hg::pztos(y * _width + x));
}

const LightingManager::Cell& LightingManager::cellAt(hg::PZInteger x, hg::PZInteger y) const {
    return _cellMatrix.at(hg::pztos(y * _width + x));
}

float LightingManager::factor(hg::PZInteger cellX, hg::PZInteger cellY, float lightX, float lightY) const {
    const float xx = (cellX + 0.5f) * _cellResolution;
    const float yy = (cellY + 0.5f) * _cellResolution;

    const double angle = std::atan2(lightY - yy, lightX - xx);
    const float stepX = 0.5f * _cellResolution * static_cast<float>(std::cos(angle));
    const float stepY = 0.5f * _cellResolution * static_cast<float>(std::sin(angle));

    float currX = xx;
    float currY = yy;
    while (true) {
        currX += stepX;
        currY += stepY;

        const int currCellX = currX / _cellResolution;
        const int currCellY = currY / _cellResolution;

        if (currCellX == cellX && currCellY == cellY) {
            continue;
        }
        if (cellAt(currCellX, currCellY).isWall || 
            (abs(currCellX - cellX) > 1 && abs(currCellY - cellY) > 1 &&
             cellAt(currCellX, currCellY).intensity == 0.0f)) {
            return 0.0f;
        }
        if (Dist_(currX, currY, lightX, lightY) <= _cellResolution / 2) {
            break;
        }
    }

    return 1.f / Sqr(Dist_(xx, yy, lightX, lightY));
}