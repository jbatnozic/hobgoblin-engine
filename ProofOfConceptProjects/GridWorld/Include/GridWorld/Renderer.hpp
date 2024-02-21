#pragma once

#include <GridWorld/Spatial_info.hpp>

#include <Hobgoblin/Math.hpp>
#include <Hobgoblin/Graphics.hpp>

#include <functional>

namespace gridworld {

namespace hg = jbatnozic::hobgoblin;

struct RenderedObject {
    SpatialInfo spatialInfo;

    virtual void draw(hg::gr::RenderTarget& aRenderTarget, hg::math::Vector2<float> aPosition) = 0;
};

struct RendererConfiguration {
    float cellSize = 32.f;
};

// TODO(unused)
class Renderer {
public:

    virtual void startFrame(/*viewport, pov*/) = 0;

    virtual void addObject() = 0;

    virtual void render() = 0;
};

} // namespace gridw
