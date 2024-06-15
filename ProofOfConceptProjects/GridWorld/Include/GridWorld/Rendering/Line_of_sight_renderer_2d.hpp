#pragma once

#include <Hobgoblin/Graphics/Color.hpp>
#include <Hobgoblin/Graphics/Render_texture.hpp>
#include <Hobgoblin/Graphics/Texture.hpp>
#include <Hobgoblin/Math.hpp>
#include <Hobgoblin/Utility/Grids.hpp>

#include <GridWorld/Model/Lights.hpp>
#include <GridWorld/Positions.hpp>
#include <GridWorld/World/World.hpp>

#include <array>
#include <cstdint>
#include <unordered_map>
#include <vector>

namespace gridworld {

namespace hg = jbatnozic::hobgoblin;

class LineOfSightRenderer2D {
public:
    enum Purpose {
        FOR_TOPDOWN,
        FOR_DIMETRIC
    };

    LineOfSightRenderer2D(const World& aWorld, hg::PZInteger aTextureSize, Purpose aPurpose);

    ~LineOfSightRenderer2D();

    void start(WorldPosition      aViewPosition,
               hg::math::Vector2f aViewSize,
               WorldPosition      aLineOfSightOrigin,
               float              aPadding);

    void render();

    std::optional<bool> testVisibilityAt(WorldPosition aPos) const;

    //! For debug purposes only.
    const hg::gr::Texture& __gwimpl_getTexture(hg::math::Vector2f* aRecommendedScale = nullptr) const;

private:
    const World& _world;

    float              _sizeMultiplier;
    float              _recommendedScale = 1.f;
    WorldPosition      _losOrigin;
    hg::math::Vector2f _viewCenterOffset;

    //! Texture to which visibility is rendered.
    hg::gr::RenderTexture _renderTexture;

    //! Width and Height of _renderTexture (it's always a square).
    hg::PZInteger _textureSize;

    //! Buffer which will hold the contents of _renderTexture in RAM
    //! (in RGBA format), for fast access for purposes of getColorAt().
    std::vector<std::uint8_t> _textureRamBuffer;

    //! Names of OpenGL Pixel Buffer Objects.
    std::array<unsigned int, 2> _pboNames;

    //! Counter used to know which PBO to write to and which PBO to read from.
    //! - In even-numbered steps, we start writing to pbo[0] and read from pbo[1],
    //! - In odd-numbered steps, we start writing to pbo[1] and read from pbo[0].
    unsigned int _stepCounter = -1;

    void _renderOcclusion();
};

} // namespace gridworld
