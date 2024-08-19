#include "Environment_manager.hpp"

#include <Hobgoblin/HGExcept.hpp>
#include <Hobgoblin/RigelNet_macros.hpp>
#include <Hobgoblin/Utility/Randomization.hpp>

#include "Collisions.hpp"

#include "Config.hpp"
#include "Loot.hpp"
#include "Resource_manager_interface.hpp"
#include "Sprite_manifest.hpp"
#include <array>
#include <deque>
#include <sstream>

void SetTerrainImpl(EnvironmentManager& aEnvMgr,
                    hg::PZInteger       aWidth,
                    hg::PZInteger       aHeight,
                    hg::PZInteger       aRowIdx,
                    const std::string&  aCellData);

namespace {
constexpr cpFloat CELL_RESOLUTION = single_terrain_size;

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

    if (ccomp<MResource>().getMode() == ResourceManagerInterface::Mode::CLIENT) {
        const auto& sprLoader = ccomp<MResource>().getSpriteLoader();
        _spr                  = sprLoader.getMultiBlueprint(SPR_MOUNTAIN).multispr();
    }
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

    std::deque<std::deque<CellKind>> _temp_cells;
    _temp_cells.push_back({});
    _temp_cells.push_back({});
    for (int j = 0; j < 4; j++) {
        _temp_cells[0].push_back(CellKind::SCALE);
        _temp_cells[1].push_back(CellKind::ROCK_1);
    }
    auto rand = []() {
        return hg::util::GetRandomNumber<std::int32_t>(0, 10000) * 0.0001;
    };

    for (int i = 1; i < mountain_height; i++) {
        auto chance1 = [i](bool inc=false) {
            float ch = ((terrain_1_chance +(inc ? terrain_1_repeat : 0)))*
                       (((float)(i * i) / (mountain_height * mountain_height)));

            return ch;
        };
        auto chance2 = [i](bool inc= false) {
            float ch = ((terrain_2_chance + (inc ? terrain_2_repeat : 0))) *
                       (1.f - ((float)(i * i) / (mountain_height * mountain_height)))
                       ;

            return ch;
        };
        auto _num       = rand();
        int  slope_left = 0;
        while (slope_chance < _num) {
            slope_left++;
            _num = rand();
        }
        left_offset += slope_left; 
        _num            = rand();
        int slope_right = 0;
        while (slope_chance < _num) {
            slope_right++;
            _num = rand();
        }
        right_offset += slope_right;

        _temp_cells.push_back({});
         for (int j = 0; j < _temp_cells[_temp_cells.size() - 2].size(); j++) {
            CellKind rock       = CellKind::ROCK_1;
            bool     inc        = false;
            if (j > 0 && _temp_cells[_temp_cells.size() - 2][j] == CellKind::ROCK_2) {
                inc = true;
            }

            if (chance1(inc) > rand()) {
                rock       = CellKind::ROCK_2;
            }
            inc = false;

            if (j > 0 && _temp_cells[_temp_cells.size() - 2][j] == CellKind::ROCK_3) {
                inc = true;
            }
            if (chance2(inc) > rand()) {
                rock       = CellKind::ROCK_3;
            }
            inc = false;
            _temp_cells[_temp_cells.size() - 1].push_back(rock);
        }
      

         for (int j = 0; j < _temp_cells.size(); j++) 
         {
            for (int l = 0; l < slope_left; l++) {
                CellKind rock = CellKind::ROCK_1;
                CellKind rock_slope = CellKind::ROCK_MT_1;
                bool inc = false;
                if (j > 0 && (_temp_cells[j - 1][1] == CellKind::ROCK_2)) {
                    inc = true;
                    if (chance1(inc) > rand()) {
                        rock       = CellKind::ROCK_2;
                        rock_slope = CellKind::ROCK_MT_2;
                    }
                    HG_LOG_FATAL(LOG_ID, "sosilica 57---------------------");
                    inc = false;

                }
                
                if (j > 0 && (_temp_cells[j - 1][1] == CellKind::ROCK_3)) {
                    inc = true;
                    if (chance2(inc) > rand()) {
                        rock       = CellKind::ROCK_3;
                        rock_slope = CellKind::ROCK_MT_3;
                        // HG_LOG_FATAL(LOG_ID, "SADA  2");
                    }
                    HG_LOG_FATAL(LOG_ID, "sosilica 5-5---------------------");
                    inc = false;
                }



                if (_temp_cells.size() - 1 >= 0 && j == _temp_cells.size() - 1) {
                    _temp_cells[j].push_front(rock);
                } else if (_temp_cells.size() - 2 >= 0 && j == _temp_cells.size() - 2) {
                    if (l == 0) {
                        _temp_cells[j].push_front(rock_slope);
                    } else {
                        _temp_cells[j].push_front(CellKind::EMPTY);
                    }

                } else {
                    _temp_cells[j].push_front(CellKind::EMPTY);
                }
            }
            for (int r = 0; r < slope_right; r++) {
                CellKind rock       = CellKind::ROCK_1;
                CellKind rock_slope = CellKind::ROCK_T_1;
                if (_temp_cells.size() - 1 >= 0 && j == _temp_cells.size() - 1) {
                    _temp_cells[j].push_back(CellKind::ROCK_1);
                } else if (_temp_cells.size() - 2 >= 0 && j == _temp_cells.size() - 2) {


                    bool     inc        = false;
                    if (j > 0 && _temp_cells[j - 1][_temp_cells[j - 1].size()-2] == CellKind::ROCK_2) {
                        inc = true;

                        if (chance1(inc) > rand()) {
                            rock       = CellKind::ROCK_2;
                            rock_slope = CellKind::ROCK_T_2;
                        }
                        HG_LOG_FATAL(LOG_ID, "sosilica 5----------------------");
                        inc = false;
                    }

                    if (j > 0 && _temp_cells[j - 1][_temp_cells[j - 1].size()-2] == CellKind::ROCK_3) {
                        inc = true;
                        if (chance2(inc) > rand()) {
                            rock       = CellKind::ROCK_3;
                            rock_slope = CellKind::ROCK_T_3;
                            // HG_LOG_FATAL(LOG_ID, "SADA  2");
                        }
                        HG_LOG_FATAL(LOG_ID, "sosilica 2----------------------");
                        inc = false;
                    }


                    if (r == 0) {
                        _temp_cells[j].push_back(rock_slope);
                    } else {
                        _temp_cells[j].push_back(rock);
                    }

                } else {
                    _temp_cells[j].push_back(CellKind::EMPTY);
                }
            }
        }
    

}



