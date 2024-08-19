#pragma once

#include "Engine.hpp"

#include "Cell_kind.hpp"
#include "Collisions.hpp"
#include "Environment_manager_interface.hpp"

#include <Hobgoblin/Alvin.hpp>
#include <Hobgoblin/ChipmunkPhysics.hpp>
#include <Hobgoblin/Utility/Grids.hpp>

#include <memory>
#include <optional>
#include <unordered_map>

struct ShapeHasher {
    std::size_t operator()(const cpShape* aShape) const {
        return reinterpret_cast<std::size_t>(aShape);
    }
};

class EnvironmentManager
    : public EnvironmentManagerInterface
    , public spe::NonstateObject
    , public TerrainInterface
    , private spe::NetworkingEventListener {
public:
    EnvironmentManager(QAO_RuntimeRef aRuntimeRef, int aExecutionPriority);
    ~EnvironmentManager() override;

    void setToHeadlessHostMode() override;
    void setToClientMode() override;
    Mode getMode() const override;

    void generateTerrain(hg::PZInteger aWidth, hg::PZInteger aHeight);

    hg::alvin::Space& getSpace() override;

    std::optional<CellKind> getCellKindOfShape(NeverNull<cpShape*> aShape) const override;

    hg::math::Vector2pz getGridSize() const override;

private:
    Mode _mode = Mode::UNINITIALIZED;

    std::optional<hg::alvin::MainCollisionDispatcher> _collisionDispatcher;
    std::optional<hg::alvin::Space>                   _space;

    std::optional<hg::alvin::CollisionDelegate>             _collisionDelegate;
    std::optional<hg::alvin::Body>                          _terrainBody;
    hg::util::RowMajorGrid<CellKind>                        _cells;
    hg::util::RowMajorGrid<std::optional<hg::alvin::Shape>> _shapes;
    std::unordered_map<cpShape*, hg::math::Vector2pz>       _shapeToPosition;

    hg::gr::Multisprite _spr;

    void _eventUpdate1() override;
    void _eventDraw1() override;

    void onNetworkingEvent(const RN_Event& aEvent) override;

    friend void SetTerrainImpl(EnvironmentManager& aEnvMgr,
                               hg::PZInteger       aWidth,
                               hg::PZInteger       aHeight,
                               hg::PZInteger       aRowIdx,
                               const std::string&  aCellData);
};
