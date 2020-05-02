
#include <Hobgoblin/Utility/Math.hpp>

#include <algorithm>
#include <cmath>
#include <cstdlib>
#include <iostream>

#include "GameContext/Game_context.hpp"
#include "Experimental/Lighting.hpp"

namespace {
using hg::util::Clamp;
using hg::util::Sqr;

template <class T>
T Dist_(T x1, T y1, T x2, T y2) {
     return std::sqrt(Sqr(x2 - x1) + Sqr(y2 - y1));
}

std::uint8_t LightFunc(double normalizedDistance) {
    return static_cast<std::uint8_t>(255.0 * std::sqrt(1.0 - normalizedDistance));
}

} // namespace

LightingController::Light::Light(float x, float y, float radius, hg::gr::Color color)
    : _x{x}
    , _y{y}
    , _radius{radius}
    , _color{color}
{
    hg::PZInteger gridDim = static_cast<hg::PZInteger>(std::ceil(radius)) * 2 - 1;
    _intensities.resize(gridDim, gridDim);
}

void LightingController::Light::render(const hg::util::RowMajorGrid<Cell>& world, float cellResolution) {
    const auto lightXInWorldGridCoords = static_cast<hg::PZInteger>(_x / cellResolution);
    const auto lightYInWorldGridCoords = static_cast<hg::PZInteger>(_y / cellResolution);

    const auto intensitiesTopLeftXInWorldGridCoords =
        static_cast<int>(lightXInWorldGridCoords - _intensities.getWidth() / 2);

    const auto intensitiesTopLeftYInWorldGridCoords =
        static_cast<int>(lightYInWorldGridCoords - _intensities.getWidth() / 2);

    for (hg::PZInteger yOffset = 0; yOffset < _intensities.getHeight(); yOffset += 1) {
        if (intensitiesTopLeftYInWorldGridCoords + yOffset < 0 ||
            intensitiesTopLeftYInWorldGridCoords + yOffset >= world.getHeight()) {
            // Black out row
            for (hg::PZInteger xOffset = 0; xOffset < _intensities.getWidth(); xOffset += 1) {
                _intensities[yOffset][xOffset] = 0;
            }
            continue;
        }

        for (hg::PZInteger xOffset = 0; xOffset < _intensities.getWidth(); xOffset += 1) {
            if (intensitiesTopLeftXInWorldGridCoords + xOffset < 0 ||
                intensitiesTopLeftXInWorldGridCoords + xOffset >= world.getWidth()) {
                // Black out cell
                _intensities[yOffset][xOffset] = 0;
                continue;
            }

            _intensities[yOffset][xOffset] = _trace(world, cellResolution, _x, _y,
                                                    intensitiesTopLeftXInWorldGridCoords + xOffset,
                                                    intensitiesTopLeftYInWorldGridCoords + yOffset);
        }
    }
}

std::uint8_t LightingController::Light::_trace(const hg::util::RowMajorGrid<Cell>& world, float cellResolution,
                                               float lightX, float lightY,
                                               hg::PZInteger cellX, hg::PZInteger cellY) const {
    const float cellXInWorldCoords = (cellX + 0.5f) * cellResolution;
    const float cellYInWorldCoords = (cellY + 0.5f) * cellResolution;

    const float totalDist = Dist_(cellXInWorldCoords, cellYInWorldCoords, lightX, lightY);
    const auto iterCnt = static_cast<hg::PZInteger>(std::floor(totalDist / cellResolution));

    const double angle = std::atan2(lightY - cellYInWorldCoords, lightX - cellXInWorldCoords);
    const double stepX = std::cos(angle) * cellResolution;
    const double stepY = std::sin(angle) * cellResolution;

    double currX = cellXInWorldCoords;
    double currY = cellYInWorldCoords;
    double lastCellX = currX / cellResolution;
    double lastCellY = currY / cellResolution;

    for (hg::PZInteger i = 0; i <= iterCnt; i += 1) {
        if (i == iterCnt) {
            currX = lightX;
            currY = lightY;
        }
        else {
            currX += stepX;
            currY += stepY;
        }

        const auto currCellX = static_cast<hg::PZInteger>(currX / cellResolution);
        const auto currCellY = static_cast<hg::PZInteger>(currY / cellResolution);

        if (currCellX == lastCellX && currCellY == lastCellY) {
            continue;
        }

        auto& currCell = world[currCellY][currCellX];

        // Walls are opaque
        if (currCell.isWall) {
            return 0;
        }

        // Light can't go through corners
        if ((lastCellX - currCellX) != 0 && (lastCellY - currCellY) != 0) {
            if (currCell.isWall && world[lastCellY][currCellX].isWall) {
                return 0;
            }
        }

        lastCellX = currCellX;
        lastCellY = currCellY;
    }

    const double normalizedDistance = totalDist / (_radius * cellResolution);
    return LightFunc(std::min(normalizedDistance, 1.0));
}

