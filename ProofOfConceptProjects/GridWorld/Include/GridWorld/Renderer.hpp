#pragma once

#include <GridWorld/Spatial_info.hpp>

#include <Hobgoblin/Math.hpp>
#include <Hobgoblin/Graphics.hpp>

#include <functional>

namespace gridworld {

namespace hg = jbatnozic::hobgoblin;

class RenderedObject {
public:
    RenderedObject() = default;

    RenderedObject(const SpatialInfo& aSpatialInfo)
        : _spatialInfo{aSpatialInfo}
    {
    }

    const SpatialInfo& getSpatialInfo() const {
        return _spatialInfo;
    }

    //! position = screen position
    virtual void draw(hg::gr::RenderTarget& aRenderTarget, hg::math::Vector2<float> aPosition) const = 0;

protected:
    SpatialInfo _spatialInfo;
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
