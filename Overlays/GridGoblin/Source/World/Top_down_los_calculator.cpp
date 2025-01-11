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
} // namespace

TopDownLineOfSightCalculator::TopDownLineOfSightCalculator(const World& aWorld)
    : _world{aWorld} {
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
    _viewCenter        = aViewCenter;
    _viewSize          = aViewSize;
    _lineOfSightOrigin = *aLineOfSightOrigin;

    const auto cr = _world.getCellResolution();

    const float worldW = _world.getCellCountX() * cr;
    const float worldH = _world.getCellCountY() * cr;

    static constexpr float F_EPSILON = 0.05;

    using hg::math::Clamp;
    const float startFX = Clamp(aViewCenter->x - aViewSize.x * 0.5f, 0.f, worldW - F_EPSILON);
    const float startFY = Clamp(aViewCenter->y - aViewSize.y * 0.5f, 0.f, worldW - F_EPSILON);
    const float endFX   = Clamp(aViewCenter->x + aViewSize.x * 0.5f, 0.f, worldW - F_EPSILON);
    const float endFY   = Clamp(aViewCenter->y + aViewSize.y * 0.5f, 0.f, worldW - F_EPSILON);

    CalculationContext ctx{.lineOfSightOrigin     = aLineOfSightOrigin,
                           .lineOfSightOriginCell = _world.posToCell(*aLineOfSightOrigin),
                           .rectTopLeftCell       = _world.posToCell({startFX, startFY}),
                           .rectBottomRightCell   = _world.posToCell({endFX, endFY})};

    const int maxRings = 15;

    // ctx.radius = maxRings * cr -
    //              hg::math::EuclideanDist(*aLineOfSightOrigin,
    //                                      hg::math::Vector2f{(ctx.lineOfSightOriginCell.x + 0.5f) * cr,
    //                                                         (ctx.lineOfSightOriginCell.y + 0.5f) * cr});
    _rayRadius = ctx.radius =
        maxRings * cr -
        std::max(std::abs(aLineOfSightOrigin->x - (ctx.lineOfSightOriginCell.x + 0.5f) * cr),
                 std::abs(aLineOfSightOrigin->y - (ctx.lineOfSightOriginCell.y + 0.5f) * cr));

    const auto& center = ctx.lineOfSightOriginCell;
    const auto& start  = ctx.rectTopLeftCell;
    const auto& end    = ctx.rectBottomRightCell;

    hg::PZInteger ring = 1;
    while ((center.x - ring >= start.x) || (center.x + ring <= end.x) || (center.y - ring >= start.y) ||
           (center.y + ring <= end.y)) {
        const auto ringObstructed = _processRing(ring, ctx);
        // if (ringObstructed) {
        //     break;
        // }
        ring += 1;
        if (ring > maxRings) {
            break; // TODO: switch to rays after both maxRings and maxTriangles have been exceeded
                   //       ...maybe also a threshold for undetermined rays?
        }
    }

    if (ring > maxRings) {
        _processRays(ctx);
        _raysDisabled = false;
    }
}