void LightingController::Light::integrate(hg::util::RowMajorGrid<Cell>& world, float cellResolution) const {
    const auto lightXInWorldGridCoords = static_cast<hg::PZInteger>(_x / cellResolution);
    const auto lightYInWorldGridCoords = static_cast<hg::PZInteger>(_y / cellResolution);

    const auto intensitiesTopLeftXInWorldGridCoords =
        static_cast<int>(lightXInWorldGridCoords - _intensities.getWidth() / 2);

    const auto intensitiesTopLeftYInWorldGridCoords =
        static_cast<int>(lightYInWorldGridCoords - _intensities.getWidth() / 2);

    for (hg::PZInteger yOffset = 0; yOffset < _intensities.getHeight(); yOffset += 1) {
        if (intensitiesTopLeftYInWorldGridCoords + yOffset < 0 ||
            intensitiesTopLeftYInWorldGridCoords + yOffset >= world.getHeight()) {
            continue;
        }

        for (hg::PZInteger xOffset = 0; xOffset < _intensities.getWidth(); xOffset += 1) {
            if (intensitiesTopLeftXInWorldGridCoords + xOffset < 0 ||
                intensitiesTopLeftXInWorldGridCoords + xOffset >= world.getWidth()) {
                continue;
            }

            auto& worldCell = world[intensitiesTopLeftYInWorldGridCoords + yOffset]
                                   [intensitiesTopLeftXInWorldGridCoords + xOffset];

            auto intensity = _intensities[yOffset][xOffset];
            hg::gr::Color intensityDummyColor{intensity, intensity, intensity};
            worldCell.color = hg::gr::AddColors(worldCell.color,
                                                hg::gr::MultiplyColors(_color, intensityDummyColor));
        }
    }
}

void LightingController::Light::setPosition(float x, float y) {
    _x = x;
    _y = y;
}

///////////////////////////////////////////////////////////////////////////////////////////////////

LightingController::LightingController(hg::PZInteger width, hg::PZInteger height,
                                       float cellResolution, hg::gr::Color ambientColor)
    : _world{width, height}
    , _cellResolution{cellResolution}
    , _ambientColor{ambientColor}
{
}

LightingController::LightingController()
    : LightingController(0, 0, 0.f)
{
}

LightingController::LightHandle LightingController::addLight(float x, float y, hg::gr::Color color, float radius) {
    auto lightIndex = _lightIndexer.acquire();

    if (_lights.size() <= lightIndex) {
        _lights.resize(lightIndex + 1);
    }

    _lights[lightIndex] = Light{x, y, radius, color};

    return static_cast<LightHandle>(lightIndex);
}

void LightingController::moveLight(LightHandle handle, float x, float y) {
    auto& light = _lights[hg::ToSz(handle)];
    light.setPosition(x, y);
    
}

hg::gr::Color LightingController::getColorAt(hg::PZInteger x, hg::PZInteger y) const {
    return _world.at(y, x).color;
}

void LightingController::render() {
    // Clear world with ambient light
    for (hg::PZInteger y = 0; y < _world.getHeight(); y += 1) {
        for (hg::PZInteger x = 0; x < _world.getWidth(); x += 1) {
            _world[y][x].color = _ambientColor;
        }
    }

    // Render all lights and integrate them
    for (std::size_t i = 0; i < _lights.size(); i += 1) {
        if (_lightIndexer.isSlotEmpty(hg::ToPz(i))) {
            continue;
        }
        _lights[i].render(_world, _cellResolution);
        _lights[i].integrate(_world, _cellResolution);
    }
}

void LightingController::resize(hg::PZInteger width, hg::PZInteger height, float cellResolution) {
    _world.resize(width, height);
    _cellResolution = _cellResolution;
}

void LightingController::setCellIsWall(hg::PZInteger x, hg::PZInteger y, bool isWall) {
    _world.at(y, x).isWall = isWall;
}





















