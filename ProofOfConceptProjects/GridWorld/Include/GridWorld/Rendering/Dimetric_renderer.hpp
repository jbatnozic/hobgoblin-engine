#pragma once

#include <Hobgoblin/Graphics/Sprite.hpp>
#include <Hobgoblin/Graphics/Sprite_loader.hpp>
#include <Hobgoblin/Graphics/View.hpp>

#include <GridWorld/Positions.hpp>
#include <GridWorld/Renderer.hpp>
#include <GridWorld/Rendering/Lighting_renderer_2d.hpp>
#include <GridWorld/Rendering/Line_of_sight_renderer_2d.hpp>
#include <GridWorld/World/World.hpp>

#include <set>
#include <unordered_map>

namespace gridworld {

namespace hg = jbatnozic::hobgoblin;

enum RenderOptions {
    RENDOPT_NONE       = 0x00,
    RENDOPT_LOWER_MORE = 0x01,
};

class DimetricRenderer /*: public Renderer*/ {
public:
    DimetricRenderer(const World&                aWorld,
                     const hg::gr::SpriteLoader& aSpriteLoader,
                     LightingRenderer2D&         aLightingRenderer,
                     LineOfSightRenderer2D&      aLineOfSightRenderer);

    void start(const hg::gr::View& aView, WorldPosition aPointOfView);

    void render(hg::gr::Canvas& aCanvas, int aRenderOptions = RENDOPT_NONE);

private:
    // ===== Injected dependencies =====

    const World&                _world;
    const hg::gr::SpriteLoader& _spriteLoader;
    LightingRenderer2D&         _lightingRenderer;
    LineOfSightRenderer2D&      _losRenderer;

    // ===== Sprite cache =====

    mutable std::unordered_map<SpriteId, hg::gr::Sprite> _spriteCache;

    // ===== View data =====

    struct ViewData {
        hg::math::Vector2f center; //!< In the screen's coordinate system
        hg::math::Vector2f size;

        WorldPosition topLeft;
        // WorldPosition bottomRight;

        WorldPosition pointOfView;
    };

    ViewData _viewData;

    // ===== Rendered objects =====

    struct RenderedObjectPtrLess {
        bool operator()(const RenderedObject* aLhs, const RenderedObject* aRhs) const;
    };

    std::set<const RenderedObject*, RenderedObjectPtrLess> _objectsToRender;

    class CellToRenderedObjectAdapter : public RenderedObject {
    public:
        CellToRenderedObjectAdapter(const CellModel& aCell, const SpatialInfo& aSpatialInfo);

        void draw(hg::gr::Canvas& aCanvas, hg::math::Vector2f aScreenPosition) const override;

    private:
        const CellModel* _cell = nullptr;
        // Render parameters: drawmode, color, etc.
    };

    std::vector<CellToRenderedObjectAdapter> _cellAdapters;

    // ===== Utility =====

    struct CellInfo {
        hg::NeverNull<const CellModel*> cell;
        hg::PZInteger                   gridX;
        hg::PZInteger                   gridY;
    };

    // ===== Private methods =====

    hg::gr::Sprite& _getSprite(SpriteId aSpriteId) const;

    template <class taCallable>
    static void _diagonalTraverse(const World& aWorld, const ViewData& aViewData, taCallable&& aFunc);

    void _renderFloor(hg::gr::Canvas& aCanvas) const;
    void _renderLighting(hg::gr::Canvas& aCanvas) const;

    //! A cell is obstructed if it contains a wall or line of sight to
    //! all of its parts is blocked by other cells.
    bool _isCellObstructed(const CellInfo& aCellInfo) const;

    void _prepareWallsForRendering() const;
    void _renderWalls(hg::gr::Canvas& aCanvas, int aRenderOptions) const; // TODO(temporary)
    void _renderObjects(hg::gr::Canvas& aCanvas) const;
};

} // namespace gridworld
