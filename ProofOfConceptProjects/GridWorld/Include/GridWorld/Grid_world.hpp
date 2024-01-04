#pragma once

#include <Hobgoblin/Common.hpp>
#include <Hobgoblin/Graphics/Sprite_loader.hpp>
#include <Hobgoblin/Graphics/Render_texture.hpp>
#include <Hobgoblin/Math.hpp>
#include <Hobgoblin/Utility/Grids.hpp>

#include <optional>
#include <unordered_map>

namespace gridworld {

namespace hg = jbatnozic::hobgoblin;

using SpriteId = hg::gr::SpriteIdNumerical;

struct Cell {
    struct Floor {
        SpriteId spriteId;
    };

    struct Wall {
        SpriteId spriteId;
        SpriteId spriteId_lowered;
    };

    Floor floor;
    std::optional<Wall> wall;
};

class IsometricRenderer;

class World {
public:
    World(hg::PZInteger aWidth, hg::PZInteger aHeight, float aCellResolution)
        : _grid{aWidth, aHeight}
        , _cellResolution{aCellResolution}
    {
    }

    // Cell height (z)

    float getCellResolution() const;

    hg::PZInteger getCellCountX() const;

    hg::PZInteger getCellCountY() const;

    ///////////////////////////////////////////////////////////////////////////
    // CELL GETTERS                                                          //
    ///////////////////////////////////////////////////////////////////////////

    Cell& getCellAt(hg::PZInteger aX, hg::PZInteger aY);

    Cell& getCellAt(hg::math::Vector2pz aPos) ;

    Cell& getCellAtUnchecked(hg::PZInteger aX, hg::PZInteger aY);

    Cell& getCellAtUnchecked(hg::math::Vector2pz aPos);

    const Cell& getCellAt(hg::PZInteger aX, hg::PZInteger aY) const;

    const Cell& getCellAt(hg::math::Vector2pz aPos) const;

    const Cell& getCellAtUnchecked(hg::PZInteger aX, hg::PZInteger aY) const;

    const Cell& getCellAtUnchecked(hg::math::Vector2pz aPos) const;

    ///////////////////////////////////////////////////////////////////////////
    // LIGHTS                                                                //
    ///////////////////////////////////////////////////////////////////////////

    int createLight(SpriteId aSpriteId, hg::math::Vector2pz aSize);

    void updateLight(int aLightHandle, hg::math::Vector2f aPosition, hg::math::AngleF aAngle);

    void destroyLight(int aLightHandle);

    // TODO(createRAIILight())

private:
    // ===== Cells =====

    hg::util::RowMajorGrid<Cell> _grid;
    float _cellResolution;

    // ===== Lights =====

    struct LightData {
        SpriteId spriteId = 0;
        hg::math::AngleF angle = hg::math::AngleF::zero();
        hg::math::Vector2f position = {0.f, 0.f};

        hg::gr::RenderTexture texture;
    };
    std::unordered_map<int, LightData> _lights;
    int _lightIdCounter = 0;

    void _renderLight(LightData& aLightData);

public: // TODO(temp)
    hg::gr::RenderTexture* _renderLight(int aLightHandle);
};

} // namespace gridw
