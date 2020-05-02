#pragma once

#include <Hobgoblin/Graphics.hpp>
#include <Hobgoblin/Utility/Grids.hpp>
#include <Hobgoblin/Utility/Slab_indexer.hpp>

#include <cstdint>
#include <vector>

#include "GameObjects/Framework/Game_object_framework.hpp"

class LightingController {
public:
    using LightHandle = int;

    LightingController();

    LightingController(hg::PZInteger width, hg::PZInteger height, 
                       float cellResolution, hg::gr::Color ambientColor = hg::gr::Color::White);

    void render();

    LightHandle addLight(float x, float y, hg::gr::Color color, float radius);
    void moveLight(LightHandle handle, float x, float y); // TODO Do this through handle

    hg::gr::Color getColorAt(hg::PZInteger x, hg::PZInteger y) const;

    void resize(hg::PZInteger width, hg::PZInteger height, float cellResolution);

    void setCellIsWall(hg::PZInteger x, hg::PZInteger y, bool isWall);

private:
    struct Cell {
        hg::gr::Color color;
        bool isWall;
    };

    class Light {
    public:
        Light() = default;

        Light(float x, float y, float radius, hg::gr::Color color);

        void render(const hg::util::RowMajorGrid<Cell>& world, float cellResolution);
        void integrate(hg::util::RowMajorGrid<Cell>& world, float cellResolution) const;

        void setPosition(float x, float y);

    private:
        float _x;
        float _y;
        float _radius; // Radius is expressed in cells (not pixels)
        hg::gr::Color _color;

        hg::util::RowMajorGrid<std::uint8_t> _intensities;

        std::uint8_t _trace(const hg::util::RowMajorGrid<Cell>& world, float cellResolution, 
                            float lightX, float lightY, hg::PZInteger cellX, hg::PZInteger cellY) const;
    };

    hg::util::RowMajorGrid<Cell> _world;
    std::vector<Light> _lights;
    hg::util::SlabIndexer _lightIndexer;

    float _cellResolution;
    hg::gr::Color _ambientColor;
};
