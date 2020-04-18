#ifndef TERRAIN_HPP
#define TERRAIN_HPP

#include <Hobgoblin/Graphics.hpp>
#include <Hobgoblin/Utility/Grids.hpp>

struct TerrainCell {
    sf::Color color;
};

class Terrain {
public:

private:
    hg::util::RowMajorGrid<TerrainCell> _grid;
};

#endif // !TERRAIN_HPP