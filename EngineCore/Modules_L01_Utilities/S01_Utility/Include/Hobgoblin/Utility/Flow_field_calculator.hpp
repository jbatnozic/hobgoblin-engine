// Copyright 2024 Jovan Batnozic. Released under MS-PL licence in Serbia.
// See https://github.com/jbatnozic/Hobgoblin?tab=readme-ov-file#licence

#ifndef UHOBGOBLIN_UTIL_FLOW_FIELD_HPP
#define UHOBGOBLIN_UTIL_FLOW_FIELD_HPP

#include <Hobgoblin/Common.hpp>
#include <Hobgoblin/HGExcept.hpp>
#include <Hobgoblin/Math/Angle.hpp>
#include <Hobgoblin/Math/Vector.hpp>
#include <Hobgoblin/Utility/Compact_angle.hpp>
#include <Hobgoblin/Utility/Grids.hpp>

#include <array>
#include <cmath>
#include <cstdint>
#include <deque> // TODO: replace with more efficient deque implementation
#include <memory>

#include <Hobgoblin/Private/Pmacro_define.hpp>

HOBGOBLIN_NAMESPACE_BEGIN
namespace util {

constexpr std::uint8_t COST_IMPASSABLE = 255;

/*
class DataProvider {
public:
    std::uint8_t getCostAt(math::Vector2pz aPosition) const {
        return costs.at(aPosition.y, aPosition.x);
    }

    util::RowMajorGrid<std::uint8_t> costs;
};
*/

//! TODO(description)
using FlowField = util::RowMajorGrid<CompactAngle>;

//! TODO(description)
template <class taCostProvider>
class FlowFieldCalculator { 
public:
    //! TODO(description)
    void reset(math::Vector2pz aFieldDimensions,
               math::Vector2pz aTarget,
               const taCostProvider& aCostProvider);

    //! If `false` is returned, it needs to be called again (until `true` is returned)
    //! before `calculateFlowField` can be called.
    bool calculateIntegrationField(PZInteger aMaxIterationCount);

    //! Calculates the integration field which is needed before
    //! the flow field can be calculated. This work is not parallelizable.
    void calculateIntegrationField();

    //! TODO(description)
    void calculateFlowField(PZInteger aStartingRow,
                            PZInteger aRowCount);

    //! Calculates the entire flow field.
    void calculateFlowField();

    //! TODO(description)
    std::shared_ptr<FlowField> takeFlowField();

private:
    using NeighbourArray = std::array<std::optional<math::Vector2i>, 8>;
    using IntegrationField = util::RowMajorGrid<std::int32_t>;
    static constexpr std::int32_t INTEGRATION_FIELD_MAX_COST = 0x7FFFFFFF;

    math::Vector2pz _fieldDimensions;
    math::Vector2pz _source;
    math::Vector2pz _target;

    const taCostProvider* _costProvider = nullptr;

    std::shared_ptr<FlowField> _flowField = nullptr;

    //! Reusable integration field (grid)
    IntegrationField _integrationField;

    //! Reusable queue for calculating the integration field
    std::deque<math::Vector2pz> _queue;

    //! 0 1 2
    //! 3 X 4
    //! 5 6 7
    void _getValidNeighboursAroundPosition(math::Vector2pz aPosition,
                                           NeighbourArray* aNeighbours,
                                           bool aDiagonalAllowed) const;

    bool _calculateIntegrationField(PZInteger aMaxIterationCount);
    void _calculateFlowField(PZInteger aStartingRow, PZInteger aRowCount) const;

