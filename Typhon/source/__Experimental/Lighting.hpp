#pragma once

#include <Hobgoblin/Graphics.hpp>
#include <Hobgoblin/Utility/Grids.hpp>
#include <Hobgoblin/Utility/No_copy_no_move.hpp>
#include <Hobgoblin/Utility/Slab_indexer.hpp>

#include <Typhon/Framework.hpp>

#include <cstdint>
#include <vector>

class LightingController : public hg::util::NonCopyable, public hg::util::NonMoveable {
public:
    using Color = hg::gr::Color;
    using PZInteger = hg::PZInteger;

    class LightHandle : public hg::util::NonCopyable {
    public:
        LightHandle() = default;
        ~LightHandle();

        LightHandle(LightHandle&& other);
        LightHandle& operator=(LightHandle&& other);

        void setPosition(float x, float y) const;
        void setColor(Color color) const;
        void setRadius(float radius) const;

        void invalidate();
        bool isValid() const noexcept;

    private:
        LightingController* _lightingCtrl = nullptr;
        PZInteger _lightIndex = 0;

        LightHandle(LightingController& lightingCtrl, PZInteger lightIndex);
        friend class LightingController;
    };

    LightingController();

    LightingController(PZInteger width, PZInteger height, 
                       float cellResolution, Color ambientColor = Color::White);

    void render();
    void smooth();

    LightHandle addLight(float x, float y, Color color, float radius);

    hg::gr::Color getColorAt(PZInteger x, PZInteger y) const;

    void resize(PZInteger width, PZInteger height, float cellResolution);

    // TODO Replace with setCellOpacity(Color) which multiplies light coming through
    void setCellIsWall(PZInteger x, PZInteger y, bool isWall);

private:
    struct Cell {
        hg::gr::Color color;
        hg::gr::Color tmpColor;
        bool isWall;
    };

    class Light {
    public:
        Light() = default;

        Light(float x, float y, float radius, Color color);

        void render(const hg::util::RowMajorGrid<Cell>& world, float cellResolution);
        void integrate(hg::util::RowMajorGrid<Cell>& world, float cellResolution) const;

        void setPosition(float x, float y);
        void setColor(Color color);
        void setRadius(float radius);

    private:
        float _x;
        float _y;
        float _radius; // Radius is expressed in cells (not pixels)
        Color _color;

        hg::util::RowMajorGrid<std::uint8_t> _intensities;

        std::uint8_t _trace(const hg::util::RowMajorGrid<Cell>& world, float cellResolution, 
                            float lightX, float lightY, PZInteger cellX, PZInteger cellY) const;
    };

    hg::util::RowMajorGrid<Cell> _world;
    std::vector<Light> _lights;
    hg::util::SlabIndexer _lightIndexer;

    float _cellResolution;
    hg::gr::Color _ambientColor;

    void _smoothCell(PZInteger x, PZInteger y);

    void _setLightPosition(PZInteger lightIndex, float x, float y);
    void _setLightColor(PZInteger lightIndex, Color color);
    void _setLightRadius(PZInteger lightIndex, float radius);
    void _removeLight(PZInteger lightIndex);
    friend class LightHandle;
};
