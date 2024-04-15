// Copyright 2024 Jovan Batnozic. Released under MS-PL licence in Serbia.
// See https://github.com/jbatnozic/Hobgoblin?tab=readme-ov-file#licence

// clang-format off

#ifndef UHOBGOBLIN_UTIL_GRIDS_HPP
#define UHOBGOBLIN_UTIL_GRIDS_HPP

#include <Hobgoblin/Common.hpp>

#include <utility>
#include <vector>

#include <Hobgoblin/Private/Pmacro_define.hpp>

HOBGOBLIN_NAMESPACE_BEGIN
namespace util {

template <class TData>
class RowMajorGrid {
public:
    RowMajorGrid();
    RowMajorGrid(PZInteger width, PZInteger height);
    RowMajorGrid(PZInteger width, PZInteger height, const TData& initialValue);

    RowMajorGrid(const RowMajorGrid& other) = default;
    RowMajorGrid& operator=(const RowMajorGrid& other) = default;

    RowMajorGrid(RowMajorGrid&& other) = default;
    RowMajorGrid& operator=(RowMajorGrid&& other) = default;

    void resize(PZInteger width, PZInteger height);
    void setAll(const TData& value);

    TData* operator[](PZInteger y);
    const TData* operator[](PZInteger y) const;
    
    TData& at(PZInteger y, PZInteger x);
    const TData& at(PZInteger y, PZInteger x) const;

    PZInteger getWidth() const noexcept;
    PZInteger getHeight() const noexcept;

private:
    std::vector<TData> _data;
    PZInteger _width;
    PZInteger _height;
};

template <class TData>
RowMajorGrid<TData>::RowMajorGrid()
    : _width{0}
    , _height{0}
{
}

template <class TData>
RowMajorGrid<TData>::RowMajorGrid(PZInteger width, PZInteger height)
    : _width{width}
    , _height{height}
{
    _data.resize(ToSz(_width * _height));
    _data.shrink_to_fit();
}

template <class TData>
RowMajorGrid<TData>::RowMajorGrid(PZInteger width, PZInteger height, const TData& initialValue)
    : _width{width}
    , _height{height}
{
    _data.resize(ToSz(_width * _height));
    _data.shrink_to_fit();
    for (auto& item : _data) {
        item = initialValue;
    }
}

template <class TData>
void RowMajorGrid<TData>::resize(PZInteger width, PZInteger height) {
    _width = width;
    _height = height;
    _data.clear();
    _data.resize(ToSz(_width * _height));
    _data.shrink_to_fit();
}

template <class TData>
void RowMajorGrid<TData>::setAll(const TData& value) {
    for (auto& item : _data) {
        item = value;
    }
}

template <class TData>
TData* RowMajorGrid<TData>::operator[](PZInteger y) {
    return std::addressof(_data[ToSz(y * _width)]);
}

template <class TData>
const TData* RowMajorGrid<TData>::operator[](PZInteger y) const {
    return std::addressof(_data[ToSz(y * _width)]);
}

template <class TData>
TData& RowMajorGrid<TData>::at(PZInteger y, PZInteger x) {
    return _data.at(ToSz(_width * y + x));
}

template <class TData>
const TData& RowMajorGrid<TData>::at(PZInteger y, PZInteger x) const {
    return _data.at(ToSz(_width * y + x));
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

// clang-format on