    std::uint8_t _getCostAt(math::Vector2pz aPosition) const {
        return _costProvider->getCostAt(aPosition);
    }
};

///////////////////////////////////////////////////////////////////////////
// IMPLEMENTATIONS                                                       //
///////////////////////////////////////////////////////////////////////////

namespace detail {
template <class taGrid, class taPosition>
auto GridUnsafeAt(taGrid&& aGrid, taPosition aPosition) -> decltype(aGrid[0][0])& {
    return aGrid[aPosition.y][aPosition.x];
}

inline
const std::array<CompactAngle, 8> NEIGHBOUR_DIRECTIONS = {
    CompactAngle{math::AngleF::fromDegrees(135.f)},
    CompactAngle{math::AngleF::fromDegrees( 90.f)},
    CompactAngle{math::AngleF::fromDegrees( 45.f)},
    CompactAngle{math::AngleF::fromDegrees(180.f)},
    CompactAngle{math::AngleF::fromDegrees(  0.f)},
    CompactAngle{math::AngleF::fromDegrees(225.f)},
    CompactAngle{math::AngleF::fromDegrees(270.f)},
    CompactAngle{math::AngleF::fromDegrees(315.f)}
};

//! 0 1 2
//! 3 X 4
//! 5 6 7
inline
CompactAngle GetDirectionTowardsNeighbour(PZInteger aNeighbourIndex) {
    return NEIGHBOUR_DIRECTIONS[pztos(aNeighbourIndex)];
}
} // namespace detail

#define GRID_U_AT detail::GridUnsafeAt

template <class taCostProvider>
void FlowFieldCalculator<taCostProvider>::reset(math::Vector2pz aFieldDimensions,
                                                math::Vector2pz aTarget,
                                                const taCostProvider& aCostProvider) {
    HG_VALIDATE_ARGUMENT(aFieldDimensions.x > 0 && aFieldDimensions.y > 0,
                         "Field width and height must both be greater than 0.");
    HG_VALIDATE_ARGUMENT(aTarget.x < aFieldDimensions.x && aTarget.y < aFieldDimensions.y,
                         "Target must be within the field.");

    _fieldDimensions = aFieldDimensions;
    _target = aTarget;
    _costProvider = &aCostProvider;

    _flowField = std::make_shared<FlowField>(_fieldDimensions.x, _fieldDimensions.y, CompactAngle{});

    _integrationField.resize(_fieldDimensions.x, _fieldDimensions.y);
    _integrationField.setAll(INTEGRATION_FIELD_MAX_COST);

    _queue.clear();
    GRID_U_AT(_integrationField, _target) = 0;
    _queue.push_back(_target);
}

template <class taCostProvider>
bool FlowFieldCalculator<taCostProvider>::calculateIntegrationField(PZInteger aMaxIterationCount) {
    return _calculateIntegrationField(aMaxIterationCount);
}

template <class taCostProvider>
void FlowFieldCalculator<taCostProvider>::calculateIntegrationField() {
    while (true) {
        if (calculateIntegrationField(1'000'000)) {
            return;
        }
    }
}

template <class taCostProvider>
void FlowFieldCalculator<taCostProvider>::calculateFlowField(PZInteger aStartingRow,
                                                             PZInteger aRowCount) {
    HG_VALIDATE_ARGUMENT(aStartingRow < _fieldDimensions.y &&
                            aStartingRow + aRowCount <= _fieldDimensions.y,
                            "Invalid row indices provided.");
    _calculateFlowField(aStartingRow, aRowCount);
}

template <class taCostProvider>
void FlowFieldCalculator<taCostProvider>::calculateFlowField() {
    calculateFlowField(0, _fieldDimensions.y);
}

template <class taCostProvider>
std::shared_ptr<FlowField> FlowFieldCalculator<taCostProvider>::takeFlowField() {
    std::shared_ptr<FlowField> result = _flowField;
    _flowField.reset();
    return result;
}

template <class taCostProvider>
bool FlowFieldCalculator<taCostProvider>::_calculateIntegrationField(PZInteger aMaxIterationCount) {
    for (PZInteger i = 0; i < aMaxIterationCount; i += 1) {
        if (_queue.empty()) {
            return true; // Done
        }

        const auto curr = _queue.front();
        _queue.pop_front();

        if (_getCostAt(curr) == COST_IMPASSABLE) {
            continue;
        }

        NeighbourArray neighbours;
        _getValidNeighboursAroundPosition(curr, &neighbours, false);

        for (PZInteger i = 0; i < 8; i += 1) {
            if (!neighbours[pztos(i)].has_value()) {
                continue;
            }

            const auto neighbourOffset = neighbours[pztos(i)];
            const auto neighbourPosition = curr + *neighbourOffset;
            const auto neighbourCost = _getCostAt(neighbourPosition);
            if (neighbourCost == COST_IMPASSABLE) {
                continue; // Impassable cell
            }

            const auto costFromCurrToNeighbour = GRID_U_AT(_integrationField, curr) + neighbourCost;

            if (costFromCurrToNeighbour >= GRID_U_AT(_integrationField, neighbourPosition)) {
                continue; // Neighbour already has a better path
            }

            GRID_U_AT(_integrationField, neighbourPosition) = costFromCurrToNeighbour;
            _queue.push_back(neighbourPosition);
        }
    }

    return false; // Not finished yet
}

template <class taCostProvider>
void FlowFieldCalculator<taCostProvider>::_getValidNeighboursAroundPosition(math::Vector2pz aPosition,
                                                                            NeighbourArray* aNeighbours,
                                                                            bool aDiagonalAllowed) const {
    static const std::array<math::Vector2i, 8> OFFSETS = {
        // Row above
        math::Vector2i{-1, -1},
        { 0, -1},
        {+1, -1},
        // Same row
        {-1,  0},
        // { 0,  0}, <-- self
        {+1,  0},
        // Row below
        {-1, +1},
        { 0, +1},
        {+1, +1}
    };

    for (std::size_t i = 0; i < 8; i += 1) {
        const auto xOff = OFFSETS[i].x;
        const auto yOff = OFFSETS[i].y;
        if (!aDiagonalAllowed && !(xOff == 0 || yOff == 0)) {
            (*aNeighbours)[i].reset();
            continue;
        }

        if (aPosition.y + yOff < 0 || aPosition.y + yOff >= _fieldDimensions.y ||
            aPosition.x + xOff < 0 || aPosition.x + xOff >= _fieldDimensions.x) {
            (*aNeighbours)[i].reset();
            continue;
        }

        (*aNeighbours)[i] = {xOff, yOff};
    }
}

template <class taCostProvider>
void FlowFieldCalculator<taCostProvider>::_calculateFlowField(PZInteger aStartingRow,
                                                              PZInteger aRowCount) const {
    HG_HARD_ASSERT(_flowField != nullptr);

    for (PZInteger y = aStartingRow; y < aStartingRow + aRowCount; y += 1) {
        for (PZInteger x = 0; x < _fieldDimensions.x; x += 1) {
            if (_getCostAt({x, y}) == COST_IMPASSABLE) {
                continue;
            }

            NeighbourArray neighbours;
            _getValidNeighboursAroundPosition({x, y}, &neighbours, true);
            
            std::int32_t minCost = INTEGRATION_FIELD_MAX_COST;
            for (PZInteger i = 0; i < 8; i += 1) {
                if (!neighbours[pztos(i)].has_value()) {
                    continue;
                }

                const auto neighbourOffset = neighbours[pztos(i)];

                // Prevent diagonal turns through corners
                if ((neighbourOffset->x & neighbourOffset->y) != 0) {
                    if (neighbourOffset->x == -1 && neighbourOffset->y == -1) {
                        if (_getCostAt({x - 1, y}) == COST_IMPASSABLE ||
                            _getCostAt({x, y - 1}) == COST_IMPASSABLE) {
                            continue;
                        }
                    } else if (neighbourOffset->x == +1 && neighbourOffset->y == -1) {
                        if (_getCostAt({x + 1, y}) == COST_IMPASSABLE ||
                            _getCostAt({x, y - 1}) == COST_IMPASSABLE) {
                            continue;
                        }    
                    } else if (neighbourOffset->x == -1 && neighbourOffset->y == +1) {
                        if (_getCostAt({x - 1, y}) == COST_IMPASSABLE ||
                            _getCostAt({x, y + 1}) == COST_IMPASSABLE) {
                            continue;
                        }  
                    } else if (neighbourOffset->x == +1 && neighbourOffset->y == +1) {
                        if (_getCostAt({x + 1, y}) == COST_IMPASSABLE ||
                            _getCostAt({x, y + 1}) == COST_IMPASSABLE) {
                            continue;
                        }   
                    }
                }

                const auto cost = GRID_U_AT(_integrationField, *neighbourOffset + math::Vector2pz(x, y));
                if (cost < minCost) {
                    minCost = cost;
                    (*_flowField)[y][x] = detail::GetDirectionTowardsNeighbour(i);
                }
            }
        }
    }
}

#undef GRID_U_AT

} // namespace util
HOBGOBLIN_NAMESPACE_END

#include <Hobgoblin/Private/Pmacro_define.hpp>

#endif // !UHOBGOBLIN_UTIL_FLOW_FIELD_HPP
