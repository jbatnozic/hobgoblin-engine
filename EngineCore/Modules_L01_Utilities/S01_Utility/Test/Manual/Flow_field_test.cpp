// Copyright 2024 Jovan Batnozic. Released under MS-PL licence in Serbia.
// See https://github.com/jbatnozic/Hobgoblin?tab=readme-ov-file#licence

#ifndef UHOBGOBLIN_UTIL_FLOW_FIELD_HPP
#define UHOBGOBLIN_UTIL_FLOW_FIELD_HPP

#include <Hobgoblin/Common.hpp>
#include <Hobgoblin/HGExcept.hpp>
#include <Hobgoblin/Math/Angle.hpp>
#include <Hobgoblin/Math/Vector.hpp>
#include <Hobgoblin/Utility/Grids.hpp>

#include <array>
#include <cstdint>
#include <deque> // TODO: replace with more efficient deque implementation

#include <Hobgoblin/Private/Pmacro_define.hpp>

HOBGOBLIN_NAMESPACE_BEGIN
namespace util {

class DataProvider {
public:
    std::uint8_t getCostAt(math::Vector2pz aPosition) {
        return costs.at(aPosition.y, aPosition.x);
    }

    util::RowMajorGrid<std::uint8_t> costs;
};

using FlowField = util::RowMajorGrid<math::AngleF>;
using IntegrationField = util::RowMajorGrid<std::int32_t>;

class FlowFieldCalculator {
    using NeighbourArray = std::array<std::optional<math::Vector2pz>, 8>;
public:
    void setFieldDimensions(math::Vector2pz aDimensions) {
        _fieldDimensions = aDimensions;
    }

    FlowField calculate(math::Vector2pz aTarget) {
        HG_VALIDATE_ARGUMENT(aTarget.x < _fieldDimensions.x && aTarget.y < _fieldDimensions.y,
                             "Target must be within the field.");

        auto flowField = FlowField{_fieldDimensions.x, _fieldDimensions.y, math::AngleF::fromDeg(361.0)};
        auto integrationField =
            IntegrationField{_fieldDimensions.x, _fieldDimensions.y, 65535};

        _queue.clear();
        integrationField[aTarget.y][aTarget.x] = 0;
        _queue.push_back(aTarget);

        while (!_queue.empty()) {
            const auto curr = _queue.front();
            _queue.pop_front();

            NeighbourArray neighbours;
            _getValidNeighboursAroundPosition(curr, &neighbours, false);

            for (PZInteger i = 0; i < 8; i += 1) {
                if (!neighbours[pztos(i)].has_value()) {
                    continue;
                }

                const auto neighbourOffset = neighbours[pztos(i)];
                const auto neighbourCost = getCostAt(curr + *neighbourOffset);
                if (neighbourCost == 255) {
                    continue; // Impassable cell
                }

                const auto costFromCurrToNeighbour =
                    integrationField[curr.y][curr.x] + neighbourCost;

                if (costFromCurrToNeighbour >= integrationField[(curr + *neighbourOffset).y][(curr + *neighbourOffset).x]) {
                    continue; // Neighbour already has a better path
                }

                integrationField[(curr + *neighbourOffset).y][(curr + *neighbourOffset).x] = costFromCurrToNeighbour;
                // flowField[(curr + *neighbourOffset).y][(curr + *neighbourOffset).x] = _directionFromNeighbour(i);
                _queue.push_back(curr + *neighbourOffset);
            }
        }

        // Make FlowField
        for (PZInteger y = 0; y < _fieldDimensions.y; y += 1) {
            for (PZInteger x = 0; x < _fieldDimensions.x; x += 1) {
                std::int32_t minCost = 2'000'000'000;

                NeighbourArray neighbours;
                _getValidNeighboursAroundPosition({x, y}, &neighbours, true);

                for (PZInteger i = 0; i < 8; i += 1) {
                    if (!neighbours[pztos(i)].has_value()) {
                        continue;
                    }
                    const auto neighbourOffset = neighbours[pztos(i)];

                    if (neighbourOffset->x == -1 && neighbourOffset->y == -1) {
                        if (getCostAt({x - 1, y}) == 255 || getCostAt({x, y - 1}) == 255) {
                            continue;
                        }
                    } else if (neighbourOffset->x == +1 && neighbourOffset->y == -1) {
                        if (getCostAt({x + 1, y}) == 255 || getCostAt({x, y - 1}) == 255) {
                            continue;
                        }    
                    } else if (neighbourOffset->x == -1 && neighbourOffset->y == +1) {
                        if (getCostAt({x - 1, y}) == 255 || getCostAt({x, y + 1}) == 255) {
                            continue;
                        }  
                    } else if (neighbourOffset->x == +1 && neighbourOffset->y == +1) {
                        if (getCostAt({x + 1, y}) == 255 || getCostAt({x, y + 1}) == 255) {
                            continue;
                        }   
                    }

                    const auto cost = integrationField[y + neighbourOffset->y][x + neighbourOffset->x];
                    if (cost < minCost) {
                        minCost = cost;
                        flowField[y][x] = _directionTowardsNeighbour(i);
                    }
                }
            }
        }

        return flowField;
    }

