#include "Environment_manager.hpp"

#include <Hobgoblin/HGExcept.hpp>
#include <Hobgoblin/RigelNet_macros.hpp>

#include "Collisions.hpp"

#include <array>

void SetTerrainImpl(EnvironmentManager& aEnvMgr,
                    hg::PZInteger       aWidth,
                    hg::PZInteger       aHeight,
                    hg::PZInteger       aRowIdx,
                    const std::string&  aCellData);

namespace {
constexpr cpFloat CELL_RESOLUTION = 64.0;

NeverNull<cpShape*> CreateRectanglePolyShape(NeverNull<cpBody*>  aBody,
                                             hg::math::Vector2pz aGridPosition) {
    std::array<cpVect, 5> vertices = {
        cpv(0.0, 0.0),
        cpv(1.0, 0.0),
        cpv(1.0, 1.0),
        cpv(0.0, 1.0),
        cpv(0.0, 0.0),
    };
    for (auto& v : vertices) {
        v.x += (cpFloat)aGridPosition.x;
        v.x *= CELL_RESOLUTION;

        v.y += (cpFloat)aGridPosition.y;
        v.y *= CELL_RESOLUTION;
    }

    return cpPolyShapeNew(aBody,
                          static_cast<int>(vertices.size()),
                          vertices.data(),
                          cpTransformIdentity,
                          0.0);
}

RN_DEFINE_RPC(SetTerrain,
              RN_ARGS(std::int32_t,
                      aWidth,
                      std::int32_t,
                      aHeight,
                      std::int32_t,
                      aRowIdx,
                      std::string&,
                      aCellData)) {
    RN_NODE_IN_HANDLER().callIfServer([](RN_ServerInterface& aServer) {
        throw RN_IllegalMessage{};
    });
    RN_NODE_IN_HANDLER().callIfClient([&](RN_ClientInterface& aClient) {
        const spe::RPCReceiverContext rc{aClient};
        auto& envMgr = static_cast<EnvironmentManager&>(rc.gameContext.getComponent<MEnvironment>());
        SetTerrainImpl(envMgr, aWidth, aHeight, aRowIdx, aCellData);
    });
}
} // namespace

void SetTerrainImpl(EnvironmentManager& aEnvMgr,
                    hg::PZInteger       aWidth,
                    hg::PZInteger       aHeight,
                    hg::PZInteger       aRowIdx,
                    const std::string&  aCellData) {
    if (aEnvMgr._cells.getWidth() != aWidth || aEnvMgr._cells.getHeight() != aHeight) {
        aEnvMgr._cells.resize(aWidth, aHeight);
    }

    HG_HARD_ASSERT(hg::stopz(aCellData.size()) == aWidth);

    std::size_t idx = 0;
    for (hg::PZInteger x = 0; x < aWidth; x += 1) {
        aEnvMgr._cells[aRowIdx][x] = static_cast<CellKind>(aCellData[idx]);
        idx++;
    }
}

EnvironmentManager::EnvironmentManager(QAO_RuntimeRef aRuntimeRef, int aExecutionPriority)
    : NonstateObject{aRuntimeRef, SPEMPE_TYPEID_SELF, aExecutionPriority, "EnvironmentManager"} {
    ccomp<MNetworking>().addEventListener(this);
}

EnvironmentManager::~EnvironmentManager() {
    ccomp<MNetworking>().removeEventListener(this);
}

void EnvironmentManager::setToHeadlessHostMode() {
    SPEMPE_VALIDATE_GAME_CONTEXT_FLAGS(ctx(), privileged == true, headless == true);
    HG_HARD_ASSERT(_mode == Mode::UNINITIALIZED);
    _mode = Mode::HEADLESS_HOST;

    _collisionDispatcher.emplace();
    _space.emplace();
    InitColliders(*_collisionDispatcher, *_space);

    generateTerrain(128, 128);
}