bool TopDownLineOfSightCalculator::_processRing(hg::PZInteger             aRingIndex,
                                                const CalculationContext& aCtx) {
    if (aRingIndex == 0) {
        return false; // TODO: special treatment?
    }

    const auto xStart = aCtx.lineOfSightOriginCell.x - aRingIndex;
    const auto xEnd   = aCtx.lineOfSightOriginCell.x + aRingIndex;

#define Y_IS_IN_BOUNDS(_y_, _ctx_) \
    ((_y_) >= (_ctx_).rectTopLeftCell.y && (_y_) <= (_ctx_).rectBottomRightCell.y)
#define X_IS_IN_BOUNDS(_x_, _ctx_) \
    ((_x_) >= (_ctx_).rectTopLeftCell.x && (_x_) <= (_ctx_).rectBottomRightCell.x)

    hg::PZInteger obstructedCellCount = 0;

    // Top row
    {
        const auto y = aCtx.lineOfSightOriginCell.y - aRingIndex;
        if (Y_IS_IN_BOUNDS(y, aCtx)) {
            for (hg::PZInteger x = xStart; x <= xEnd; x += 1) {
                if (X_IS_IN_BOUNDS(x, aCtx)) {
                    obstructedCellCount += _processCell({x, y}, aCtx);
                } else {
                    // obstructedCellCount += 1;
                }
            }
        } else {
            // obstructedCellCount += (xEnd - xStart + 1);
        }
    }

    // Middle rows
    for (hg::PZInteger y = aCtx.lineOfSightOriginCell.y - aRingIndex + 1;
         y <= aCtx.lineOfSightOriginCell.y + aRingIndex - 1;
         y += 1) {
        if (Y_IS_IN_BOUNDS(y, aCtx)) {
            if (X_IS_IN_BOUNDS(xStart, aCtx)) {
                obstructedCellCount += _processCell({xStart, y}, aCtx);
            } else {
                // obstructedCellCount += 1;
            }
            if (X_IS_IN_BOUNDS(xEnd, aCtx)) {
                obstructedCellCount += _processCell({xEnd, y}, aCtx);
            } else {
                // obstructedCellCount += 1;
            }
        } else {
            // obstructedCellCount += 2;
        }
    }

    // Bottom row
    {
        const auto y = aCtx.lineOfSightOriginCell.y + aRingIndex;
        if (Y_IS_IN_BOUNDS(y, aCtx)) {
            for (hg::PZInteger x = xStart; x <= xEnd; x += 1) {
                if (X_IS_IN_BOUNDS(x, aCtx)) {
                    obstructedCellCount += _processCell({x, y}, aCtx);
                } else {
                    // obstructedCellCount += 1;
                }
            }
        } else {
            // obstructedCellCount += (xEnd - xStart + 1);
        }
    }

#undef X_IS_IN_BOUNDS
#undef Y_IS_IN_BOUNDS

    const auto cellsInRing = aRingIndex * 8;
    // return (obstructedCellCount == cellsInRing);
    return 0;
}

template <class T>
int Signum(T val) {
    return (T(0) < val) - (val < T(0));
}

