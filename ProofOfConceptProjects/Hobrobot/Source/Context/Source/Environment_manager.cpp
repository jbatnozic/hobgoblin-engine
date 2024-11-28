
#include <HobRobot/Context/Managers/Environment_manager.hpp>

#include <Hobgoblin/Math.hpp>
#include <Hobgoblin/Utility/Randomization.hpp>
#include <Hobgoblin/Graphics.hpp>

#include <cstdint>

namespace hobrobot {

namespace {
constexpr auto LOG_ID = "Hobrobot";

std::vector<cpVect> GetBoxVertices(cpFloat aCellResolution) {
    // IMPORTANT: The vertices must be in counter clockwise order and must form a convex shape
    std::vector<cpVect> rv;

    rv.push_back(cpv(0., 0.));
    rv.push_back(cpv(0., 1.));
    rv.push_back(cpv(1., 1.));
    rv.push_back(cpv(1., 0.));
    rv.push_back(cpv(0., 0.));

    for (auto& vect : rv) {
        vect = cpvmult(vect, aCellResolution);
    }

    return rv;
}
} // namespace

#define TYPE_EMPTY 0
#define TYPE_WALL  1

void EnvironmentManager_SetTerrain(EnvironmentManager& aEnvMgr,
                                   hg::PZInteger aWidth,
                                   hg::PZInteger aHeight,
                                   hg::PZInteger aCellResolution,
                                   hg::util::Packet& aTerrainData) {
    auto& self = aEnvMgr;
    self._grid.resize(aWidth, aHeight);
    self._cellResolution = aCellResolution;

    for (hg::PZInteger y = 0; y < aHeight; y += 1) {
        for (hg::PZInteger x = 0; x < aWidth; x += 1) {
            const auto type = aTerrainData.extract<std::int8_t>();
            self._grid.at(y, x).isEmpty = (type == TYPE_EMPTY);
        }
    }
}

RN_DEFINE_RPC(EnvironmentManager_SetTerrain, RN_ARGS(std::int32_t, aWidth,
                                                     std::int32_t, aHeight,
                                                     std::int32_t, aCellResolution,
                                                     hg::util::Packet&, aTerrainData)) {
    RN_NODE_IN_HANDLER().callIfClient(
        [&](RN_ClientInterface& aClient) {
            HG_LOG_INFO(LOG_ID, "SetTerrain message received.");
            const auto rc = spe::RPCReceiverContext{aClient};
            auto& envMgr = dynamic_cast<EnvironmentManager&>(
                rc.gameContext.getComponent<EnvironmentManagerInterface>()
            );
            EnvironmentManager_SetTerrain(envMgr, aWidth, aHeight, aCellResolution, aTerrainData);
        }
    );
    RN_NODE_IN_HANDLER().callIfServer(
        [&](RN_ServerInterface& aServer) {
            throw RN_IllegalMessage{};
        }
    );
}

EnvironmentManager::EnvironmentManager(QAO_RuntimeRef aRuntimeRef, int aExecutionPriority)
    : NonstateObject{aRuntimeRef, SPEMPE_TYPEID_SELF, aExecutionPriority, "hobrobot::EnvironmentManager"}
{
}

EnvironmentManager::~EnvironmentManager() {
    if (_mode == Mode::Host) {
        ccomp<MNetworking>().removeEventListener(this);
    }
    if (_physicsSpace) {
        _grid.resize(0, 0);

        cpSpaceDestroy(_physicsSpace);
        _physicsSpace = nullptr;
    }
}

void EnvironmentManager::setToHostMode() {
    SPEMPE_VALIDATE_GAME_CONTEXT_FLAGS(ctx(), privileged==true);
    _mode = Mode::Host;

    _physicsSpace = cpSpaceNew();
    cpSpaceSetGravity(_physicsSpace, cpv(0.0, 120.0));
    cpSpaceSetDamping(_physicsSpace, 0.75);

    ccomp<MNetworking>().addEventListener(this);

    _generateTerrain(200, 20, 45);
    // cpSpaceSet...
    //auto* body = cpSpaceGetStaticBody(_physicsSpace);
    //auto* shape = cpSpaceAddShape(_physicsSpace, cpSegmentShapeNew(body, cpv(-10000.0, 800.0), cpv(10000.0, 800.0), 1.0));
    //cpShapeSetElasticity(shape, 0.1);
    //cpShapeSetFriction(shape, 0.5);
}

void EnvironmentManager::setToClientMode() {
    SPEMPE_VALIDATE_GAME_CONTEXT_FLAGS(ctx(), privileged==false);
    _mode = Mode::Client;
}

EnvironmentManager::Mode EnvironmentManager::getMode() const {
    return _mode;
}

cpSpace* EnvironmentManager::getPhysicsSpace() const {
    return _physicsSpace;
}

void EnvironmentManager::onNetworkingEvent(const RN_Event& aEvent) {
    aEvent.visit(
        [this](const RN_Event::Connected& ev) {
            auto& server = ccomp<MNetworking>().getServer();
            const auto receiverIndex = *ev.clientIndex;

            if (server.getClientConnector(receiverIndex).getStatus() != RN_ConnectorStatus::Connected) {
                // If we have both Connected and Disconnected queued up, it means this
                // client is actually no longer connected..
                return;
            }

            hg::util::Packet packet;
            for (hg::PZInteger y = 0; y < _grid.getHeight(); y += 1) {
                for (hg::PZInteger x = 0; x < _grid.getWidth(); x += 1) {
                    packet << static_cast<std::int8_t>(_grid.at(y, x).isEmpty ? TYPE_EMPTY : TYPE_WALL);
                }
            }

            HG_LOG_INFO(LOG_ID, "Composing SetTerrain message for client {}.", receiverIndex);
            Compose_EnvironmentManager_SetTerrain(
                server,
                receiverIndex,
                _grid.getWidth(),
                _grid.getHeight(),
                _cellResolution,
                packet
            );
        }
    );
}

void EnvironmentManager::_eventPostUpdate() {
    if (_physicsSpace) {
        const cpFloat deltaTimeInSeconds = ctx().getRuntimeConfig().tickRate.getDeltaTime().count();
        cpSpaceStep(_physicsSpace, deltaTimeInSeconds);
    }
}

void EnvironmentManager::_eventDraw1() {
    auto& canvas = ccomp<MWindow>().getCanvas();
    hg::gr::RectangleShape rect;
    rect.setSize({static_cast<float>(_cellResolution), static_cast<float>(_cellResolution)});
    rect.setFillColor(hg::gr::COLOR_STEEL_BLUE);
    rect.setOutlineThickness(3.0);
    rect.setOutlineColor(hg::gr::COLOR_DARK_ORANGE);

    for (hg::PZInteger y = 0; y < _grid.getHeight(); y += 1) {
        for (hg::PZInteger x = 0; x < _grid.getWidth(); x += 1) {
            if (_grid.at(y, x).isEmpty) continue;

            rect.setPosition(
                static_cast<float>(x * _cellResolution),
                static_cast<float>(y * _cellResolution)
            );
            canvas.draw(rect);
        }
    }
}

void EnvironmentManager::_generateTerrain(hg::PZInteger aWidth,
                                          hg::PZInteger aHeight,
                                          hg::PZInteger aCellResolution) {
    _grid.resize(aWidth, aHeight);
    _cellResolution = aCellResolution;

    hg::PZInteger floorHeight = aHeight - 1;

    for (hg::PZInteger x = 0; x < aWidth; x += 1) {
        if (hg::util::GetRandomNumber<float>(0.f, 100.f) < (100.f * static_cast<float>(x) / aWidth)) {
            const auto mod = hg::util::GetRandomNumber<int>(0, 1) * 2 - 1;
            floorHeight = hg::math::Clamp(floorHeight + mod, 0, aHeight - 1);
        }

        for (hg::PZInteger y = 0; y < aHeight; y += 1) {
            if (x == 0 || y == 0 || x == aWidth - 1 || y == aHeight - 1) {
                _setCellToWall(x, y);
                continue;
            }

            if (y >= floorHeight) {
                _setCellToWall(x, y);
                continue;
            }

            _setCellToEmpty(x, y);
        }
    }
}

void  EnvironmentManager::_setCellToEmpty(hg::PZInteger aX, hg::PZInteger aY) {
    _grid.at(aY, aX).shape.reset(nullptr);
    _grid.at(aY, aX).isEmpty = true;
}

void  EnvironmentManager::_setCellToWall(hg::PZInteger aX, hg::PZInteger aY) {
    const auto cellRes = static_cast<cpFloat>(_cellResolution);
    auto vertices = GetBoxVertices(cellRes);
    for (auto& [x, y] : vertices) {
        x += cellRes * aX;
        y += cellRes * aY;
    }

    auto* spaceStaticBody = cpSpaceGetStaticBody(_physicsSpace);
    auto* shape = cpSpaceAddShape(
        _physicsSpace,
        cpPolyShapeNew(
            spaceStaticBody,
            static_cast<int>(vertices.size()),
            vertices.data(),
            cpTransformIdentity,
            1.0
        )
    );
    cpShapeSetElasticity(shape, 0.1);
    cpShapeSetFriction(shape, 1.0);

    _grid.at(aY, aX).shape.reset(shape);
    _grid.at(aY, aX).isEmpty = false;
}

} // namespace hobrobot
