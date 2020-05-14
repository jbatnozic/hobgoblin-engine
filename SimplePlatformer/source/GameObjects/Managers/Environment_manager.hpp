#ifndef ENVIRONMENT_MANAGER_HPP
#define ENVIRONMENT_MANAGER_HPP

#include <Hobgoblin/ChipmunkPhysics.hpp>
#include <Hobgoblin/Graphics.hpp>
#include <Hobgoblin/Utility/Grids.hpp>
#include <Hobgoblin/Utility/Packet.hpp>
#include <Hobgoblin/Utility/Rectangle.hpp>

#include <Hobgoblin/RigelNet_Macros.hpp> // TODO Temp.

#include <unordered_map>

#include "Experimental/Lighting.hpp"
#include "GameObjects/Framework/Game_object_framework.hpp"
#include "Terrain/Terrain.hpp"
#include "GameObjects/Gameplay/Collisions.hpp"

class EnvironmentManager : public GOF_SynchronizedObject, private Collideables::ITerrain {
public:
    EnvironmentManager(QAO_RuntimeRef rtRef, GOF_SynchronizedObjectRegistry& syncObjReg, GOF_SyncId syncId);

    ~EnvironmentManager();

    void generate(hg::PZInteger width, hg::PZInteger height, float cellResolution);
    void destroy();
    void setCellType(hg::PZInteger x, hg::PZInteger y, Terrain::TypeId typeId);

    hg::PZInteger getTerrainRowCount() const;
    hg::PZInteger getTerrainColumnCount() const;

    // Light:
    LightingController::LightHandle addLight(float x, float y, LightingController::Color color, float radius);

protected:
    void syncCreateImpl(RN_Node& node, const std::vector<hg::PZInteger>& rec) const override;
    void syncUpdateImpl(RN_Node& node, const std::vector<hg::PZInteger>& rec) const override;
    void syncDestroyImpl(RN_Node& node, const std::vector<hg::PZInteger>& rec) const override;

    void eventPostUpdate() override;
    void eventDraw1() override;

private:
    hg::util::RowMajorGrid<Terrain::TypeId> _typeIdGrid;
    hg::util::RowMajorGrid<hg::cpShapeUPtr> _shapeGrid;
    LightingController _lightingCtrl;
    float _cellResolution = 32.f; // TODO

    std::unordered_map<SpriteId, hg::gr::Multisprite> _spriteCache;

    void _resizeAllGrids(hg::PZInteger width, hg::PZInteger height);
    void _drawCell(hg::PZInteger x, hg::PZInteger y);

    friend RN_HANDLER_SIGNATURE(ResizeTerrain, RN_ARGS(std::int32_t, width, std::int32_t, height));
    friend RN_HANDLER_SIGNATURE(SetTerrainRow, RN_ARGS(std::int32_t, rowIndex, hg::util::Packet&, packet));
};

#endif // !ENVIRONMENT_MANAGER_HPP