#pragma once

#include <GridWorld/Model/Cell.hpp>

#include <Hobgoblin/Utility/Grids.hpp>

namespace gridworld {

namespace hg = jbatnozic::hobgoblin;

namespace detail {

class Chunk {
public:
    Chunk()                        = default;
    Chunk(const Chunk&)            = default;
    Chunk& operator=(const Chunk&) = default;
    Chunk(Chunk&&)                 = default;
    Chunk& operator=(Chunk&&)      = default;

    Chunk(hg::PZInteger aWidth, hg::PZInteger aHeight, const CellModel& aInitialValue = {})
        : _cells{aWidth, aHeight, CellModelExt{aInitialValue}} {}

    hg::PZInteger getCellCountX() const {
        return _cells.getWidth();
    }

    hg::PZInteger getCellCountY() const {
        return _cells.getHeight();
    }

    CellModelExt& getCellAtUnchecked(hg::PZInteger aX, hg::PZInteger aY) {
        return _cells[aY][aX];
    }

    const CellModelExt& getCellAtUnchecked(hg::PZInteger aX, hg::PZInteger aY) const {
        return _cells[aY][aX];
    }

    friend bool operator==(const Chunk& aLhs, const Chunk& aRhs) {
        if (aLhs.getCellCountX() != aRhs.getCellCountX() ||
            aLhs.getCellCountY() != aRhs.getCellCountY()) {
            return false;
        }

        for (hg::PZInteger y = 0; y < aLhs.getCellCountY(); y += 1) {
            for (hg::PZInteger x = 0; x < aRhs.getCellCountX(); x += 1) {
                const auto& lhsCell = static_cast<const CellModel&>(aLhs._cells[y][x]);
                const auto& rhsCell = static_cast<const CellModel&>(aRhs._cells[y][x]);
                if (lhsCell != rhsCell) {
                    return false;
                }
            }
        }

        // TBD: take into consideration chunk extensions?

        return true;
    }

    friend bool operator!=(const Chunk& aLhs, const Chunk& aRhs) {
        return !(aLhs == aRhs);
    }

private:
    hg::util::RowMajorGrid<CellModelExt> _cells;
};

} // namespace detail

} // namespace gridworld