    int getCostAt(math::Vector2pz aPosition) {
        return dataProvider->getCostAt(aPosition);
    }

    DataProvider* dataProvider = nullptr;

private:
    std::deque<math::Vector2pz> _queue;
    math::Vector2pz _fieldDimensions;

    //! 0 1 2
    //! 3 X 4
    //! 5 6 7
    void _getValidNeighboursAroundPosition(math::Vector2pz aPosition,
                                           NeighbourArray* aNeighbours,
                                           bool aDiagonalAllowed) const {
        static const std::array<math::Vector2i, 8> OFFSETS = {
            // Row above
            math::Vector2i{-1, -1},
            { 0, -1},
            {+1, -1},
            // Same row
            {-1,  0},
            // { 0,  0}, -- self
            {+1,  0},
            // Row below
            {-1, +1},
            { 0, +1},
            {+1, +1}
        };

        for (std::size_t i = 0; i < 8; i += 1) {
            const auto xOff = OFFSETS[i].x;
            const auto yOff = OFFSETS[i].y;
            if (!aDiagonalAllowed && !(xOff == 0 || yOff == 0)) {
                (*aNeighbours)[i].reset();
                continue;
            }

            if (aPosition.y + yOff < 0 || aPosition.y + yOff >= _fieldDimensions.y ||
                aPosition.x + xOff < 0 || aPosition.x + xOff >= _fieldDimensions.x) {
                (*aNeighbours)[i].reset();
                continue;
            }

            (*aNeighbours)[i] = {xOff, yOff};
        }
    }

    // TODO: make free function
    static constexpr math::AngleF _directionTowardsNeighbour(PZInteger aNeighbourIndex) {
        constexpr std::array<math::AngleF, 8> DIRECTIONS = {
            math::AngleF::fromDegrees(135.f),
            math::AngleF::fromDegrees( 90.f),
            math::AngleF::fromDegrees( 45.f),
            math::AngleF::fromDegrees(180.f),
            math::AngleF::fromDegrees(  0.f),
            math::AngleF::fromDegrees(225.f),
            math::AngleF::fromDegrees(270.f),
            math::AngleF::fromDegrees(315.f)
        };
        return DIRECTIONS[pztos(aNeighbourIndex)];
    }

    // TODO: make free function
    static constexpr math::AngleF _directionFromNeighbour(PZInteger aNeighbourIndex) {
        constexpr std::array<math::AngleF, 8> DIRECTIONS = {
            math::AngleF::fromDegrees(315.f),
            math::AngleF::fromDegrees(270.f),
            math::AngleF::fromDegrees(225.f),
            math::AngleF::fromDegrees(  0.f),
            math::AngleF::fromDegrees(180.f),
            math::AngleF::fromDegrees( 45.f),
            math::AngleF::fromDegrees( 90.f),
            math::AngleF::fromDegrees(135.f)
        };
        return DIRECTIONS[pztos(aNeighbourIndex)];
    }
};

} // namespace util
HOBGOBLIN_NAMESPACE_END