hg::PZInteger TopDownLineOfSightCalculator::_processCell(hg::math::Vector2pz       aCell,
                                                         const CalculationContext& aCtx) {
    // if (std::abs((int)aCell.x - (int)aCell.y) > aCtx.cornerThreshold) {
    //     return 1;
    // }

    const auto* cell = _world.getCellAtUnchecked(aCell);
    if (cell == nullptr) {
        return 1;
    }
    if (!cell->isWallInitialized()) {
        return 0;
    }

    static constexpr CellFlags ALL_EDGES =
        CellModel::RIGHT_EDGE_OBSTRUCTED | CellModel::TOP_EDGE_OBSTRUCTED |
        CellModel::LEFT_EDGE_OBSTRUCTED | CellModel::BOTTOM_EDGE_OBSTRUCTED;

    CellFlags edgesOfInterest = ALL_EDGES;
    switch (Signum(aCell.x - aCtx.lineOfSightOriginCell.x)) {
    case -1:
        edgesOfInterest ^= CellModel::RIGHT_EDGE_OBSTRUCTED;
        break;
    case +1:
        edgesOfInterest ^= CellModel::LEFT_EDGE_OBSTRUCTED;
        break;
    }
    switch (Signum(aCell.y - aCtx.lineOfSightOriginCell.y)) {
    case -1:
        edgesOfInterest ^= CellModel::BOTTOM_EDGE_OBSTRUCTED;
        break;
    case 1:
        edgesOfInterest ^= CellModel::TOP_EDGE_OBSTRUCTED;
        break;
    }

    const auto cr = _world.getCellResolution();

    std::array<hg::math::Vector2f, 8> vertices;
    const auto vertCnt = GetUnobstructedVertices(*cell, aCell, edgesOfInterest, cr, vertices);

    if (vertCnt == 0) {
        return 0;
    }

    hg::PZInteger visibleCnt = 0;
    for (std::size_t i = 0; i < vertCnt; i += 1) {
        if (i > 0 && vertices[i] == vertices[i - 1]) {
            continue;
        }
        if (_isPointVisible(PositionInWorld{vertices[i]}, edgesOfInterest)) {
            visibleCnt += 1;
            break;
        }
        if (i % 2 == 1 && _isLineVisible(PositionInWorld{vertices[i]},
                                         PositionInWorld{vertices[i - 1]},
                                         edgesOfInterest,
                                         2)) {
            visibleCnt += 1;
            break;
        }
    }
    if (visibleCnt == 0) {
        return 1;
    }

    for (int i = 0; i < vertCnt; i += 2) {
        hg::math::AngleD a1, a2;

        const auto ray1End = [&vertices, &a1, i, &aCtx]() -> hg::math::Vector2f {
            hg::math::Vector2f diff = {vertices[i].x - aCtx.lineOfSightOrigin->x,
                                       vertices[i].y - aCtx.lineOfSightOrigin->y};
            diff.x *= 1000.f; // TODO: magic number (use trigonometry)
            diff.y *= 1000.f; // TODO: magic number (use trigonometry)

            a1 = hg::math::AngleD::fromVector(diff.x, diff.y);

            return vertices[i] + diff;
        }();
        const auto ray2End = [&vertices, &a2, i, &aCtx]() -> hg::math::Vector2f {
            hg::math::Vector2f diff = {vertices[i + 1].x - aCtx.lineOfSightOrigin->x,
                                       vertices[i + 1].y - aCtx.lineOfSightOrigin->y};
            diff.x *= 1000.f; // TODO: magic number (use trigonometry)
            diff.y *= 1000.f; // TODO: magic number (use trigonometry)

            a2 = hg::math::AngleD::fromVector(diff.x, diff.y);

            return vertices[i + 1] + diff;
        }();

        _darkZones.push_back({vertices[i], vertices[i + 1], ray1End, edgesOfInterest});
        _darkZones.push_back({ray1End, vertices[i + 1], ray2End, edgesOfInterest});


        if (a2 < a1) {
            std::swap(a1, a2);
        }
        if (HG_UNLIKELY_CONDITION(a1 <= hg::math::AngleD::halfCircle() * 0.5 &&
                                  a2 >= hg::math::AngleD::halfCircle() * 1.5)) {
            HG_UNLIKELY_BRANCH;
            int start = std::max((int)std::round(a2.asDeg()), 0);
            int end   = std::min((int)std::round(a1.asDeg()), 359);
            for (int i = start; i < 360 || i % 360 <= end; i += 1) {
                _rays[i % 360] = aCtx.radius;
            }
        } else {
            HG_LIKELY_BRANCH;
            int start = std::max((int)std::round(a1.asDeg()), 0);
            int end   = std::min((int)std::round(a2.asDeg()), 359);
            for (int i = start; i <= end; i += 1) {
                _rays[i] = aCtx.radius;
            }
        }        
    }

    return 0;
}

void TopDownLineOfSightCalculator::_processRays(const CalculationContext& aCtx) {
    // TODO: see how rays deal with pairs of blocks where only corners are touching
    // TODO: optimize

    const auto maxX = _world.getCellCountX() * _world.getCellResolution();
    const auto maxY = _world.getCellCountY() * _world.getCellResolution();

    for (std::size_t i = 0; i < _rays.size(); i += 1) {
        if (_rays[i] != INFINITY) {
            continue;
        }

        const auto direction = hg::math::AngleF::fromDeg((double)i);
        const auto increment = direction.asNormalizedVector() * _world.getCellResolution() / 6.f;

        hg::math::Vector2f point = _lineOfSightOrigin + direction.asNormalizedVector() * _rayRadius;
        for (int t = 0; t < 400; t += 1) {
            point += increment;
            if (HG_UNLIKELY_CONDITION(point.x < 0.f || point.y < 0.f || point.x >= maxX || point.y >= maxY)) {
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
    float dist;
    if (!_raysDisabled) {
        dist = hg::math::EuclideanDist(*aPosInWorld, _lineOfSightOrigin);
    }

    if (_raysDisabled || dist < _rayRadius) {
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
    } else {
        hg::math::Vector2f diff = {aPosInWorld->x - _lineOfSightOrigin.x,
                                   aPosInWorld->y - _lineOfSightOrigin.y};
        const auto angle = hg::math::AngleD::fromVector(diff.x, diff.y);
        const int idx = hg::math::Clamp((int)std::round(angle.asDeg()), 0, 359);
        if (dist > _rays[idx]) {
            return false;
        }

        // Have to ALSO check triangles
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
