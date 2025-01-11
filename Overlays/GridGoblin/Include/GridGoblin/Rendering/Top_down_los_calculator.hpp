// Copyright 2024 Jovan Batnozic. Released under MS-PL licence in Serbia.
// See https://github.com/jbatnozic/Hobgoblin?tab=readme-ov-file#licence

#pragma once

#include <Hobgoblin/Math.hpp>
#include <Hobgoblin/Utility/Grids.hpp>

#include <GridGoblin/Spatial/Position_in_world.hpp>
#include <GridGoblin/World/World.hpp>

#include <array>
#include <vector>

namespace jbatnozic {
namespace gridgoblin {

namespace hg = jbatnozic::hobgoblin;

class TopDownLineOfSightCalculator {
public:
    enum Purpose {
        FOR_TOPDOWN,
        FOR_DIMETRIC
    };

    TopDownLineOfSightCalculator(const World& aWorld);

    void calc(PositionInWorld    aViewCenter,
              hg::math::Vector2f aViewSize,
              PositionInWorld    aLineOfSightOrigin);

    std::optional<bool> testVisibilityAt(PositionInWorld aPos) const;

    std::size_t getTriangleCount() const {
        return _darkZones.size();
    }

    std::size_t getTriangleComparisons() const {
        return hg::pztos(_comparisons);
    }

private:
    const World& _world;

    PositionInWorld    _viewCenter;
    hg::math::Vector2f _viewSize;
    hg::math::Vector2f _lineOfSightOrigin;

    struct Triangle : public hg::math::TriangleF {
        Triangle(hg::math::Vector2f aA,
                 hg::math::Vector2f aB,
                 hg::math::Vector2f aC,
                 std::uint16_t      aFlags)
            : jbatnozic::hobgoblin::math::TriangleF{aA, aB, aC}
            , flags{aFlags}
            {}

        std::uint16_t flags;
    };

    std::vector<Triangle> _darkZones;

    std::array<float, 360> _rays; // TODO: variable number of rays
    float _rayRadius = 0.f;
    bool _raysDisabled = true;

    mutable hg::PZInteger _comparisons = 0;

    struct CalculationContext {
        PositionInWorld     lineOfSightOrigin;
        hg::math::Vector2pz lineOfSightOriginCell;

        // The three cell coordinates below define the rectangle in which we need to process cells
        hg::math::Vector2pz rectTopLeftCell;
        hg::math::Vector2pz rectBottomRightCell;

        float radius;

        hg::PZInteger cornerThreshold = 100;
    };

    //! \returns is whole ring obstructed?
    bool _processRing(hg::PZInteger aRingIndex, const CalculationContext& aCtx);

    hg::PZInteger _processCell(hg::math::Vector2pz aCell, const CalculationContext& aCtx);

    void _processRays(const CalculationContext& aCtx);

    bool _isPointVisible(PositionInWorld aPosInWorld, std::uint16_t aFlags) const;

    bool _isLineVisible(PositionInWorld aP1, PositionInWorld aP2, std::uint16_t aFlags, hg::PZInteger aLevels) const;
};

} // namespace gridgoblin
} // namespace jbatnozic
