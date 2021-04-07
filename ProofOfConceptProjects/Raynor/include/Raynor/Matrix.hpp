
#pragma once

#include <Hobgoblin/Common.hpp>

namespace jbatnozic {
namespace raynor {

template <class taReal, hg::PZInteger taHeight, hg::PZInteger taWidth>
class Matrix {
    static_assert(taHeight > 0 && taWidth > 0);
public:
    using Real = taReal;

    static constexpr hg::PZInteger HEIGHT = taHeight;
    static constexpr hg::PZInteger WIDTH = taWidth;
    
#define _elems __jbatnozic_raynor_privateData

    static constexpr Matrix makeIdentity() {
        static_assert(WIDTH == HEIGHT);

        Matrix result;
        for (hg::PZInteger y = 0; y < HEIGHT; y += 1) {
            for (hg::PZInteger x = 0; x < WIDTH; x += 1) {
                if (x == y) {
                    result._elems[y][x] = Real{1.0};
                }
                else {
                    result._elems[y][x] = Real{0.0};
                }
            }
        }
        return result;
    }

    taReal* operator[](hg::PZInteger aY) {
        return _elems[aY];
    }

    const taReal* operator[](hg::PZInteger aY) const {
        return _elems[aY];
    }

#undef  _elems

    Real __jbatnozic_raynor_privateData[taHeight][taWidth] = {Real{0.0}}; // Zero-init everything
};

template <class taReal>
using Matrix33 = Matrix<taReal, 3, 3>;

template <class taReal>
using Matrix44 = Matrix<taReal, 4, 4>;

template <class taReal, 
          hg::PZInteger taHeight1, hg::PZInteger taWidth1, hg::PZInteger taHeight2, hg::PZInteger taWidth2>
Matrix<taReal, taHeight1, taWidth2> Multiply(const Matrix<taReal, taHeight1, taWidth1>& aMatrix1,
                                             const Matrix<taReal, taHeight2, taWidth2>& aMatrix2) {
    static_assert(taWidth1 == taHeight2);

    Matrix<taReal, taHeight1, taWidth2> result;

    for (hg::PZInteger y = 0; y < taHeight1; y += 1) {
        for (hg::PZInteger x = 0; x < taWidth2; x += 1) {
            taReal value = taReal{0.0};
            for (hg::PZInteger i = 0; i < taWidth1; i += 1) {
                value += aMatrix1[y][i] * aMatrix2[i][x];
            }
            result[y][x] = value;
        }
    }

    return result;
}

// TODO To transformations.hpp
template <class taReal>
Matrix33<taReal> MakeZAxisRotationMatrix33(hg::math::Angle<taReal> aAngle) {
    auto result = Matrix33<taReal>::makeIdentity();
    result[0][0] = +std::cos(aAngle.asRadians());
    result[0][1] = -std::sin(aAngle.asRadians());
    result[1][0] = +std::sin(aAngle.asRadians());
    result[1][1] = +std::cos(aAngle.asRadians());
    return result;
}

} // namespace raynor
} // namespace jbatnozic