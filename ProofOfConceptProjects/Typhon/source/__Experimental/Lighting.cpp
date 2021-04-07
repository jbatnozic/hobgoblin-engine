
#include <Hobgoblin/Math.hpp>

#include <algorithm>
#include <cmath>
#include <cstdlib>
#include <iostream>

#include "Lighting.hpp"

namespace {
using hg::math::Clamp;
using hg::math::Sqr;

template <class T>
T Dist_(T x1, T y1, T x2, T y2) {
     return std::sqrt(Sqr(x2 - x1) + Sqr(y2 - y1));
}

std::uint8_t LightFunc(double normalizedDistance) {
    //const double multiplier = 1.0 - normalizedDistance;
    const double multiplier = (normalizedDistance < 0.5) ? (1.0 - normalizedDistance * 0.5)
                                                         : (1.5 - normalizedDistance * 1.5);
    return static_cast<std::uint8_t>(255.0 * multiplier);
}

} // namespace

LightingController::Light::Light(float x, float y, float radius, Color color)
    : _x{x}
    , _y{y}
    , _radius{radius}
    , _color{color}
{
    PZInteger gridDim = static_cast<PZInteger>(std::ceil(radius)) * 2 + 1;
    _intensities.resize(gridDim, gridDim);
}

void LightingController::Light::render(const hg::util::RowMajorGrid<Cell>& world, float cellResolution) {
    const auto lightXInWorldGridCoords = static_cast<PZInteger>(_x / cellResolution);
    const auto lightYInWorldGridCoords = static_cast<PZInteger>(_y / cellResolution);

    const auto intensitiesTopLeftXInWorldGridCoords =
        static_cast<int>(lightXInWorldGridCoords - _intensities.getWidth() / 2);

    const auto intensitiesTopLeftYInWorldGridCoords =
        static_cast<int>(lightYInWorldGridCoords - _intensities.getWidth() / 2);

    for (PZInteger yOffset = 0; yOffset < _intensities.getHeight(); yOffset += 1) {
        if (intensitiesTopLeftYInWorldGridCoords + yOffset < 0 ||
            intensitiesTopLeftYInWorldGridCoords + yOffset >= world.getHeight()) {
            // Black out row
            for (PZInteger xOffset = 0; xOffset < _intensities.getWidth(); xOffset += 1) {
                _intensities[yOffset][xOffset] = 0;
            }
            continue;
        }

        for (PZInteger xOffset = 0; xOffset < _intensities.getWidth(); xOffset += 1) {
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
                                               PZInteger cellX, PZInteger cellY) const {
    const float PRECISION = 1.0;

    const float cellXInWorldCoords = (cellX + 0.5f) * cellResolution;
    const float cellYInWorldCoords = (cellY + 0.5f) * cellResolution;

    const float totalDist = Dist_(cellXInWorldCoords, cellYInWorldCoords, lightX, lightY);
    const auto iterCnt = static_cast<PZInteger>(std::floor(PRECISION * totalDist / cellResolution));

    const double angle = std::atan2(lightY - cellYInWorldCoords, lightX - cellXInWorldCoords);
    const double stepX = std::cos(angle) * cellResolution / PRECISION;
    const double stepY = std::sin(angle) * cellResolution / PRECISION;

    double currX = cellXInWorldCoords;
    double currY = cellYInWorldCoords;
    auto lastCellX = static_cast<PZInteger>(currX / cellResolution);
    auto lastCellY = static_cast<PZInteger>(currY / cellResolution);

    for (PZInteger i = 0; i <= iterCnt; i += 1) {
        if (i == iterCnt) {
            currX = lightX;
            currY = lightY;
        }
        else {
            currX += stepX;
            currY += stepY;
        }

        const auto currCellX = static_cast<PZInteger>(currX / cellResolution);
        const auto currCellY = static_cast<PZInteger>(currY / cellResolution);

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
            if (world[lastCellY][currCellX].isWall && world[currCellY][lastCellX].isWall) {
                return 0;
            }
        }

        lastCellX = currCellX;
        lastCellY = currCellY;
    }

    // Normalized = between 0 and 1
    const double normalizedDistance = totalDist / (_radius * cellResolution);
    return LightFunc(std::min(normalizedDistance, 1.0));
}

void LightingController::Light::integrate(hg::util::RowMajorGrid<Cell>& world, float cellResolution) const {
    const auto lightXInWorldGridCoords = static_cast<PZInteger>(_x / cellResolution);
    const auto lightYInWorldGridCoords = static_cast<PZInteger>(_y / cellResolution);

    const auto intensitiesTopLeftXInWorldGridCoords =
        static_cast<int>(lightXInWorldGridCoords - _intensities.getWidth() / 2);

    const auto intensitiesTopLeftYInWorldGridCoords =
        static_cast<int>(lightYInWorldGridCoords - _intensities.getWidth() / 2);

    for (PZInteger yOffset = 0; yOffset < _intensities.getHeight(); yOffset += 1) {
        if (intensitiesTopLeftYInWorldGridCoords + yOffset < 0 ||
            intensitiesTopLeftYInWorldGridCoords + yOffset >= world.getHeight()) {
            continue;
        }

        for (PZInteger xOffset = 0; xOffset < _intensities.getWidth(); xOffset += 1) {
            if (intensitiesTopLeftXInWorldGridCoords + xOffset < 0 ||
                intensitiesTopLeftXInWorldGridCoords + xOffset >= world.getWidth()) {
                continue;
            }

            auto& worldCell = world[intensitiesTopLeftYInWorldGridCoords + yOffset]
                                   [intensitiesTopLeftXInWorldGridCoords + xOffset];

            auto intensity = _intensities[yOffset][xOffset];
            Color intensityDummyColor{intensity, intensity, intensity};
            worldCell.color = hg::gr::AddColors(worldCell.color,
                                                hg::gr::MultiplyColors(_color, intensityDummyColor));
        }
    }
}

void LightingController::Light::setPosition(float x, float y) {
    _x = x;
    _y = y;
}

void LightingController::Light::setColor(Color color) {
    _color = color;
}

void LightingController::Light::setRadius(float radius) {
    _radius = radius;
}

///////////////////////////////////////////////////////////////////////////////////////////////////

LightingController::LightHandle::LightHandle(LightingController& lightingCtrl, PZInteger lightIndex)
    : _lightingCtrl{&lightingCtrl}
    , _lightIndex{lightIndex}
{
}

LightingController::LightHandle::LightHandle(LightHandle&& other) 
    : _lightingCtrl{other._lightingCtrl}
    , _lightIndex{other._lightIndex}
{
    other._lightingCtrl = nullptr;
}

LightingController::LightHandle& LightingController::LightHandle::operator=(LightHandle&& other) {
    this->_lightingCtrl = other._lightingCtrl;
    this->_lightIndex = other._lightIndex;

    other._lightingCtrl = nullptr;

    return *this;
}

LightingController::LightHandle::~LightHandle() {
    if (isValid()) {
        invalidate();
    }
}

void LightingController::LightHandle::setPosition(float x, float y) const {
    assert(isValid());
    _lightingCtrl->_setLightPosition(_lightIndex, x, y);
}

void LightingController::LightHandle::setColor(Color color) const {
    assert(isValid());
    _lightingCtrl->_setLightColor(_lightIndex, color);
}

void LightingController::LightHandle::setRadius(float radius) const {
    assert(isValid());
    _lightingCtrl->_setLightRadius(_lightIndex, radius);
}

void LightingController::LightHandle::invalidate() {
    assert(isValid());
    _lightingCtrl->_removeLight(_lightIndex);
    _lightingCtrl = nullptr;
}

bool LightingController::LightHandle::isValid() const noexcept {
    return (_lightingCtrl != nullptr);
}

///////////////////////////////////////////////////////////////////////////////////////////////////

LightingController::LightingController(PZInteger width, PZInteger height,
                                       float cellResolution, Color ambientColor)
    : _world{width, height}
    , _cellResolution{cellResolution}
    , _ambientColor{ambientColor}
{
}

LightingController::LightingController()
    : LightingController(0, 0, 0.f)
{
}

LightingController::LightHandle LightingController::addLight(float x, float y, Color color, float radius) {
    auto lightIndex = _lightIndexer.acquire();

    if (_lights.size() <= lightIndex) {
        _lights.resize(lightIndex + 1);
    }

    _lights[lightIndex] = Light{x, y, radius, color};

    return LightHandle{*this, hg::ToPz(lightIndex)};
}

LightingController::Color LightingController::getColorAt(PZInteger x, PZInteger y) const {
    return _world.at(y, x).color;
}

void LightingController::render() {
    // Clear world with ambient light
    for (PZInteger y = 0; y < _world.getHeight(); y += 1) {
        for (PZInteger x = 0; x < _world.getWidth(); x += 1) {
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

void LightingController::smooth() {
    // Smooth all cells
    for (PZInteger y = 0; y < _world.getHeight(); y += 1) {
        for (PZInteger x = 0; x < _world.getWidth(); x += 1) {
            _smoothCell(x, y);
        }
    }

    // Copy results
    for (PZInteger y = 0; y < _world.getHeight(); y += 1) {
        for (PZInteger x = 0; x < _world.getWidth(); x += 1) {
            _world[y][x].color = _world[y][x].tmpColor;
        }
    }
}

void LightingController::resize(PZInteger width, PZInteger height, float cellResolution) {
    _world.resize(width, height);
    _cellResolution = _cellResolution;
}

void LightingController::setCellIsWall(PZInteger x, PZInteger y, bool isWall) {
    _world.at(y, x).isWall = isWall;
}

void LightingController::_smoothCell(PZInteger x, PZInteger y) {
    const int OFFSET = 1;
    const int DIVIDER = 3;

    auto& cell = _world[y][x];
    cell.tmpColor = cell.color;

    if (x - OFFSET > 0) {
        auto& other = _world[y][x - OFFSET];
        if (!other.isWall || cell.isWall) {
            cell.tmpColor.r += (other.color.r - std::min(other.color.r, cell.color.r)) / DIVIDER;
            cell.tmpColor.g += (other.color.g - std::min(other.color.g, cell.color.g)) / DIVIDER;
            cell.tmpColor.b += (other.color.b - std::min(other.color.b, cell.color.b)) / DIVIDER;
        }
    }

    if (y - OFFSET > 0) {
        auto& other = _world[y - OFFSET][x];
        if (!other.isWall || cell.isWall) {
            cell.tmpColor.r += (other.color.r - std::min(other.color.r, cell.color.r)) / DIVIDER;
            cell.tmpColor.g += (other.color.g - std::min(other.color.g, cell.color.g)) / DIVIDER;
            cell.tmpColor.b += (other.color.b - std::min(other.color.b, cell.color.b)) / DIVIDER;
        }
    }

    if (x + OFFSET < _world.getWidth()) {
        auto& other = _world[y][x + OFFSET];
        if (!other.isWall || cell.isWall) {
            cell.tmpColor.r += (other.color.r - std::min(other.color.r, cell.color.r)) / DIVIDER;
            cell.tmpColor.g += (other.color.g - std::min(other.color.g, cell.color.g)) / DIVIDER;
            cell.tmpColor.b += (other.color.b - std::min(other.color.b, cell.color.b)) / DIVIDER;
        }
    }

    if (y + OFFSET < _world.getHeight()) {
        auto& other = _world[y + OFFSET][x];
        if (!other.isWall || cell.isWall) {
            cell.tmpColor.r += (other.color.r - std::min(other.color.r, cell.color.r)) / DIVIDER;
            cell.tmpColor.g += (other.color.g - std::min(other.color.g, cell.color.g)) / DIVIDER;
            cell.tmpColor.b += (other.color.b - std::min(other.color.b, cell.color.b)) / DIVIDER;
        }
    }
}

void LightingController::_setLightPosition(PZInteger lightIndex, float x, float y) {
    _lights[hg::ToSz(lightIndex)].setPosition(x, y);
}

void LightingController::_setLightColor(PZInteger lightIndex, Color color) {
    _lights[hg::ToSz(lightIndex)].setColor(color);
}

void LightingController::_setLightRadius(PZInteger lightIndex, float radius) {
    _lights[hg::ToSz(lightIndex)].setRadius(radius);
}

void LightingController::_removeLight(PZInteger lightIndex) {
    _lights[hg::ToSz(lightIndex)] = Light(0.f, 0.f, 0.f, Color::Transparent);
    _lightIndexer.free(lightIndex);
}