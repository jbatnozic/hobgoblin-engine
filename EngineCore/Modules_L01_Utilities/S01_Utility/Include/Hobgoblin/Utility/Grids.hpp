// Copyright 2024 Jovan Batnozic. Released under MS-PL licence in Serbia.
// See https://github.com/jbatnozic/Hobgoblin?tab=readme-ov-file#licence

#ifndef UHOBGOBLIN_UTIL_GRIDS_HPP
#define UHOBGOBLIN_UTIL_GRIDS_HPP

#include <Hobgoblin/Common.hpp>
#include <Hobgoblin/HGExcept.hpp>

#include <algorithm>
#include <memory>

#include <Hobgoblin/Private/Pmacro_define.hpp>

HOBGOBLIN_NAMESPACE_BEGIN
namespace util {

template <class TData>
class RowMajorGrid {
public:
    RowMajorGrid();
    RowMajorGrid(PZInteger width, PZInteger height);
    RowMajorGrid(PZInteger width, PZInteger height, const TData& initialValue);

    RowMajorGrid(const RowMajorGrid& other);
    RowMajorGrid& operator=(const RowMajorGrid& other);

    RowMajorGrid(RowMajorGrid&& other)            = default;
    RowMajorGrid& operator=(RowMajorGrid&& other) = default;

    //! Set width and height back to 0 and free all help memory.
    void reset();

    //! Set width and height to the given values. The values of individual fields in the grid
    //! will be undefined after this function call.
    void reset(PZInteger width, PZInteger height);

    //! Set width and height to the given values. The values of individual fields in the grid
    //! will all be `initialValue`.
    void reset(PZInteger width, PZInteger height, const TData& initialValue);

    //! Set all fields to a single given value.
    void setAll(const TData& value);

    TData*       operator[](PZInteger y);
    const TData* operator[](PZInteger y) const;

    TData&       at(PZInteger y, PZInteger x);
    const TData& at(PZInteger y, PZInteger x) const;

    PZInteger getWidth() const noexcept;
    PZInteger getHeight() const noexcept;

private:
    std::unique_ptr<TData[]> _data   = nullptr;
    PZInteger                _width  = 0;
    PZInteger                _height = 0;

    static std::unique_ptr<TData[]> _makeDataPtr(PZInteger width, PZInteger height) {
        if (width == 0 || height == 0) {
            return nullptr;
        }
        return std::make_unique<TData[]>(ToSz(width * height));
    }
};

template <class TData>
RowMajorGrid<TData>::RowMajorGrid() = default;

template <class TData>
RowMajorGrid<TData>::RowMajorGrid(PZInteger width, PZInteger height)
    : _data{_makeDataPtr(width, height)}
    , _width{width}
    , _height{height} {}

template <class TData>
RowMajorGrid<TData>::RowMajorGrid(PZInteger width, PZInteger height, const TData& initialValue)
    : RowMajorGrid(width, height) {
    setAll(initialValue);
}

template <class TData>
RowMajorGrid<TData>::RowMajorGrid(const RowMajorGrid& other)
    : RowMajorGrid(other._width, other._height) {
    std::copy_n(other._data.get(), ToSz(_width * _height), _data.get());
}

template <class TData>
RowMajorGrid<TData>& RowMajorGrid<TData>::operator=(const RowMajorGrid& other) {
    if (this != &other) {
        SELF = RowMajorGrid{other._width, other._height};
        std::copy_n(other._data.get(), ToSz(_width * _height), _data.get());
    }
    return SELF;
}

template <class TData>
void RowMajorGrid<TData>::reset() {
    _data.reset();
    _width = _height = 0;
}

template <class TData>
void RowMajorGrid<TData>::reset(PZInteger width, PZInteger height) {
    if (_width != width || _height != height) {
        _width  = width;
        _height = height;
        _data   = std::make_unique<TData[]>(ToSz(_width * _height));
    }
}

template <class TData>
void RowMajorGrid<TData>::reset(PZInteger width, PZInteger height, const TData& initialValue) {
    if (_width != width || _height != height) {
        _width  = width;
        _height = height;
        _data   = _makeDataPtr(width, height);
    }

    setAll(initialValue);
}

template <class TData>
void RowMajorGrid<TData>::setAll(const TData& value) {
    std::fill_n(_data.get(), ToSz(_width * _height), value);
}

template <class TData>
TData* RowMajorGrid<TData>::operator[](PZInteger y) {
    return _data.get() + (y * _width);
}

template <class TData>
const TData* RowMajorGrid<TData>::operator[](PZInteger y) const {
    return _data.get() + (y * _width);
}

template <class TData>
TData& RowMajorGrid<TData>::at(PZInteger y, PZInteger x) {
    HG_VALIDATE_ARGUMENT(y < _height && x < _width);
    return *(_data.get() + (y * _width) + x);
}

template <class TData>
const TData& RowMajorGrid<TData>::at(PZInteger y, PZInteger x) const {
    HG_VALIDATE_ARGUMENT(y < _height && x < _width);
    return *(_data.get() + (y * _width) + x);
}

template <class TData>
PZInteger RowMajorGrid<TData>::getWidth() const noexcept {
    return _width;
}

template <class TData>
PZInteger RowMajorGrid<TData>::getHeight() const noexcept {
    return _height;
}

} // namespace util
HOBGOBLIN_NAMESPACE_END

#include <Hobgoblin/Private/Pmacro_undef.hpp>
#include <Hobgoblin/Private/Short_namespace.hpp>

#endif // !UHOBGOBLIN_UTIL_GRIDS_HPP
