// Copyright 2024 Jovan Batnozic. Released under MS-PL licence in Serbia.
// See https://github.com/jbatnozic/Hobgoblin?tab=readme-ov-file#licence

// clang-format off

#include <Hobgoblin/Logging.hpp>
#include <Hobgoblin/Utility/Flow_field_spooler.hpp>

#include <SFML/Graphics.hpp>
#include <SFML/Window.hpp>

#include <cmath>
#include <chrono>
#include <iostream>

namespace hg = jbatnozic::hobgoblin;

#define GRID_W (3*100)
#define GRID_H (3*100)
#define COST_PROVIDER_ID 5

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

class CostProvider {
public:
    std::uint8_t getCostAt(hg::math::Vector2pz aPosition) const {
        return costs.at(aPosition.y, aPosition.x);
    }

    hg::util::RowMajorGrid<std::uint8_t> costs;
};

int main(int argc, char* argv[]) {
    hg::log::SetMinimalLogSeverity(hg::log::Severity::All);

    sf::RenderWindow window{sf::VideoMode{1200, 900}, "FlowFields", sf::Style::Default};
    window.setFramerateLimit(60);

    CostProvider provider;
    provider.costs.resize(GRID_W, GRID_H);
    provider.costs.setAll(1);

    for (hg::PZInteger y = 0; y < GRID_H; y += 1) {
        for (hg::PZInteger x = 0; x < GRID_W; x += 1) {
            if (x % 2 == 0 && y % 2 == 0) {
                provider.costs[y][x] = 255;
            }
        }
    }

    hg::util::FlowFieldSpooler<CostProvider> spooler{{{COST_PROVIDER_ID, &provider}}, 8};

    hg::util::FlowField ff;
    {
        const auto id = spooler.addRequest({0, 0}, {GRID_W, GRID_H}, {0, 0}, COST_PROVIDER_ID, 1);
        spooler.tick();
        ff = spooler.collectResult(id)->flowField;
    }

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
                        const auto id1 = spooler.addRequest({0, 0}, {GRID_W, GRID_H}, {gridX, gridY}, COST_PROVIDER_ID, 1);
                        const auto id2 = spooler.addRequest({0, 0}, {GRID_W, GRID_H}, {gridX, gridY}, COST_PROVIDER_ID, 1);
                        const auto id3 = spooler.addRequest({0, 0}, {GRID_W, GRID_H}, {gridX, gridY}, COST_PROVIDER_ID, 1);
                        const auto id4 = spooler.addRequest({0, 0}, {GRID_W, GRID_H}, {gridX, gridY}, COST_PROVIDER_ID, 1);
                        spooler.cancelRequest(id2);
                        spooler.tick();
                        (void)spooler.collectResult(id4)->flowField;
                        (void)spooler.collectResult(id3)->flowField;
                        // (void)spooler.collectResult(id2)->flowField;
                        ff = spooler.collectResult(id1)->flowField;
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
                bool drawRect = true;
                if (provider.costs[y][x] == 1) {
                    drawRect = false;
                } else {
                    rect.setPosition({x * 16.f, y * 16.f});
                    rect.setFillColor(sf::Color::Black);
                }

                if (drawRect) {
                    window.draw(rect);
                }

                DrawDirection(window, 
                              {x * 16.f + 8.f, y * 16.f + 8.f}, 
                              (ff[y][x].hasValue()) ? ff[y][x].getValue() : hg::math::AngleF::zero(), 
                              (ff[y][x].hasValue()) ? sf::Color::Green : sf::Color::Red);
            }
        }

        window.display();
    }

    return EXIT_SUCCESS;
}

// clang-format on
