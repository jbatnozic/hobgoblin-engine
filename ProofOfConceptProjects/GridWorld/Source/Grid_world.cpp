
#include <GridWorld/Grid_world.hpp>

#include <Hobgoblin/HGExcept.hpp>

#include <cmath>

namespace gridworld {

float World::getCellResolution() const {
    return _cellResolution;
}

hg::PZInteger World::getCellCountX() const {
    return _grid.getWidth();
}

hg::PZInteger World::getCellCountY() const {
    return _grid.getHeight();
}

///////////////////////////////////////////////////////////////////////////
// CELL GETTERS                                                          //
///////////////////////////////////////////////////////////////////////////

Cell& World::getCellAt(hg::PZInteger aX, hg::PZInteger aY) {
    return _grid.at(aY, aX);
}

Cell& World::getCellAt(hg::math::Vector2pz aPos) {
    return _grid.at(aPos.y, aPos.x);
}

Cell& World::getCellAtUnchecked(hg::PZInteger aX, hg::PZInteger aY) {
    return _grid[aY][aX];
}

Cell& World::getCellAtUnchecked(hg::math::Vector2pz aPos) {
    return _grid[aPos.y][aPos.x];
}

const Cell& World::getCellAt(hg::PZInteger aX, hg::PZInteger aY) const {
    return _grid.at(aY, aX);
}

const Cell& World::getCellAt(hg::math::Vector2pz aPos) const {
    return _grid.at(aPos.y, aPos.x);
}

const Cell& World::getCellAtUnchecked(hg::PZInteger aX, hg::PZInteger aY) const {
    return _grid[aY][aX];
}

const Cell& World::getCellAtUnchecked(hg::math::Vector2pz aPos) const {
    return _grid[aPos.y][aPos.x];
}

///////////////////////////////////////////////////////////////////////////
// LIGHTS                                                                //
///////////////////////////////////////////////////////////////////////////

int World::createLight(SpriteId aSpriteId, hg::math::Vector2pz aSize) {
    const int id = (_lightIdCounter + 1);
    _lightIdCounter += 1;

    const auto pair = _lights.emplace(std::piecewise_construct,
                                      std::forward_as_tuple(id),
                                      std::forward_as_tuple());
    auto& light = pair.first->second;
    light.spriteId = aSpriteId;
    light.texture.create(aSize);

    return id;
}

void World::updateLight(int aLightHandle, hg::math::Vector2f aPosition, hg::math::AngleF aAngle) {
    const auto iter = _lights.find(aLightHandle);
    HG_HARD_ASSERT(iter != _lights.end());

    auto& light = iter->second;
    light.angle = aAngle;
    light.position = aPosition;
}

void World::destroyLight(int aLightHandle) {
    const auto iter = _lights.find(aLightHandle);
    HG_HARD_ASSERT(iter != _lights.end());

    _lights.erase(iter);
}

void World::_renderLight(LightData& aLightData) {
    const auto size = aLightData.texture.getSize();

    const auto startGridX = 
        hg::math::Clamp(static_cast<int>(trunc((aLightData.position.x - size.x / 2.f) / _cellResolution)), 
                        0, 
                        getCellCountX() - 1
        );
    const auto startGridY =
        hg::math::Clamp(static_cast<int>(trunc((aLightData.position.y - size.y / 2.f) / _cellResolution)),
                        0,
                        getCellCountY() - 1
        );
    const auto endGridX = 
        hg::math::Clamp(static_cast<int>(trunc((aLightData.position.x + size.x / 2.f) / _cellResolution)), 
                        0, 
                        getCellCountX() - 1
        );
    const auto endGridY =
        hg::math::Clamp(static_cast<int>(trunc((aLightData.position.y + size.y / 2.f) / _cellResolution)),
                        0,
                        getCellCountY() - 1
        );

    aLightData.texture.clear(hg::gr::COLOR_WHEAT); // TODO(temp)
    auto& view = aLightData.texture.getView();
    view.setSize({(float)size.x, (float)size.y}); // TODO: call only once on creation
    view.setCenter(aLightData.position);
    view.setViewport({0.f, 0.f, 1.f, 1.f});

    hg::gr::Vertex vertices[10];
    for (auto& vertex : vertices) {
        vertex.color = hg::gr::COLOR_BLACK;
    }

    for (hg::PZInteger y = startGridY; y <= endGridY; y += 1) {
        for (hg::PZInteger x = startGridX; x <= endGridX; x += 1) {
            // ****************************************************
            if (auto wall = getCellAtUnchecked(x, y).wall) {
                vertices[0].position = { x      * _cellResolution,  y      * _cellResolution};
                vertices[2].position = { x      * _cellResolution, (y + 1) * _cellResolution};
                vertices[4].position = {(x + 1) * _cellResolution, (y + 1) * _cellResolution};
                vertices[6].position = {(x + 1) * _cellResolution,  y      * _cellResolution};
                vertices[8] = vertices[0];

                for (int i = 1; i < 10; i += 2) {
                    hg::math::Vector2f diff = {vertices[i - 1].position.x - aLightData.position.x,
                                               vertices[i - 1].position.y - aLightData.position.y};
                    diff.x *= 1000.f;
                    diff.y *= 1000.f;

                    vertices[i].position = {vertices[i - 1].position.x + diff.x,
                                            vertices[i - 1].position.y + diff.y};
                }

                aLightData.texture.draw(vertices, 10, hg::gr::PrimitiveType::TriangleStrip);
            }
            // ****************************************************
        }
    }

    aLightData.texture.display();
}

hg::gr::RenderTexture* World::_renderLight(int aLightHandle) {
    const auto iter = _lights.find(aLightHandle);
    HG_HARD_ASSERT(iter != _lights.end());

    _renderLight(iter->second);

    return &(iter->second.texture);
}

} // namespace gridworld