void EnvironmentManager::setToClientMode() {
    SPEMPE_VALIDATE_GAME_CONTEXT_FLAGS(ctx(), privileged == false, headless == false);
    HG_HARD_ASSERT(_mode == Mode::UNINITIALIZED);
    _mode = Mode::CLIENT;
}

EnvironmentManager::Mode EnvironmentManager::getMode() const {
    return _mode;
}

void EnvironmentManager::generateTerrain(hg::PZInteger aWidth, hg::PZInteger aHeight) {
    // Cells
    _cells.resize(aWidth, aHeight);
    _cells.setAll(CellKind::ROCK);

    // Collision delegate
    _collisionDelegate.emplace(hg::alvin::CollisionDelegateBuilder{}
                                   .setDefaultDecision(hg::alvin::Decision::ACCEPT_COLLISION)
                                   .finalize());

    // Terrain body
    _terrainBody.emplace(hg::alvin::Body::createStatic());

    // Shapes
    _shapes.resize(aWidth, aHeight);
    for (hg::PZInteger y = 0; y < aHeight; y += 1) {
        for (hg::PZInteger x = 0; x < aWidth; x += 1) {
            if (_cells[y][x] != CellKind::EMPTY) {
                auto alvinShape = hg::alvin::Shape{CreateRectanglePolyShape(*_terrainBody, {x, y})};
                {
                    auto pair = _shapeToPosition.insert(
                        std::make_pair(static_cast<cpShape*>(alvinShape), hg::math::Vector2pz{x, y}));
                    HG_HARD_ASSERT(pair.second && "Insertion must happen!");
                }
                _shapes[y][x].emplace(std::move(alvinShape));
                _collisionDelegate->bind(*this, *_shapes[y][x]);
            }
        }
    }
}

hg::alvin::Space& EnvironmentManager::getSpace() {
    HG_ASSERT(_mode == Mode::HEADLESS_HOST && _space.has_value());
    return *_space;
}

std::optional<CellKind> EnvironmentManager::getCellKindOfShape(NeverNull<cpShape*> aShape) const {
    const auto iter = _shapeToPosition.find(aShape);
    if (iter == _shapeToPosition.end()) {
        return {};
    }
    const auto pos = iter->second;
    return _cells.at(pos.y, pos.y);
}

void EnvironmentManager::_eventDraw1() {
    hg::gr::RectangleShape rect{
        {(float)CELL_RESOLUTION, (float)CELL_RESOLUTION}
    };
    rect.setFillColor(hg::gr::COLOR_DARK_GREY);
    rect.setOutlineColor(hg::gr::COLOR_BLACK);
    rect.setOutlineThickness(4.f);

    auto& canvas = ccomp<MWindow>().getCanvas();
    for (hg::PZInteger y = 0; y < _cells.getHeight(); y += 1) {
        for (hg::PZInteger x = 0; x < _cells.getWidth(); x += 1) {
            rect.setPosition(x * (float)CELL_RESOLUTION, y * (float)CELL_RESOLUTION);
            canvas.draw(rect);
        }
    }
}

void EnvironmentManager::onNetworkingEvent(const RN_Event& aEvent) {
    if (!ctx().isPrivileged()) {
        return;
    }

    aEvent.visit([this](const RN_Event::Connected& aEventData) {
        if (!aEventData.clientIndex.has_value()) {
            return;
        }

        for (hg::PZInteger y = 0; y < _cells.getHeight(); y += 1) {
            std::string str;
            str.resize(hg::pztos(_cells.getWidth()));

            std::size_t idx = 0;
            for (hg::PZInteger x = 0; x < _cells.getWidth(); x += 1) {
                str[idx] = static_cast<char>(_cells[y][x]);
                idx++;
            }

            Compose_SetTerrain(ccomp<MNetworking>().getNode(),
                                *aEventData.clientIndex,
                                static_cast<std::int32_t>(_cells.getWidth()),
                                static_cast<std::int32_t>(_cells.getHeight()),
                                static_cast<std::int32_t>(y),
                                str);
        }
    });
}
