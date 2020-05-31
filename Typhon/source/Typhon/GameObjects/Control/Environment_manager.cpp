
#include <algorithm>
#include <cstdlib>
#include <vector>

#include "Environment_manager.hpp"

// TODO Temp.
#include "../Gameplay/Physics_player.hpp"

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

using spempe::GameContext;
using spempe::NetworkingManager;

} // namespace

RN_DEFINE_HANDLER(ResizeTerrain, RN_ARGS(std::int32_t, width, std::int32_t, height)) {
    RN_NODE_IN_HANDLER().visit(
        [&](NetworkingManager::ClientType& client) {
            auto& ctx = *client.getUserData<GameContext>();
            auto& envMgr = GetEnvironmentManager(ctx);

            envMgr._resizeAllGrids(width, height);          
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
            auto& envMgr = GetEnvironmentManager(ctx);

            for (hg::PZInteger x = 0; x < envMgr.getTerrainColumnCount(); x += 1) {
                std::int16_t terrainTypeId;
                packet >> terrainTypeId;
                envMgr.setCellType(x, rowIndex, static_cast<Terrain::TypeId>(terrainTypeId));
            }          
        },
        [](NetworkingManager::ServerType& server) {
            // ERROR
        }
    );
}

EnvironmentManager::EnvironmentManager(QAO_RuntimeRef rtRef)
    : StateObject{rtRef, TYPEID_SELF, *PEXEPR_ENVIRON_MGR, "TerrainManager"}
    , _lightingCtrl{0, 0, 32.f, hg::gr::Color{5, 5, 10}}
{
    ctx(MNetworking).addEventListener(this);
}

EnvironmentManager::~EnvironmentManager() {
    ctx(MNetworking).removeEventListener(this);
}

void EnvironmentManager::generate(hg::PZInteger width, hg::PZInteger height, float cellResolution) {
    auto* space = ctx(DPhysicsSpace);

    _resizeAllGrids(width, height);

    for (hg::PZInteger y = 0; y < getTerrainRowCount(); y += 1) {
        for (hg::PZInteger x = 0; x < getTerrainColumnCount(); x += 1) {
            if ((std::rand() % 100) < 3 ||
                x == 0 || y == 0 || (x == (getTerrainColumnCount() - 1)) || (y == (getTerrainRowCount() - 1))) {
                setCellType(x, y, Terrain::TypeId::CaveWall);
            }
            else {
                setCellType(x, y, Terrain::TypeId::CaveFloor);
            }
        }
    }

    setCellType(1, 1, Terrain::TypeId::CaveFloor);
    // _cellResolution = cellResolution; TODO

    _setEnvironmentPhysicsParameters();
}

void EnvironmentManager::destroy() {
    _resizeAllGrids(0, 0);
    _cellResolution = 0.f;
}

void EnvironmentManager::setCellType(hg::PZInteger x, hg::PZInteger y, Terrain::TypeId typeId) {
    auto& tprop = Terrain::getTypeProperties(typeId);

    // Type ID:
    _typeIdGrid[y][x] = typeId;

    // Physics shape:
    auto vertices = GetShapeVertices(tprop.shape, _cellResolution);
    if (!vertices.empty()) {
        auto* space = ctx(DPhysicsSpace);
        auto* body = cpSpaceGetStaticBody(space);

        for (auto& vertex : vertices) {
            vertex.x += cpFloat{_cellResolution} * x;
            vertex.y += cpFloat{_cellResolution} * y;
        }

        auto* shape = cpPolyShapeNew(body,
                                     static_cast<int>(vertices.size()),
                                     vertices.data(),
                                     cpTransformIdentity,
                                     0.0);

        auto& physicsData = _physicsGrid[y][x];

        physicsData.shape.reset(cpSpaceAddShape(space, shape));
        cpShapeSetElasticity(shape, 0.5);
        Collideables::initTerrain(shape, physicsData);
        cpSpaceReindexShape(space, shape);
    }
    else {
        _physicsGrid[y][x].shape.reset();
    }
    
    // Lighting:
    if (tprop.shape == Terrain::CellShape::FullSquare) {
        _lightingCtrl.setCellIsWall(x, y, true);
    }
    else {
        _lightingCtrl.setCellIsWall(x, y, false);
    }
}

