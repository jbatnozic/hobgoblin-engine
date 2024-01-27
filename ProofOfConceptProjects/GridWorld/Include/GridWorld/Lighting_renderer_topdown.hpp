#pragma once

#include <Hobgoblin/Graphics/Render_texture.hpp>
#include <Hobgoblin/Math.hpp>
#include <Hobgoblin/Utility/Grids.hpp>

#include <GridWorld/Grid_world.hpp>

#include <cstdint>
#include <unordered_map>

namespace gridworld {

namespace hg = jbatnozic::hobgoblin;

class TopdownLightingRenderer {
public:
    TopdownLightingRenderer(const World& aWorld,
                            const hg::gr::SpriteLoader& aSpriteLoader);

    void start(const hg::gr::View& aView /*, pov*/);

    void render();

private:

};

} // namespace gridworld