    aWidth  = static_cast<hg::PZInteger>(_temp_cells[0].size());
    aHeight = static_cast<hg::PZInteger>(_temp_cells.size());
    _cells.resize(aWidth, aHeight);

    for (hg::PZInteger y = 0; y < aHeight; y += 1) {
        std::ostringstream oss;
        for (hg::PZInteger x = 0; x < _temp_cells[y].size(); x += 1) {
            if (y == aHeight - 1 || y == aHeight - 2) {
                _cells[y][x] = CellKind::ROCK_1;
            } else {
                _cells[y][x] = _temp_cells[y][x];
            }
            oss << (int)_temp_cells[y][x];


        }
        HG_LOG_FATAL(LOG_ID, "{}", oss.str());
    }

    // Collision delegate
    _collisionDelegate.emplace(hg::alvin::CollisionDelegateBuilder{}
                                   .setDefaultDecision(hg::alvin::Decision::ACCEPT_COLLISION)
                                   .finalize());

    // Terrain body
    _terrainBody.emplace(hg::alvin::Body::createStatic());
    _space->add(*_terrainBody);

    // Shapes
    _shapes.resize(_temp_cells[0].size(), _temp_cells.size());
    for (hg::PZInteger y = 0; y < _temp_cells.size(); y += 1) {
        for (hg::PZInteger x = 0; x < _temp_cells[y].size(); x += 1) {
            if (_cells[y][x] != CellKind::EMPTY) {
                auto alvinShape = hg::alvin::Shape{CreateRectanglePolyShape(*_terrainBody, {x, y})};
                {
                    auto pair = _shapeToPosition.insert(
                        std::make_pair(static_cast<cpShape*>(alvinShape), hg::math::Vector2pz{x, y}));
                    HG_HARD_ASSERT(pair.second && "Insertion must happen!");
                }
                _shapes[y][x].emplace(std::move(alvinShape));
                _collisionDelegate->bind(*this, *_shapes[y][x]);
                _space->add(*_shapes[y][x]);
            }
        }
    }

