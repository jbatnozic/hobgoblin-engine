#pragma once

#include <Hobgoblin/Graphics/Color.hpp>
#include <Hobgoblin/Graphics/Render_texture.hpp>
#include <Hobgoblin/Graphics/Texture.hpp>
#include <Hobgoblin/Math.hpp>
#include <Hobgoblin/Utility/Grids.hpp>

#include <GridWorld/Model/Lights.hpp>
#include <GridWorld/World/World.hpp>

#include <array>
#include <cstdint>
#include <unordered_map>
#include <vector>

namespace gridworld {

namespace hg = jbatnozic::hobgoblin;

class LightingRenderer2D {
public:
    enum Purpose {
        FOR_TOPDOWN,
        FOR_DIMETRIC
    };

    LightingRenderer2D(const World& aWorld,
                       const hg::gr::SpriteLoader& aSpriteLoader,
                       hg::PZInteger aTextureSize,
                       Purpose aPurpose);

    void start(hg::math::Vector2f aWorldPosition,
               hg::math::Vector2f aViewSize,
               float aPadding);

    void render();

    std::optional<hg::gr::Color> getColorAt(hg::math::Vector2f aPos) const;

    const hg::gr::Texture& getTexture(hg::math::Vector2f* aRecommendedScale = nullptr) const;

private:
    const World& _world;
    const hg::gr::SpriteLoader& _spriteLoader;

    mutable std::unordered_map<model::SpriteId, hg::gr::Sprite> _spriteCache;

    hg::PZInteger _textureSize;
    float _sizeMultiplier;
    float _recommendedScale = 1.f;

    hg::gr::RenderTexture _renderTexture;
    // Image which will hold the contents of _renderTexture in RAM,
    // for fast access for purposes of getColorAt().
    hg::gr::Image _image;
    std::vector<std::uint8_t> _imageData;

    std::array<unsigned int, 2> _framebuffers;
    unsigned int _pbo;

    int _step = 0;

    hg::gr::Sprite& _getSprite(model::SpriteId aSpriteId) const;

    void _renderLight(const model::LightData& aLightData);
    void _drawLight(const model::LightData& aLightData);
};

} // namespace gridworld
