// Copyright 2024 Jovan Batnozic. Released under MS-PL licence in Serbia.
// See https://github.com/jbatnozic/Hobgoblin?tab=readme-ov-file#licence

#include <GridGoblin/Rendering/Top_down_los_calculator.hpp>

#include <Hobgoblin/Common.hpp>
#include <Hobgoblin/Logging.hpp>

#include <array>
#include <cmath>
#include <type_traits>

namespace jbatnozic {
namespace gridgoblin {

namespace {
using CellFlags = std::underlying_type_t<CellModel::Flags>;

using hg::math::AngleF;
using hg::math::Clamp;
using hg::math::Sign;

std::size_t GetUnobstructedVertices(const CellModel&                   aCell,
                                    hg::math::Vector2pz                aCellCoords,
                                    CellFlags                          aEdgesOfInterest,
                                    float                              aCellResolution,
                                    std::array<hg::math::Vector2f, 8>& aVertices) {
    std::size_t cnt = 0;

    const auto flags = aCell.getFlags() | aEdgesOfInterest;

#define cr aCellResolution

    static constexpr float OFFSET = 0.025;

    if ((flags & CellModel::RIGHT_EDGE_OBSTRUCTED) == 0) {
        aVertices[cnt + 0] = {(aCellCoords.x + 1) * cr + OFFSET, (aCellCoords.y + 1) * cr + OFFSET};
        aVertices[cnt + 1] = {(aCellCoords.x + 1) * cr + OFFSET, (aCellCoords.y + 0) * cr - OFFSET};
        cnt += 2;
    }
    if ((flags & CellModel::TOP_EDGE_OBSTRUCTED) == 0) {
        aVertices[cnt + 0] = {(aCellCoords.x + 1) * cr + OFFSET, (aCellCoords.y + 0) * cr - OFFSET};
        aVertices[cnt + 1] = {(aCellCoords.x + 0) * cr - OFFSET, (aCellCoords.y + 0) * cr - OFFSET};
        cnt += 2;
    }
    if ((flags & CellModel::LEFT_EDGE_OBSTRUCTED) == 0) {
        aVertices[cnt + 0] = {(aCellCoords.x + 0) * cr - OFFSET, (aCellCoords.y + 0) * cr - OFFSET};
        aVertices[cnt + 1] = {(aCellCoords.x + 0) * cr - OFFSET, (aCellCoords.y + 1) * cr + OFFSET};
        cnt += 2;
    }
    if ((flags & CellModel::BOTTOM_EDGE_OBSTRUCTED) == 0) {
        aVertices[cnt + 0] = {(aCellCoords.x + 0) * cr - OFFSET, (aCellCoords.y + 1) * cr + OFFSET};
        aVertices[cnt + 1] = {(aCellCoords.x + 1) * cr + OFFSET, (aCellCoords.y + 1) * cr + OFFSET};
        cnt += 2;
    }

#undef cr

    return cnt;
}

void CastRay(/*  in */ hg::math::Vector2f  aOrigin,
             /*  in */ hg::math::Vector2f  aPassesThrough,
             /*  in */ float               aLength,
             /* out */ hg::math::Vector2f& aRayEnd,
             /* out */ hg::math::AngleF&   aRayAngle) {
    aPassesThrough -= aOrigin;
    aRayAngle = AngleF::fromVector(aPassesThrough.x, aPassesThrough.y);
    aRayEnd   = aOrigin + aRayAngle.asNormalizedVector() * aLength;
}

constexpr float FLOAT_EPSILON = (1.f / 256.f);
} // namespace

TopDownLineOfSightCalculator::TopDownLineOfSightCalculator(const World& aWorld)
    : _world{aWorld}
    , _cr{_world.getCellResolution()}
    , _xLimit{std::nextafter(_world.getCellCountX() * _cr, 0.f)}
    , _yLimit{std::nextafter(_world.getCellCountY() * _cr, 0.f)} //
{
    for (auto& r : _rays) {
        r = INFINITY;
    }
}

std::optional<bool> TopDownLineOfSightCalculator::testVisibilityAt(PositionInWorld aPos) const {
    // if (!point_inside_view) { return std:: nullopt; }
    return _isPointVisible(aPos, 0xFFFF);
}

void TopDownLineOfSightCalculator::calc(PositionInWorld    aViewCenter,
                                        hg::math::Vector2f aViewSize,
                                        PositionInWorld    aLineOfSightOrigin) {
    // top-left x, top-left y, width, height
    _viewBbox = {Clamp(aViewCenter->x - aViewSize.x * 0.5f, 0.f, _xLimit),
                 Clamp(aViewCenter->y - aViewSize.y * 0.5f, 0.f, _yLimit),
                 aViewSize.x,
                 aViewSize.y};

    if (_viewBbox.getRight() >= _xLimit) {
        _viewBbox.w = _xLimit - _viewBbox.x;
    }
    if (_viewBbox.getBottom() >= _yLimit) {
        _viewBbox.h = _yLimit - _viewBbox.y;
    }

    _viewTopLeftCell     = _world.posToCell(_viewBbox.getLeft(), _viewBbox.getTop());
    _viewBottomRightCell = _world.posToCell(_viewBbox.getRight(), _viewBbox.getBottom());

    _lineOfSightOrigin     = *aLineOfSightOrigin;
    _lineOfSightOriginCell = {hg::ToPz(_lineOfSightOrigin.x / _cr),
                              hg::ToPz(_lineOfSightOrigin.y / _cr)};

    const int maxRings = 15;

    _rayRadius = maxRings * _cr -
                 std::max(std::abs(_lineOfSightOrigin.x - (_lineOfSightOriginCell.x + 0.5f) * _cr),
                          std::abs(_lineOfSightOrigin.y - (_lineOfSightOriginCell.y + 0.5f) * _cr));

    const auto& center = _lineOfSightOriginCell;
    const auto& start  = _viewTopLeftCell;
    const auto& end    = _viewBottomRightCell;

    hg::PZInteger ring = 1;
    while ((center.x - ring >= start.x) || (center.x + ring <= end.x) || (center.y - ring >= start.y) ||
           (center.y + ring <= end.y)) {
        _processRing(ring);
        ring += 1;
        if (ring > maxRings) {
            break; // TODO: switch to rays after both maxRings and maxTriangles have been exceeded
                   //       ...maybe also a threshold for undetermined rays?
        }
    }

    if (ring > maxRings) {
        _processRays();
        _raysDisabled = false;
    }
}

// MARK: Private

std::uint16_t TopDownLineOfSightCalculator::_calcEdgesOfInterest(hg::math::Vector2pz aCell) const {
    static constexpr CellFlags ALL_EDGES =
        CellModel::RIGHT_EDGE_OBSTRUCTED | CellModel::TOP_EDGE_OBSTRUCTED |
        CellModel::LEFT_EDGE_OBSTRUCTED | CellModel::BOTTOM_EDGE_OBSTRUCTED;

    CellFlags edgesOfInterest = ALL_EDGES;

    switch (Sign(aCell.x - _lineOfSightOriginCell.x)) {
    case -1:
        edgesOfInterest ^= CellModel::RIGHT_EDGE_OBSTRUCTED;
        break;
    case +1:
        edgesOfInterest ^= CellModel::LEFT_EDGE_OBSTRUCTED;
        break;
    }
    switch (Sign(aCell.y - _lineOfSightOriginCell.y)) {
    case -1:
        edgesOfInterest ^= CellModel::BOTTOM_EDGE_OBSTRUCTED;
        break;
    case 1:
        edgesOfInterest ^= CellModel::TOP_EDGE_OBSTRUCTED;
        break;
    }

    return edgesOfInterest;
}

bool TopDownLineOfSightCalculator::_areAnyVerticesVisible(
    const std::array<hg::math::Vector2f, 8>& aVertices,
    std::size_t                              aVertCount,
    std::uint16_t                            aEdgesOfInterest) const //
{
    for (std::size_t i = 0; i < aVertCount; i += 1) {
        if (i > 0 && aVertices[i] == aVertices[i - 1]) {
            continue;
        }
        if (_isPointVisible(PositionInWorld{aVertices[i]}, aEdgesOfInterest)) {
            return true;
        }
        if (i % 2 == 1 && _isLineVisible(PositionInWorld{aVertices[i]},
                                         PositionInWorld{aVertices[i - 1]},
                                         aEdgesOfInterest,
                                         2)) { // TODO: 2 = magic constant
            return true;
        }
    }

    return false;
}

void TopDownLineOfSightCalculator::_processRing(hg::PZInteger aRingIndex) {
    if (aRingIndex == 0) {
        return; // TODO: special treatment?
    }

    const auto xStart = _lineOfSightOriginCell.x - aRingIndex;
    const auto xEnd   = _lineOfSightOriginCell.x + aRingIndex;

#define CELL_Y_IS_IN_BOUNDS(_y_) ((_y_) >= _viewTopLeftCell.y && (_y_) <= _viewBottomRightCell.y)
#define CELL_X_IS_IN_BOUNDS(_x_) ((_x_) >= _viewTopLeftCell.x && (_x_) <= _viewBottomRightCell.x)

    // Top row
    {
        const auto y = _lineOfSightOriginCell.y - aRingIndex;
        if (CELL_Y_IS_IN_BOUNDS(y)) {
            for (hg::PZInteger x = xStart; x <= xEnd; x += 1) {
                if (CELL_X_IS_IN_BOUNDS(x)) {
                    _processCell({x, y});
                }
            }
        }
    }

    // Middle rows
    for (hg::PZInteger y = _lineOfSightOriginCell.y - aRingIndex + 1;
         y <= _lineOfSightOriginCell.y + aRingIndex - 1;
         y += 1) {
        if (CELL_Y_IS_IN_BOUNDS(y)) {
            if (CELL_X_IS_IN_BOUNDS(xStart)) {
                _processCell({xStart, y});
            }
            if (CELL_X_IS_IN_BOUNDS(xEnd)) {
                _processCell({xEnd, y});
            }
        }
    }

    // Bottom row
    {
        const auto y = _lineOfSightOriginCell.y + aRingIndex;
        if (CELL_Y_IS_IN_BOUNDS(y)) {
            for (hg::PZInteger x = xStart; x <= xEnd; x += 1) {
                if (CELL_X_IS_IN_BOUNDS(x)) {
                    _processCell({x, y});
                }
            }
        }
    }

#undef X_IS_IN_BOUNDS
#undef Y_IS_IN_BOUNDS
}

void TopDownLineOfSightCalculator::_processCell(hg::math::Vector2pz aCell) {
    // if (std::abs((int)aCell.x - (int)aCell.y) > aCtx.cornerThreshold) {
    //     return 1;
    // }

    const auto* cell = _world.getCellAtUnchecked(aCell);
    if (HG_UNLIKELY_CONDITION(cell == nullptr)) {
        HG_UNLIKELY_BRANCH;
        return;
    }

    if (!cell->isWallInitialized()) {
        return;
    }

    const auto edgesOfInterest = _calcEdgesOfInterest(aCell);

    std::array<hg::math::Vector2f, 8> vertices;
    const auto vertCnt = GetUnobstructedVertices(*cell, aCell, edgesOfInterest, _cr, vertices);

    if (vertCnt == 0) {
        return;
    }

    if (!_areAnyVerticesVisible(vertices, vertCnt, edgesOfInterest)) {
        return;
    }

    for (std::size_t i = 0; i < vertCnt; i += 2) {
        hg::math::Vector2f ray1End, ray2End;
        AngleF             a1, a2;

        CastRay(_lineOfSightOrigin, vertices[i + 0], _rayRadius * 2.f, ray1End, a1);
        CastRay(_lineOfSightOrigin, vertices[i + 1], _rayRadius * 2.f, ray2End, a2);

        _darkZones.push_back({vertices[i], vertices[i + 1], ray1End, edgesOfInterest});
        _darkZones.push_back({ray1End, vertices[i + 1], ray2End, edgesOfInterest});

        if (a2 < a1) {
            std::swap(a1, a2);
        }
        if (HG_UNLIKELY_CONDITION(a1 <= AngleF::halfCircle() * 0.5 &&
                                  a2 >= AngleF::halfCircle() * 1.5)) {
            HG_UNLIKELY_BRANCH;
            int start = std::max((int)std::round(a2.asDeg()), 0);
            int end   = std::min((int)std::round(a1.asDeg()), 359);
            for (int i = start; i < 360 || i % 360 <= end; i += 1) {
                _rays[i % 360] = _rayRadius;
            }
        } else {
            HG_LIKELY_BRANCH;
            int start = std::max((int)std::round(a1.asDeg()), 0);
            int end   = std::min((int)std::round(a2.asDeg()), 359);
            for (int i = start; i <= end; i += 1) {
                _rays[i] = _rayRadius;
            }
        }
    }
}

void TopDownLineOfSightCalculator::_processRays() {
    // TODO: see how rays deal with pairs of blocks where only corners are touching
    // TODO: optimize

    for (std::size_t i = 0; i < _rays.size(); i += 1) {
        if (_rays[i] != INFINITY) {
            continue;
        }

        const auto direction = hg::math::AngleF::fromDeg((double)i);
        const auto increment = direction.asNormalizedVector() * _world.getCellResolution() / 6.f;

        hg::math::Vector2f point = _lineOfSightOrigin + direction.asNormalizedVector() * _rayRadius;
        for (int t = 0; t < 400; t += 1) {
            point += increment;
            if (HG_UNLIKELY_CONDITION(point.x < 0.f || point.y < 0.f || point.x >= _xLimit ||
                                      point.y >= _yLimit)) {
                HG_UNLIKELY_BRANCH;
                _rays[i] = _rayRadius + (t + 1) * _world.getCellResolution() / 6.f;
                break;
            }

            const auto* cell = _world.getCellAtUnchecked(_world.posToCellUnchecked(point));
            if (cell == nullptr || cell->isWallInitialized()) {
                _rays[i] = _rayRadius + (t + 1) * _world.getCellResolution() / 6.f;
                break;
            }
        }
    }
}

bool TopDownLineOfSightCalculator::_isPointVisible(PositionInWorld aPosInWorld,
                                                   std::uint16_t   aFlags) const {
    const float dist = _raysDisabled ? 0.f : hg::math::EuclideanDist(*aPosInWorld, _lineOfSightOrigin);

    if (!_raysDisabled && dist > _rayRadius) {
        hg::math::Vector2f diff  = {aPosInWorld->x - _lineOfSightOrigin.x,
                                    aPosInWorld->y - _lineOfSightOrigin.y};
        const auto         angle = AngleF::fromVector(diff.x, diff.y);
        const int          idx   = hg::math::Clamp((int)std::round(angle.asDeg()), 0, 359);
        if (dist > _rays[idx]) {
            return false;
        }
    }

    const auto limit = _darkZones.size();
    for (std::int64_t i = 0; i < limit; i += 1) {
        const auto mask = _darkZones[i].flags & aFlags;
        if (HG_LIKELY_CONDITION((mask & (mask - 1)) == 0)) {
            HG_LIKELY_BRANCH;
            continue;
        }
        _comparisons += 1;
        if (hg::math::IsPointInsideTriangle(*aPosInWorld, _darkZones[i])) {
            return false;
        }
    }

    return true;
}

bool TopDownLineOfSightCalculator::_isLineVisible(PositionInWorld aP1,
                                                  PositionInWorld aP2,
                                                  std::uint16_t   aFlags,
                                                  hg::PZInteger   aLevels) const {
    if (aLevels <= 0) {
        return false;
    }

    const auto p = (*aP1 + *aP2) / 2.f;

    if (_isPointVisible(PositionInWorld{p}, aFlags)) {
        return true;
    }

    if (aLevels <= 1) {
        return false;
    } else {
        return (_isLineVisible(aP1, PositionInWorld{p}, aFlags, aLevels - 1) ||
                _isLineVisible(PositionInWorld{p}, aP2, aFlags, aLevels - 1));
    }
}

} // namespace gridgoblin
} // namespace jbatnozic