hg::PZInteger EnvironmentManager::getTerrainRowCount() const {
    return _typeIdGrid.getHeight();
}

hg::PZInteger EnvironmentManager::getTerrainColumnCount() const {
    return _typeIdGrid.getWidth();
}

LightingController::LightHandle EnvironmentManager::addLight(float x, float y, 
                                                             LightingController::Color color, float radius) {
    return _lightingCtrl.addLight(x, y, color, radius);
}

void EnvironmentManager::onNetworkingEvent(const RN_Event& ev) {
    if (!ctx().isPrivileged()) {
        return;
    }
    
    ev.visit(
        [this](const RN_Event::Connected& ev) {
            auto& node = ctx(MNetworking).getNode();
            auto receiverIndex = *ev.clientIndex;

            Compose_ResizeTerrain(node, receiverIndex, getTerrainColumnCount(), getTerrainRowCount());

            for (hg::PZInteger y = 0; y < getTerrainRowCount(); y += 1) {
                hg::util::Packet packet;
                for (hg::PZInteger x = 0; x < getTerrainColumnCount(); x += 1) {
                    packet << static_cast<std::int16_t>(_typeIdGrid[y][x]);
                }
                Compose_SetTerrainRow(node, receiverIndex, y, packet);
            }
        },
        [](const auto&) {}
    );
}

void EnvironmentManager::eventPostUpdate() {
    //_lightingCtrl.render();
    //_lightingCtrl.smooth();
}

void EnvironmentManager::eventDraw1() {
    auto& view = ctx(MWindow).getView();
    hg::PZInteger startX = std::max(0, (int)std::floor((view.getCenter().x - view.getSize().x / 2.f) / _cellResolution));
    hg::PZInteger startY = std::max(0, (int)std::floor((view.getCenter().y - view.getSize().y / 2.f) / _cellResolution));

    hg::PZInteger endX = std::min(getTerrainColumnCount(),
                                  (int)std::ceil((view.getCenter().x + view.getSize().x / 2.f) / _cellResolution));
    hg::PZInteger endY = std::min(getTerrainRowCount(),
                                  (int)std::ceil((view.getCenter().y + view.getSize().y / 2.f) / _cellResolution));

    for (hg::PZInteger y = startY; y < endY; y += 1) {
        for (hg::PZInteger x = startX; x < endX; x += 1) {
            _drawCell(x, y);
        }
    }
}

void EnvironmentManager::_setEnvironmentPhysicsParameters() const {
    //cpSpaceSetDamping(ctx(DPhysicsSpace), 0.1);
}

void EnvironmentManager::_resizeAllGrids(hg::PZInteger width, hg::PZInteger height) {
    _typeIdGrid.resize(width, height);
    _physicsGrid.resize(width, height);
    _lightingCtrl.resize(width, height, _cellResolution);

    for (hg::PZInteger y = 0; y < getTerrainRowCount(); y += 1) {
        for (hg::PZInteger x = 0; x < getTerrainColumnCount(); x += 1) {
            _physicsGrid[y][x].x = x;
            _physicsGrid[y][x].y = y;
        }
    }
}

void EnvironmentManager::_drawCell(hg::PZInteger x, hg::PZInteger y) {
    auto typeId = _typeIdGrid[y][x];

    auto& tprop = Terrain::getTypeProperties(typeId);
    
    auto iter = _spriteCache.find(tprop.spriteId);
    if (iter == _spriteCache.end()) {
        auto pair = _spriteCache.insert(std::make_pair(tprop.spriteId, ctx(DSprite, tprop.spriteId)));
        assert(pair.second);
        iter = pair.first;
    }

    auto& multisprite = (*iter).second;
    multisprite.setSubspriteIndex(0);
    multisprite.setPosition(x * _cellResolution, y * _cellResolution);
    //multisprite.setColor(_lightingCtrl.getColorAt(x, y));
    multisprite.setColor(hg::gr::Color::White);

    ctx(MWindow).getCanvas().draw(multisprite);
}