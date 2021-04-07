#ifndef ROW_MAJOR_MATRIX_HPP
#define ROW_MAJOR_MATRIX_HPP

#include <algorithm>
#include <cassert>
#include <vector>
#include <stdexcept>

//#define T int
template <class T>
class RowMajorMatrix {
public:
    RowMajorMatrix(int width_, int height_)
        : width{verifyDimension(width_)}
        , height{verifyDimension(height_)}
        , data{}
    {
        data.resize(width_ * height_);
    }

    RowMajorMatrix(int width_, int height_, const T& initial_val)
        : width{verifyDimension(width_)}
        , height{verifyDimension(height_)}
        , data{}
    {
        data.resize(width_ * height_);
        std::fill_n(data.begin(), data.size(), initial_val);
    }

    int getWidth() const {
        return width;
    }

    int getHeight() const {
        return height;
    }

    T& at(int x, int y) {
        assert(x >= 0 && x < width);
        assert(y >= 0 && y < height);
        return data[y * width + x];
    }

    const T& at(int x, int y) const {
        assert(x >= 0 && x < width);
        assert(y >= 0 && y < height);
        return data[y * width + x];
    }

private:
    int width;
    int height;
    std::vector<T> data;

    int verifyDimension(int dimen) {
        if (dimen <= 0) {
            throw std::invalid_argument{"RowMajorMatrix::RowMajorMatrix - Dimensions must be positive"};
        }
        return dimen;
    }
};

#endif // !ROW_MAJOR_MATRIX_HPP

