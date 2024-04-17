// Copyright 2024 Jovan Batnozic. Released under MS-PL licence in Serbia.
// See https://github.com/jbatnozic/Hobgoblin?tab=readme-ov-file#licence

#ifndef UHOBGOBLIN_UTIL_FLOW_FIELD_HPP
#define UHOBGOBLIN_UTIL_FLOW_FIELD_HPP

#include <Hobgoblin/Common.hpp>
#include <Hobgoblin/HGExcept.hpp>
#include <Hobgoblin/Math/Vector.hpp>
#include <Hobgoblin/Utility/Grids.hpp>

#include <array>
#include <cstdint>
#include <deque> // TODO: replace with more efficient deque implementation

#include <Hobgoblin/Private/Pmacro_define.hpp>

HOBGOBLIN_NAMESPACE_BEGIN
namespace util {

class DataProvider {
public:
    int getCostAt(math::Vector2pz aPosition);
};

using FlowField = util::RowMajorGrid<std::int16_t>;

class FlowFieldCalculator {
public:
    void setFieldDimensions(math::Vector2pz aDimensions);

    FlowField calculate(math::Vector2pz aTarget) {
        HG_VALIDATE_ARGUMENT(aTarget.x < _fieldDimensions.x && aTarget.y < _fieldDimensions.y,
                             "Target must be within the field.");

        auto flowField        = FlowField{_fieldDimensions.x, _fieldDimensions.y};
        auto integrationField = util::RowMajorGrid<std::int32_t>{_fieldDimensions.x, _fieldDimensions.y, 65535};

        // integrationField[aDimensions] = 0
        _queue.clear();
        _queue.push_back(aTarget);

        while (!_queue.empty()) {
            const auto curr = _queue.front();
            _queue.pop_front();

            std::array<math::Vector2pz, 8> neighbours;
            PZInteger validNeighboursCount = 0;
            _getValidNeighboursAroundPosition(curr, &neighbours, &validNeighboursCount);

            for (PZInteger i = 0; i < validNeighboursCount; i += 1) {
                const auto neighbour = neighbours[pztos(i)];
                const auto costFromCurrToNeighbour = integrationField[curr.y][curr.x] + getCostAt(neighbour); // TODO: handle impassable neighbours

                if (costFromCurrToNeighbour >= integrationField[neighbour.y][neighbour.x]) {
                    continue;
                }

                integrationField[neighbour.y][neighbour.x] = costFromCurrToNeighbour;
                _queue.push_back(neighbour);
            }
        }
    }

    int getCostAt(math::Vector2pz aPosition);

private:
    std::deque<math::Vector2pz> _queue;
    math::Vector2pz _fieldDimensions;

    void _getValidNeighboursAroundPosition(math::Vector2pz aPosition,
                                           std::array<math::Vector2pz, 8>* aNeighbours,
                                           PZInteger* aNeighbourCount) const {
        std::size_t count = 0;
        for (int yOff = -1; yOff <= 1; yOff += 1) {
            if (aPosition.y + yOff < 0 || aPosition.y + yOff >= _fieldDimensions.y) {
                continue;
            }
            for (int xOff = -1; xOff <= 1; xOff += 1) {
                if (aPosition.x + xOff < 0 || aPosition.x + xOff >= _fieldDimensions.x) {
                    continue;
                }
                if (yOff == 0 && xOff == 0) {
                    continue;
                }
                (*aNeighbours)[count] = {aPosition.x + xOff, aPosition.y + yOff};
                count += 1;
            }
        }
        (*aNeighbourCount) = stopz(count);
    }
};

} // namespace util
HOBGOBLIN_NAMESPACE_END

#include <Hobgoblin/Private/Pmacro_define.hpp>

#endif // !UHOBGOBLIN_UTIL_FLOW_FIELD_HPP
