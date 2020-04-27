
#include <Hobgoblin/RigelNet_Macros.hpp>

#include <algorithm>
#include <cstdlib>

#include "Game_context.hpp"
#include "Terrain_manager.hpp"

RN_DEFINE_HANDLER(ResizeTerrain, RN_ARGS(std::int32_t, width, std::int32_t, height)) {
    RN_NODE_IN_HANDLER().visit(
        [&](NetworkingManager::ClientType& client) {
            auto& ctx = *client.getUserData<GameContext>();
            // TODO Fetch TerrainManager instance through SyncObjMgr

            ctx.terrMgr._grid.resize(width, height);
        },
        [](NetworkingManager::ServerType& server) {
            // ERROR
        }
    );
}

RN_DEFINE_HANDLER(SetTerrainRow, RN_ARGS(std::int32_t, rowIndex, hg::util::Packet&, packet)) {
    RN_NODE_IN_HANDLER().visit(
        [&](NetworkingManager::ClientType& client) {
            auto& ctx = *client.getUserData<GameContext>();
            // TODO Fetch TerrainManager instance through SyncObjMgr

            for (hg::PZInteger x = 0; x < ctx.terrMgr._grid.getWidth(); x += 1) {
                packet >> ctx.terrMgr._grid[rowIndex][x];
            }
        },
        [](NetworkingManager::ServerType& server) {
            // ERROR
        }
    );
}

TerrainManager::~TerrainManager() {
    if (isMasterObject()) {
        syncDestroy();
    }
}

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

void TerrainManager::syncCreateImpl(RN_Node& node, const std::vector<hg::PZInteger>& rec) const {
    Compose_ResizeTerrain(node, rec, _grid.getWidth(), _grid.getHeight());
    
    for (hg::PZInteger y = 0; y < _grid.getHeight(); y += 1) {
        hg::util::Packet packet;
        for (hg::PZInteger x = 0; x < _grid.getWidth(); x += 1) {
            packet << _grid[y][x];
        }
        Compose_SetTerrainRow(node, rec, y, packet);
    }
}

void TerrainManager::syncUpdateImpl(RN_Node& node, const std::vector<hg::PZInteger>& rec) const {}
void TerrainManager::syncDestroyImpl(RN_Node& node, const std::vector<hg::PZInteger>& rec) const {}

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