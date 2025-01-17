// Copyright 2024 Jovan Batnozic. Released under MS-PL licence in Serbia.
// See https://github.com/jbatnozic/Hobgoblin?tab=readme-ov-file#licence

#pragma once

#include <GridGoblin/Rendering/Rendered_object.hpp>
#include <GridGoblin/Spatial/Position_in_view.hpp>
#include <GridGoblin/Spatial/Position_in_world.hpp>
#include <GridGoblin/World/World.hpp>

#include <GridGoblin/Rendering/Visibility_calculator.hpp>

#include <Hobgoblin/Graphics.hpp>

#include <vector>

namespace jbatnozic {
namespace gridgoblin {

class DimetricRenderer {
public:
    DimetricRenderer(const World& aWorld, const hg::gr::SpriteLoader& aSpriteLoader);

    struct OverdrawAmounts {
        float top    = 0.f;
        float bottom = 0.f;
        float left   = 0.f;
        float right  = 0.f;
    };

    void prepareToRenderStart(const hg::gr::View&         aView,
                              PositionInWorld             aPointOfView,
                              const OverdrawAmounts&      aOverdrawAmounts,
                              const VisibilityCalculator& aVisCals);

    void prepareToRenderEnd();

    void render(hg::gr::Canvas& aCanvas);

private:
    // ===== Dependencies =====

    const World&                _world;
    const hg::gr::SpriteLoader& _spriteLoader;

    // =====

    std::int64_t _renderCycleCounter = 0;

    // ===== View data =====

    struct ViewData {
        PositionInView     center;
        hg::math::Vector2f size;
        OverdrawAmounts    overdraw;

        PositionInWorld topLeft;
    };

    ViewData _viewData;

    // ===== Cell info =====

    struct CellInfo {
        const CellModel* cell;
        hg::PZInteger    gridX;
        hg::PZInteger    gridY;
    };

    // ===== Cell adapters =====

    class CellToRenderedObjectAdapter : public RenderedObject {
    public:
        CellToRenderedObjectAdapter(DimetricRenderer&  aRenderer,
                                    const CellModel&   aCell,
                                    const SpatialInfo& aSpatialInfo,
                                    int                aState);

        void render(hg::gr::Canvas& aCanvas, PositionInView aPosInView) const override;

    private:
        DimetricRenderer& _renderer;
        const CellModel&  _cell;

        int _state;
        // TODO: Render parameters: drawmode, color, etc.
    };

    friend class CellToRenderedObjectAdapter;

    std::vector<CellToRenderedObjectAdapter> _cellAdapters;

    // ===== Rendered objects =====

    std::vector<const RenderedObject*> _objectsToRender;

    // ===== Sprite cache =====

    mutable std::unordered_map<SpriteId, hg::gr::Sprite> _spriteCache;

    // ===== Methods =====

    hg::gr::Sprite& _getSprite(SpriteId aSpriteId) const;

    template <class taCallable>
    void _diagonalTraverse(const World& aWorld, const ViewData& aViewData, taCallable&& aFunc);

    void _reduceCellsBelowIfCellIsVisible(hg::math::Vector2pz         aCell,
                                          PositionInView              aCellPosInView,
                                          PositionInWorld             aPointOfView,
                                          const VisibilityCalculator& aVisCalc);

    void _prepareCells(bool                        aPredicate,
                       bool                        aVisibility,
                       PositionInWorld             aPointOfView,
                       const VisibilityCalculator* aVisCalc);
};

} // namespace gridgoblin
} // namespace jbatnozic
