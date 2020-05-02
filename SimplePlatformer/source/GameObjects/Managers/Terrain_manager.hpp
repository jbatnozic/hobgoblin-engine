#ifndef TERRAIN_MANAGER_HPP
#define TERRAIN_MANAGER_HPP

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

// Terrain cell: 64 bit
// [24: rgb][8: UNUSED][12: spriteId][4: UNUSED][8: subspriteIdx][8: collisionMask]
// 12 unused for TerrainTypeIndex

//struct TerrainCell {
//    sf::Color color;
//    
//    friend hg::util::PacketBase& operator<<(hg::util::PacketBase& packet, const TerrainCell& self) {
//        packet << self.color.toInteger();
//        return packet;
//    }
//
//    friend hg::util::PacketBase& operator>>(hg::util::PacketBase& packet, TerrainCell& self) {
//        sf::Uint32 rgba;
//        packet >> rgba;
//        self.color = sf::Color{rgba};
//        return packet;
//    }
//};

class TerrainManager : public GOF_SynchronizedObject {
public:
    TerrainManager(QAO_RuntimeRef rtRef, SynchronizedObjectManager& syncObjMgr, SyncId syncId);

    ~TerrainManager();

    void generate(hg::PZInteger width, hg::PZInteger height, float cellResolution);
    void destroy();
    void setCellType(hg::PZInteger x, hg::PZInteger y, Terrain::TypeId typeId);

    hg::PZInteger getRowCount() const;
    hg::PZInteger getColumnCount() const;

    //bool pointIntersectsTerrain(float x, float y) const;
    //bool rectIntersectsTerrain(const hg::util::Rectangle<float>& rect) const;

    virtual void syncCreateImpl(RN_Node& node, const std::vector<hg::PZInteger>& rec) const;
    virtual void syncUpdateImpl(RN_Node& node, const std::vector<hg::PZInteger>& rec) const;
    virtual void syncDestroyImpl(RN_Node& node, const std::vector<hg::PZInteger>& rec) const;

    void eventPostUpdate() override;
    void eventDraw1() override;

    friend RN_HANDLER_SIGNATURE(ResizeTerrain, RN_ARGS(std::int32_t, width, std::int32_t, height));
    friend RN_HANDLER_SIGNATURE(SetTerrainRow, RN_ARGS(std::int32_t, rowIndex, hg::util::Packet&, packet));

private:
    hg::util::RowMajorGrid<Terrain::TypeId> _typeIdGrid;
    hg::util::RowMajorGrid<hg::cpShapeUPtr> _shapeGrid;
    LightingController _lightingCtrl;
    float _cellResolution = 64.f; // TODO

    std::unordered_map<SpriteId, hg::gr::Multisprite> _spriteCache;

    void _resizeAllGrids(hg::PZInteger width, hg::PZInteger height);
    void _drawCell(hg::PZInteger x, hg::PZInteger y);
};

#endif // !TERRAIN_MANAGER_HPP