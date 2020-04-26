
#include <algorithm>
#include <cstdlib>

#include "Game_context.hpp"
#include "Terrain_manager.hpp"

void TerrainManager::generate(hg::PZInteger width, hg::PZInteger height, float cellResolution) {
    _grid.resize(width, height);
    for (hg::PZInteger y = 0; y < _grid.getHeight(); y += 1) {
        for (hg::PZInteger x = 0; x < _grid.getWidth(); x += 1) {
            if ((std::rand() % 100) < 25) {
                _grid[y][x].color = hg::gr::Color::Black;
            }
            else {
                _grid[y][x].color = hg::gr::Color::DarkSlategrey;
            }
        }
    }

    _cellResolution = cellResolution;
}

void TerrainManager::eventDraw1() {
    auto& canvas = ctx().windowMgr.getCanvas();
    sf::RectangleShape rect({_cellResolution, _cellResolution});

    auto& view = ctx().windowMgr.getView();
    hg::PZInteger startX = std::max(0, (int)std::floor((view.getCenter().x - view.getSize().x / 2.f) / _cellResolution));
    hg::PZInteger startY = std::max(0, (int)std::floor((view.getCenter().y - view.getSize().y / 2.f) / _cellResolution));

    hg::PZInteger endX = std::min(_grid.getWidth() -1,
                                  (int)std::ceil((view.getCenter().x + view.getSize().x / 2.f) / _cellResolution));
    hg::PZInteger endY = std::min(_grid.getHeight() - 1,
                                  (int)std::ceil((view.getCenter().y + view.getSize().y / 2.f) / _cellResolution));

    for (hg::PZInteger y = startY; y < endY; y += 1) {
        for (hg::PZInteger x = startX; x < endX; x += 1) {
            rect.setFillColor(_grid[y][x].color);
            rect.setPosition(x * _cellResolution, y * _cellResolution);
            canvas.draw(rect);
        }
    }
}