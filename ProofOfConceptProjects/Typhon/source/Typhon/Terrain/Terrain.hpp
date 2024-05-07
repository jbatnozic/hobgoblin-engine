// Copyright 2024 Jovan Batnozic. Released under MS-PL licence in Serbia.
// See https://github.com/jbatnozic/Hobgoblin?tab=readme-ov-file#licence

// clang-format off

#ifndef TYPHON_TERRAIN_TERRAIN_HPP
#define TYPHON_TERRAIN_TERRAIN_HPP

#include <cstdint>
#include <vector>

#include <Typhon/Graphics/Sprites.hpp>

class Terrain {
public:
    enum class TypeId : std::int16_t {
        None = -1,

        Blackness = 0,
        CaveFloor,
        CaveWall,
        EnergyShield,
        Space,

        Count // Keep last
    };

    enum class CellShape : std::int8_t {
        None = -1,

        Empty = 0,
        FullSquare,
    };

    struct TypeProperties {
        CellShape shape = CellShape::Empty;
        std::int8_t collisionMask = 0;

        SpriteId spriteId;
        // opacity info
    };

    void defineTerrainTypes() {
        typeProperties.resize(static_cast<std::size_t>(TypeId::Count));
        TypeProperties tp;

        // Blackness
        // TODO

        // Cave floor
        tp.shape = CellShape::Empty;
        tp.spriteId = SpriteId::CaveFloor;
        typeProperties[static_cast<std::size_t>(TypeId::CaveFloor)] = tp;

        // Cave wall
        tp.shape = CellShape::FullSquare;
        tp.spriteId = SpriteId::CaveWall;
        tp.collisionMask = 0x000F;
        typeProperties[static_cast<std::size_t>(TypeId::CaveWall)] = tp;

        // Energy shield
        tp.shape = CellShape::FullSquare;
        tp.spriteId = SpriteId::EnergyShield;
        tp.collisionMask = 0x000F;
        typeProperties[static_cast<std::size_t>(TypeId::EnergyShield)] = tp;

        // Space
        tp.shape = CellShape::Empty;
        tp.spriteId = SpriteId::Space;
        typeProperties[static_cast<std::size_t>(TypeId::Space)] = tp;
    }

    static const TypeProperties& getTypeProperties(TypeId typeId);
    static void initializeSingleton();

private:
    Terrain();

    static const Terrain& getInstance();

    std::vector<TypeProperties> typeProperties;
};

#endif // !TYPHON_TERRAIN_TERRAIN_HPP

// clang-format on
