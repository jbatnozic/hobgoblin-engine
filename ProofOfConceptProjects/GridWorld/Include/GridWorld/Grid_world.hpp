#pragma once

#include <Hobgoblin/Common.hpp>
#include <Hobgoblin/Graphics/Sprite_loader.hpp>
#include <Hobgoblin/Graphics/Render_texture.hpp>
#include <Hobgoblin/Math.hpp>
#include <Hobgoblin/Utility/Grids.hpp>

#include <GridWorld/Cell.hpp>
#include <GridWorld/Lighting.hpp>
#include <GridWorld/Sprites.hpp>

#include <optional>
#include <unordered_map>

namespace gridworld {
#define internal private

namespace hg = jbatnozic::hobgoblin;

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

    LightId createLight(SpriteId aSpriteId, hg::math::Vector2pz aSize);

    void updateLight(LightId aLightHandle, hg::math::Vector2f aPosition, hg::math::AngleF aAngle);

    void destroyLight(LightId aLightHandle);

    // TODO(createRAIILight())

internal:
    detail::LightDataConstIterator lightDataBegin() const {
        return _lights.cbegin();
    }

    detail::LightDataConstIterator lightDataEnd() const {
        return _lights.cend();
    }

private:
    // ===== Cells =====

    hg::util::RowMajorGrid<Cell> _grid;
    float _cellResolution;

    // ===== Lights =====

    detail::LightMap _lights;
    LightId _lightIdCounter = 0;

    void _renderLight(detail::LightData& aLightData);

public: // TODO(temp)
    hg::gr::RenderTexture* _renderLight(int aLightHandle);
};

#undef internal
} // namespace gridw