    // Loot
    for (hg::PZInteger y = 0; y < aHeight; y += 1) {
        for (hg::PZInteger x = 0; x < aWidth; x += 1) {
            if (_cells[y][x] == CellKind::EMPTY) {
                continue;
            }
            if (hg::util::GetRandomNumber<std::int32_t>(0, 100) > 10) {
                continue;
            }
            auto* p = QAO_PCreate<LootObject>(ctx().getQAORuntime(),
                                              ccomp<MNetworking>().getRegistryId(),
                                              spe::SYNC_ID_NEW);
            p->init((float)CELL_RESOLUTION * (0.5f + x),
                    (float)CELL_RESOLUTION * (0.5f + y),
                    LootKind::PROTEIN);
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

hg::math::Vector2pz EnvironmentManager::getGridSize() const {
    return {_cells.getWidth(), _cells.getHeight()};
}

void EnvironmentManager::_eventUpdate1() {
    if (_space.has_value() && !ctx().getGameState().isPaused) {
        _space->step(1.0 / 60.0);
    }
}

void EnvironmentManager::_eventDraw1() {
    auto& winMgr = ccomp<MWindow>();
    auto& view   = winMgr.getView(0);
    auto& canvas = winMgr.getCanvas();
    canvas.clear(hg::gr::COLOR_SKY_BLUE);

    const hg::PZInteger startX = std::max(
        static_cast<int>((view.getCenter().x - view.getSize().x / 2.f) / (float)CELL_RESOLUTION - 1.f),
        0);
    const hg::PZInteger startY = std::max(
        static_cast<int>((view.getCenter().y - view.getSize().y / 2.f) / (float)CELL_RESOLUTION - 1.f),
        0);
    const hg::PZInteger endX = std::min(
        static_cast<int>((view.getCenter().x + view.getSize().x / 2.f) / (float)CELL_RESOLUTION + 1.f),
        _cells.getWidth());
    const hg::PZInteger endY = std::min(
        static_cast<int>((view.getCenter().y + view.getSize().y / 2.f) / (float)CELL_RESOLUTION + 1.f),
        _cells.getHeight());

    for (hg::PZInteger y = startY; y < endY; y += 1) {
        for (hg::PZInteger x = startX; x < endX; x += 1) {
            if (_cells[y][x] == CellKind::EMPTY) {
                continue;
            }
            int  offset      = 1;
            bool skipDrawing = false;
            switch (_cells[y][x]) {
            case CellKind::ROCK_1:
                _spr.selectSubsprite(0);
                break;
            case CellKind::ROCK_2:
                _spr.selectSubsprite(1);
                break;
            case CellKind::ROCK_3:
                _spr.selectSubsprite(2);
                break;
            case CellKind::ROCK_T_1:
                _spr.selectSubsprite(3);
                break;
            case CellKind::ROCK_T_2:
                _spr.selectSubsprite(4);
                break;
            case CellKind::ROCK_T_3:
                _spr.selectSubsprite(5);
                break;
            case CellKind::ROCK_MT_1:
                _spr.selectSubsprite(3);
                offset = -1;
                break;
            case CellKind::ROCK_MT_2:
                _spr.selectSubsprite(4);
                offset = -1;
                break;
            case CellKind::ROCK_MT_3:
                _spr.selectSubsprite(5);
                offset = -1;
                break;
            default:
                _spr.selectSubsprite(1);
                // skipDrawing = true;
                break;
            }

            if (!skipDrawing) {
                const auto& bounds = _spr.getLocalBounds();
                _spr.setOrigin(bounds.w / 2.f , bounds.h / 2.f);
                _spr.setPosition((float)CELL_RESOLUTION * (x + 0.5f),
                                 (float)CELL_RESOLUTION * (y + 0.5f));
                _spr.setScale(offset, 1);
                canvas.draw(_spr);
            }
        }
    }

    {
        hg::gr::RectangleShape rect{{2.f, view.getSize().y}};
        rect.setFillColor(hg::gr::COLOR_RED);
        rect.setPosition({
            0.f,
            view.getCenter().y - view.getSize().y / 2.f
        });
        canvas.draw(rect);
        rect.setPosition({
            getGridSize().x * (float)CELL_RESOLUTION,
            view.getCenter().y - view.getSize().y / 2.f
        });
        canvas.draw(rect);
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
