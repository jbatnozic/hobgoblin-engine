// Copyright 2024 Jovan Batnozic. Released under MS-PL licence in Serbia.
// See https://github.com/jbatnozic/Hobgoblin?tab=readme-ov-file#licence

#pragma once

#include <Hobgoblin/Math.hpp>
#include <Hobgoblin/Utility/Grids.hpp>

#include <GridGoblin/Spatial/Position_in_world.hpp>
#include <GridGoblin/World/World.hpp>

#include <vector>

namespace jbatnozic {
namespace gridgoblin {

namespace hg = jbatnozic::hobgoblin;

class TopDownLineOfSightCalculator {
public:
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

    std::size_t getPreciseRings() const {
        return hg::pztos(_preciseRings);
    }

private:
    // ===== Dependencies =====

    const World& _world;

    // ===== Configuration =====

    const float _cr;     //!< Cell resolution (known from _world).
    const float _xLimit; //!< Maximum for X values (known from _world).
    const float _yLimit; //!< Maximum for Y values (known from _world).

    hg::PZInteger _minRingsBeforeRaycasting     = 15;
    hg::PZInteger _minTrianglesBeforeRaycasting = 100;
    hg::PZInteger _rayCount                     = 360;
    hg::PZInteger _rayPointsPerCellResolution   = 6;

    // ===== Calculation context =====

    hg::math::Rectangle<float> _processedRingsBbox;

    hg::math::Rectangle<float> _viewBbox;
    hg::math::Vector2pz        _viewTopLeftCell;
    hg::math::Vector2pz        _viewBottomRightCell;

    hg::math::Vector2f  _lineOfSightOrigin;
    hg::math::Vector2pz _lineOfSightOriginCell;

    float         _triangleSideLength;
    float         _rayRadius;
    hg::PZInteger _maxPointsPerRay;

    // ===== Data structures =====

    struct Triangle : public hg::math::TriangleF {
        Triangle(hg::math::Vector2f aA,
                 hg::math::Vector2f aB,
                 hg::math::Vector2f aC,
                 std::uint16_t      aFlags)
            : jbatnozic::hobgoblin::math::TriangleF{aA, aB, aC}
            , flags{aFlags} {}

        std::uint16_t flags;
    };

    std::vector<Triangle> _darkZones;

    std::vector<float> _rays;

    bool _raysDisabled = true;

    // ===== Statistics =====

    mutable hg::PZInteger _comparisons  = 0;
    mutable hg::PZInteger _preciseRings = 0;

    // ===== Methods =====

    void _setInitialCalculationContext(PositionInWorld    aViewCenter,
                                       hg::math::Vector2f aViewSize,
                                       PositionInWorld    aLineOfSightOrigin);

    std::uint16_t _calcEdgesOfInterest(hg::math::Vector2pz aCell) const;

    bool _areAnyVerticesVisible(const std::array<hg::math::Vector2f, 8>& aVertices,
                                std::size_t                              aVertCount,
                                std::uint16_t                            aEdgesOfInterest) const;

    void _processRing(hg::PZInteger aRingIndex);

    void _processCell(hg::math::Vector2pz aCell, hg::PZInteger aRingIndex);

    void _setRaysFromTriangles(hg::math::AngleF aAngle1, hg::math::AngleF aAngle2);

    void _processRays();

    void _castRay(hg::PZInteger aRayIndex);

    bool _isPointVisible(PositionInWorld aPosInWorld, std::uint16_t aFlags) const;

    bool _isLineVisible(PositionInWorld aP1,
                        PositionInWorld aP2,
                        std::uint16_t   aFlags,
                        hg::PZInteger   aLevels) const;
};

} // namespace gridgoblin
} // namespace jbatnozic
