#pragma once

#include <Hobgoblin/Graphics/Color.hpp>
#include <Hobgoblin/Graphics/Render_texture.hpp>
#include <Hobgoblin/Graphics/Texture.hpp>
#include <Hobgoblin/Math.hpp>
#include <Hobgoblin/Utility/Grids.hpp>

#include <GridWorld/Model/Lights.hpp>
#include <GridWorld/World/World.hpp>

#include <cstdint>
#include <unordered_map>

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

    std::optional<hg::gr::Color> getColorAt() const;

    const hg::gr::Texture& getTexture(hg::math::Vector2f* aRecommendedScale = nullptr) const;

private:
    const World& _world;
    const hg::gr::SpriteLoader& _spriteLoader;

    float _sizeMultiplier;
    float _recommendedScale = 1.f;

    hg::gr::RenderTexture _renderTexture;
    hg::gr::Image _image;

    void _renderLight(const model::LightData& aLightData);
    void _drawLight(const model::LightData& aLightData);
};

} // namespace gridworld
