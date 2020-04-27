#ifndef TERRAIN_MANAGER_HPP
#define TERRAIN_MANAGER_HPP

#include <Hobgoblin/Graphics.hpp>
#include <Hobgoblin/Utility/Grids.hpp>

#include "Game_object_framework.hpp"

struct TerrainCell {
    sf::Color color;
};

class TerrainManager : public GOF_SynchronizedObject {
public:
    TerrainManager(QAO_RuntimeRef rtRef, SynchronizedObjectManager& syncObjMgr, SyncId syncId)
        : GOF_SynchronizedObject{rtRef, TYPEID_SELF, 0, "TerrainManager", syncObjMgr, syncId}
        , _grid{0, 0}
    {
    }

    ~TerrainManager();

    void generate(hg::PZInteger width, hg::PZInteger height, float cellResolution);

    virtual void syncCreateImpl(RN_Node& node, const std::vector<hg::PZInteger>& rec) const {}
    virtual void syncUpdateImpl(RN_Node& node, const std::vector<hg::PZInteger>& rec) const {}
    virtual void syncDestroyImpl(RN_Node& node, const std::vector<hg::PZInteger>& rec) const {}

    void eventDraw1() override;

private:
    hg::util::RowMajorGrid<TerrainCell> _grid;
    float _cellResolution;
};

#endif // !TERRAIN_MANAGER_HPP