// Copyright 2024 Jovan Batnozic. Released under MS-PL licence in Serbia.
// See https://github.com/jbatnozic/Hobgoblin?tab=readme-ov-file#licence

#pragma once

#include <GridGoblin/Model/Cell_model.hpp>
#include <GridGoblin/Private/Cell_model_ext.hpp>

#include <Hobgoblin/Common.hpp>
#include <Hobgoblin/HGExcept.hpp>

#include <memory>

namespace jbatnozic {
namespace gridgoblin {
namespace detail {

namespace hg = ::jbatnozic::hobgoblin;

//! A specialized form of a row-majow grid, made specifically to hold GridGoblin Cells.
//! When not empty, it holds one additional cell (over width*height) - this last cell is not
//! used in the usual way, but instead it stores a pointer to a chunk extension (if any).
class CellGrid {
public:
    CellGrid();
    CellGrid(hg::PZInteger width, hg::PZInteger height);

    CellGrid(const CellGrid& other)            = delete;
    CellGrid& operator=(const CellGrid& other) = delete;

    CellGrid(CellGrid&& other);
    CellGrid& operator=(CellGrid&& other);

    //! Set width and height back to 0 and free all help memory.
    void reset();

    hg::PZInteger getWidth() const noexcept;
    hg::PZInteger getHeight() const noexcept;

    CellModelExt*       operator[](hg::PZInteger y);
    const CellModelExt* operator[](hg::PZInteger y) const;

    CellModelExt&       getExtensionCell();
    const CellModelExt& getExtensionCell() const;

private:
    std::unique_ptr<CellModelExt[]> _data   = nullptr;
    hg::PZInteger                   _width  = 0;
    hg::PZInteger                   _height = 0;

    static std::unique_ptr<CellModelExt[]> _makeDataPtr(hg::PZInteger width, hg::PZInteger height) {
        if (width == 0 || height == 0) {
            return nullptr;
        }
        return std::make_unique<CellModelExt[]>(hg::ToSz(width * height + 1));
    }
};

inline CellGrid::CellGrid()
    : CellGrid{0, 0} {}

inline CellGrid::CellGrid(hg::PZInteger width, hg::PZInteger height)
    : _data{_makeDataPtr(width, height)}
    , _width{width}
    , _height{height} {}

inline CellGrid::CellGrid(CellGrid&& other)
    : _data{std::move(other._data)}
    , _width{other._width}
    , _height{other._height} {
    other._width = other._height = 0;
}

inline CellGrid& CellGrid::operator=(CellGrid&& other) {
    if (&other != this) {
        _data        = std::move(other._data);
        _width       = other._width;
        _height      = other._height;
        other._width = other._height = 0;
    }
    return *this;
}

inline void CellGrid::reset() {
    _data.reset();
    _width = _height = 0;
}

inline hg::PZInteger CellGrid::getWidth() const noexcept {
    return _width;
}

inline hg::PZInteger CellGrid::getHeight() const noexcept {
    return _height;
}

inline CellModelExt* CellGrid::operator[](hg::PZInteger y) {
    return _data.get() + (y * _width);
}

inline const CellModelExt* CellGrid::operator[](hg::PZInteger y) const {
    return _data.get() + (y * _width);
}

inline CellModelExt& CellGrid::getExtensionCell() {
    HG_ASSERT(_data != nullptr);
    return *(_data.get() + (_width * _height));
}

inline const CellModelExt& CellGrid::getExtensionCell() const {
    return const_cast<CellGrid*>(this)->getExtensionCell();
}

} // namespace detail
} // namespace gridgoblin
} // namespace jbatnozic
