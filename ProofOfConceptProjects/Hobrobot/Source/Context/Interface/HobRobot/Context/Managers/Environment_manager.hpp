#pragma once

#include <HobRobot/Context/Managers/Environment_manager_interface.hpp>

#include <Hobgoblin/Utility/Grids.hpp>

namespace hobrobot {

class EnvironmentManager
    : public EnvironmentManagerInterface
    , public spe::NonstateObject
    , private spe::NetworkingEventListener {
public:
    EnvironmentManager(QAO_RuntimeRef aRuntimeRef, int aExecutionPriority);

    ~EnvironmentManager() override;

    void setToHostMode() override;
    void setToClientMode() override;
    Mode getMode() const override;

    cpSpace* getPhysicsSpace() const override;

    void onNetworkingEvent(const RN_Event& aEvent) override;

private:
    Mode _mode = Mode::Uninitialized;
    cpSpace* _physicsSpace = nullptr;

    hg::PZInteger _cellResolution = 0;
    struct Cell {
        hg::cpShapeUPtr shape;
        bool isEmpty = true;
    };
    hg::util::RowMajorGrid<Cell> _grid;

    void _eventPostUpdate() override;
    void _eventDraw1() override;

    void _generateTerrain(hg::PZInteger aWidth, hg::PZInteger aHeight, hg::PZInteger aCellResolution);
    void _setCellToEmpty(hg::PZInteger aX, hg::PZInteger aY);
    void _setCellToWall(hg::PZInteger aX, hg::PZInteger aY);

    friend void EnvironmentManager_SetTerrain(EnvironmentManager& aEnvMgr,
                                              hg::PZInteger aWidth,
                                              hg::PZInteger aHeight,
                                              hg::PZInteger aCellResolution,
                                              hg::util::Packet& aTerrainData);
};

} // namespace hobrobot
