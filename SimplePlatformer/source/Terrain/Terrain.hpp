#ifndef TERRAIN_HPP
#define TERRAIN_HPP

#include <cstdint>

#include <Graphics/Sprites.hpp>

enum class TerrainType : std::int16_t {
    None = -1,

    Blackness = 0,
    StoneFloor,
    StoneWall
};

struct TerrainCellProperties {

    std::int8_t collisionMask;
};

#endif // !TERRAIN_HPP
