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

    hg::PZInteger cetCellCountX() const {
        return _data.getWidth();
    }

    hg::PZInteger getCellCountY() const {
        return _data.getHeight();
    }

    CellModelExt& getCellAtUnchecked(hg::PZInteger aX, hg::PZInteger aY) {
        return _data[aY][aX];
    }

    const CellModelExt& getCellAtUnchecked(hg::PZInteger aX, hg::PZInteger aY) const {
        return _data[aY][aX];
    }

private:
    hg::util::RowMajorGrid<CellModelExt> _data;
};

} // namespace detail

} // namespace gridworld
