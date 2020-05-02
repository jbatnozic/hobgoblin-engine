
#include <Hobgoblin/RigelNet_Macros.hpp>

#include <algorithm>
#include <cstdlib>
#include <vector>

#include "GameContext/Game_context.hpp"
#include "GameObjects/Framework/Execution_priorities.hpp"
#include "GameObjects/Managers/Terrain_manager.hpp"

namespace {

std::vector<cpVect> GetShapeVertices(Terrain::CellShape shape, cpFloat cellResolution) {
    // IMPORTANT: The vertices must be in counter clockwise order and must form a convex shape
    std::vector<cpVect> rv;

    switch (shape) {
    case Terrain::CellShape::Empty:
        // Do nothing
        break;

    case Terrain::CellShape::FullSquare:
        rv.push_back(cpv(0., 0.));
        rv.push_back(cpv(0., 1.));
        rv.push_back(cpv(1., 1.));
        rv.push_back(cpv(1., 0.));
        rv.push_back(cpv(0., 0.));
        break;

    default:
        assert(0 && "Unreachable");
    }

    for (auto& vect : rv) {
        vect = cpvmult(vect, cellResolution);
    }

    return rv;
}

} // namespace

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

TerrainManager::TerrainManager(QAO_RuntimeRef rtRef, SynchronizedObjectManager& syncObjMgr, SyncId syncId)
    : GOF_SynchronizedObject{rtRef, TYPEID_SELF, EXEPR_TERRAIN_MGR, "TerrainManager", syncObjMgr, syncId}
{
}

TerrainManager::~TerrainManager() {
    if (isMasterObject()) {
        syncDestroy();
    }
}

void TerrainManager::generate(hg::PZInteger width, hg::PZInteger height, float cellResolution) {
    auto* space = ctx().getPhysicsSpace();

    _resizeAllGrids(width, height);

    for (hg::PZInteger y = 0; y < _grid.getHeight(); y += 1) {
        for (hg::PZInteger x = 0; x < _grid.getWidth(); x += 1) {
            if ((std::rand() % 100) < 15 ||
                x == 0 || y == 0 || (x == (_grid.getWidth() - 1)) || (y == (_grid.getHeight() - 1))) {

                setCellType(x, y, Terrain::TypeId::CaveWall);
            }
            else {
                setCellType(x, y, Terrain::TypeId::CaveFloor);
            }
        }
    }

    setCellType(1, 1, Terrain::TypeId::CaveFloor);
    // _cellResolution = cellResolution; TODO
}

void TerrainManager::destroy() {
    _resizeAllGrids(0, 0);
    _cellResolution = 0.f;
}

void TerrainManager::setCellType(hg::PZInteger x, hg::PZInteger y, Terrain::TypeId typeId) {
    auto& tprop = Terrain::getTypeProperties(typeId);

    // Type ID:
    _typeIdGrid[y][x] = typeId;

    // Physics shape:
    auto* space = ctx().getPhysicsSpace();
    auto* body = cpSpaceGetStaticBody(space);
    auto vertices = GetShapeVertices(tprop.shape, _cellResolution);
    for (auto& vertex : vertices) {
        vertex.x += cpFloat{_cellResolution} * x;
        vertex.y += cpFloat{_cellResolution} * y;
    }

    auto* shape = cpPolyShapeNew(body,
                                 static_cast<int>(vertices.size()),
                                 vertices.data(),
                                 cpTransformIdentity,
                                 0.0);
    _physicsGrid[y][x].shape = hg::cpShapeUPtr{cpSpaceAddShape(space, shape)};

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
    //auto& canvas = ctx().windowMgr.getCanvas();
    //sf::RectangleShape rect({_cellResolution, _cellResolution});

    auto& view = ctx().windowMgr.getView();
    hg::PZInteger startX = std::max(0, (int)std::floor((view.getCenter().x - view.getSize().x / 2.f) / _cellResolution));
    hg::PZInteger startY = std::max(0, (int)std::floor((view.getCenter().y - view.getSize().y / 2.f) / _cellResolution));

    hg::PZInteger endX = std::min(_grid.getWidth(),
                                  (int)std::ceil((view.getCenter().x + view.getSize().x / 2.f) / _cellResolution));
    hg::PZInteger endY = std::min(_grid.getHeight(),
                                  (int)std::ceil((view.getCenter().y + view.getSize().y / 2.f) / _cellResolution));

    for (hg::PZInteger y = startY; y < endY; y += 1) {
        for (hg::PZInteger x = startX; x < endX; x += 1) {
            //rect.setFillColor(_grid[y][x].color);
            //rect.setPosition(x * _cellResolution, y * _cellResolution);
            //canvas.draw(rect);
            _drawCell(x, y);
        }
    }
}

void TerrainManager::_resizeAllGrids(hg::PZInteger width, hg::PZInteger height) {
    _typeIdGrid.resize(width, height);
    _grid.resize(width, height);
    _physicsGrid.resize(width, height);
}

void TerrainManager::_drawCell(hg::PZInteger x, hg::PZInteger y) {
    auto typeId = _typeIdGrid[y][x];

    auto& tprop = Terrain::getTypeProperties(typeId);
    
    auto iter = _spriteCache.find(tprop.spriteId);
    if (iter == _spriteCache.end()) {
        auto pair = _spriteCache.insert(std::make_pair(tprop.spriteId, getspr(tprop.spriteId)));
        assert(pair.second);
        iter = pair.first;
    }

    auto& multisprite = (*iter).second;
    multisprite.setSubspriteIndex(0);
    multisprite.setPosition(x * _cellResolution, y * _cellResolution);
    multisprite.setColor(sf::Color::White);

    ctx().windowMgr.getCanvas().draw(multisprite);
}