//void LightingController::renderLight(float lightX, float lightY, float intensity) {
//    const int radius = std::ceil(intensity);
//
//    const int startX = static_cast<int>(lightX / _cellResolution);
//    const int startY = static_cast<int>(lightY / _cellResolution);
//
//    // Centre
//    if (Clamp(startX, 0, _width - 1) == startX && Clamp(startY, 0, _height - 1) == startY) {
//        cellAt(startX, startY).intensity = std::min(1.f, intensity);
//    }
//
//    for (int offset = 1; offset <= radius; offset += 1) {
//        // Upper row
//        {
//            const int yOffset = startY - offset;
//            if (Clamp(yOffset, 0, _height - 1) == yOffset) {
//                for (int xOffset = startX - offset; xOffset <= startX + offset; xOffset += 1) {
//                    if (xOffset == startX) {
//                        int __break = 5;
//                    }
//
//                    if (Clamp(xOffset, 0, _width - 1) == xOffset) {
//                        cellAt(xOffset, yOffset).intensity = std::min(1.f, factor(xOffset, yOffset, lightX, lightY, intensity));
//                    }
//                }
//            }
//        }
//
//        // Lower row
//        {
//            const int yOffset = startY + offset;
//            if (Clamp(yOffset, 0, _height - 1) == yOffset) {
//                for (int xOffset = startX - offset; xOffset <= startX + offset; xOffset += 1) {
//                    if (Clamp(xOffset, 0, _width - 1) == xOffset) {
//                        cellAt(xOffset, yOffset).intensity = std::min(1.f, factor(xOffset, yOffset, lightX, lightY, intensity));
//                    }
//                }
//            }
//        }
//
//        // Left column
//        {
//            const int xOffset = startX - offset;
//            if (Clamp(xOffset, 0, _width - 1) == xOffset) {
//                for (int yOffset = startY - offset + 1; yOffset < startY + offset; yOffset += 1) {
//                    if (Clamp(yOffset, 0, _height - 1) == yOffset) {
//                        cellAt(xOffset, yOffset).intensity = std::min(1.f, factor(xOffset, yOffset, lightX, lightY, intensity));
//                    }
//                }
//            }
//        }
//
//        // Right column
//        {
//            const int xOffset = startX + offset;
//            if (Clamp(xOffset, 0, _width - 1) == xOffset) {
//                for (int yOffset = startY - offset + 1; yOffset < startY + offset; yOffset += 1) {
//                    if (Clamp(yOffset, 0, _height - 1) == yOffset) {
//                        cellAt(xOffset, yOffset).intensity = std::min(1.f, factor(xOffset, yOffset, lightX, lightY, intensity));
//                    }
//                }
//            }
//        }
//    }
//}
//
//LightingController::Cell& LightingController::cellAt(hg::PZInteger xOffset, hg::PZInteger yOffset) {
//    return _cellMatrix.at(hg::pztos(yOffset * _width + xOffset));
//}
//
//const LightingController::Cell& LightingController::cellAt(hg::PZInteger xOffset, hg::PZInteger yOffset) const {
//    return _cellMatrix.at(hg::pztos(yOffset * _width + xOffset));
//}

//float LightingController::factor(hg::PZInteger cellX, hg::PZInteger cellY, 
//                              float lightX, float lightY, float intensity) const {
//    const float xx = (cellX + 0.5f) * _cellResolution; // cell x in world coordinates
//    const float yy = (cellY + 0.5f) * _cellResolution; // cell y in world coordinates
//
//    const float totalDist = Dist_(xx, yy, lightX, lightY);
//    const int iterCnt = std::floor(totalDist / _cellResolution);
//
//    const double angle = std::atan2(lightY - yy, lightX - xx);
//    const float stepX = 1.f * _cellResolution * static_cast<float>(std::cos(angle));
//    const float stepY = 1.f * _cellResolution * static_cast<float>(std::sin(angle));
//
//    float currX = xx;
//    float currY = yy;
//    float lastCellX = currX / _cellResolution;
//    float lastCellY = currY / _cellResolution;
//        
//    for (int i = 0; i <= iterCnt; i += 1) {
//        if (i == iterCnt) {
//            currX = lightX;
//            currY = lightY;
//        }
//        else {
//            currX += stepX;
//            currY += stepY;
//        }
//
//        const int currCellX = currX / _cellResolution;
//        const int currCellY = currY / _cellResolution;
//
//        if (currCellX == lastCellX && currCellY == lastCellY) {
//            continue;
//        }
//
//        auto& currCell = cellAt(currCellX, currCellY);
//
//        if (currCell.isWall) { // Walls are opaque
//            return 0.0f;
//        }
//
//        if ((lastCellX - currCellX) != 0 && (lastCellY - currCellY) != 0) {
//            if (cellAt(lastCellX, currCellY).isWall && cellAt(currCellX, lastCellY).isWall) {
//                return 0.0f;
//            }
//        }
//
//        lastCellX = currCellX;
//        lastCellY = currCellY;
//    }
//    
//    //auto lightFunc = [](float d) { return std::sqrt(std::cos(d * 3.1415f / 2.f)); };
//    //auto lightFunc = [](float d) { return std::cos(d * 3.1415f / 2.f); };
//    auto lightFunc = [](float d) { return std::sqrt(1.f - d); };
//    
//    //return 1.f / Sqr(Dist_(xx, yy, lightX, lightY));
//    const float normalizedDistance = Dist_(xx, yy, lightX, lightY) / (intensity * _cellResolution);
//    return lightFunc(Clamp(normalizedDistance, 0.f, 1.f));
//}