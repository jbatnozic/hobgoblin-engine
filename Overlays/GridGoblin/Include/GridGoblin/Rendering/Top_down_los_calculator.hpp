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
    // ===== Dependencies =====

    const World& _world;

    // ===== Configuration =====

    const float _cr;     //!< Cell resolution (known from _world).
    const float _xLimit; //!< Maximum for X values (known from _world).
    const float _yLimit; //!< Maximum for Y values (known from _world).

    hg::PZInteger _minRingsBeforeRaycasting;
    hg::PZInteger _rayCount;

    // ===== Calculation context =====

    hg::math::Rectangle<float> _viewBbox;
    hg::math::Vector2pz        _viewTopLeftCell;
    hg::math::Vector2pz        _viewBottomRightCell;

    hg::math::Vector2f  _lineOfSightOrigin;
    hg::math::Vector2pz _lineOfSightOriginCell;

    float _rayRadius;

    // ===== Data structures =====

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
    bool _raysDisabled = true;

    // ===== Statistics =====

    mutable hg::PZInteger _comparisons = 0;

    // ===== Methods =====

    std::uint16_t _calcEdgesOfInterest(hg::math::Vector2pz aCell) const;

    bool _areAnyVerticesVisible(const std::array<hg::math::Vector2f, 8>& aVertices,
                                std::size_t                              aVertCount,
                                std::uint16_t                            aEdgesOfInterest) const;

    void _processRing(hg::PZInteger aRingIndex);

    void _processCell(hg::math::Vector2pz aCell);

    void _processRays();

    bool _isPointVisible(PositionInWorld aPosInWorld, std::uint16_t aFlags) const;

    bool _isLineVisible(PositionInWorld aP1, PositionInWorld aP2, std::uint16_t aFlags, hg::PZInteger aLevels) const;
};

} // namespace gridgoblin
} // namespace jbatnozic