#include <Hobgoblin/Private/Pmacro_define.hpp>

#endif // !UHOBGOBLIN_UTIL_FLOW_FIELD_HPP

// ****************************************************************************
// MANUAL TEST

#include <SFML/Graphics.hpp>
#include <SFML/Window.hpp>

#include <cmath>
#include <chrono>
#include <iostream>

namespace hg = jbatnozic::hobgoblin;

#define GRID_W 512
#define GRID_H 512

static void DrawDirection(sf::RenderTarget& aTarget,
                          sf::Vector2f aPosition,
                          hg::math::AngleF aDirection,
                          sf::Color aColor) {
    sf::RectangleShape rect{{8.f, 2.f}};
    rect.setOrigin({0.f, 1.f});
    rect.setFillColor(aColor);
    rect.setRotation(-aDirection.asDeg());
    rect.setPosition(aPosition);
    aTarget.draw(rect);
}

int main(int argc, char* argv[]) {
    sf::RenderWindow window{sf::VideoMode{1200, 900}, "FlowFields", sf::Style::Default};
    window.setFramerateLimit(60);

    hg::util::DataProvider provider;
    provider.costs.resize(GRID_W, GRID_H);
    provider.costs.setAll(1);

    hg::util::FlowFieldCalculator calc;
    calc.setFieldDimensions({GRID_W, GRID_H});
    calc.dataProvider = &provider;

    hg::util::FlowField ff;
    ff = calc.calculate({0, 0});

    while (window.isOpen()) {
        sf::Event ev;
        while (window.pollEvent(ev)) {
            if (ev.type == sf::Event::Closed) {
                window.close();
                break;
            }
            if (ev.type == sf::Event::MouseButtonPressed) {
                const auto x = ev.mouseButton.x;
                const auto y = ev.mouseButton.y;
                const auto worldXY = window.mapPixelToCoords({x, y});
                const auto gridX = static_cast<int>(std::floor(worldXY.x) / 16.f);
                const auto gridY = static_cast<int>(std::floor(worldXY.y) / 16.f);
                if (gridX >= 0 && gridY >= 0 && gridX < GRID_W && gridY < GRID_H) {
                    if (ev.mouseButton.button == sf::Mouse::Left) {
                        auto& val = provider.costs.at(gridY, gridX);
                        val = (val == 1) ? 255 : 1;
                    } else if (ev.mouseButton.button == sf::Mouse::Right) {
                        const auto start = std::chrono::steady_clock::now();
                        ff = calc.calculate({gridX, gridY});
                        const auto end = std::chrono::steady_clock::now();
                        std::cout << "Calculate took "
                                  << std::chrono::duration_cast<std::chrono::microseconds>(end - start).count() / 1000.0
                                  << "ms.\n";
                    }
                }
            }
        }

        window.clear(sf::Color{125, 125, 125});

        sf::RectangleShape rect{{16.f, 16.f}};
        rect.setFillColor(sf::Color::Transparent);
        rect.setOutlineColor(sf::Color::Black);
        rect.setOutlineThickness(1.0);
        for (hg::PZInteger y = 0; y < GRID_H; y += 1) {
            for (hg::PZInteger x = 0; x < GRID_W; x += 1) {
                if (provider.costs[y][x] == 1) {
                    rect.setFillColor(sf::Color::Transparent);
                } else {
                    rect.setFillColor(sf::Color::Black);
                }

                rect.setPosition({x * 16.f, y * 16.f});
                window.draw(rect);

                DrawDirection(window, 
                              {x * 16.f + 8.f, y * 16.f + 8.f}, 
                              ff[y][x], 
                              (ff[y][x] == hg::math::AngleF::fromDeg(361.f)) ? sf::Color::Red : sf::Color::Green);
            }
        }

        window.display();
    }

    return EXIT_SUCCESS;
}
