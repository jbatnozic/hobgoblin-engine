// Copyright 2024 Jovan Batnozic. Released under MS-PL licence in Serbia.
// See https://github.com/jbatnozic/Hobgoblin?tab=readme-ov-file#licence

// clang-format off


#pragma once

#include <Raynor/Matrix.hpp>
#include <Raynor/Vector.hpp>

#include <cmath>
#include <utility>

namespace jbatnozic {
namespace raynor {

template <class taReal>
Matrix<taReal, 1, 4> Homogenize(const Vec3<taReal>& aVec) {
    return Matrix<taReal, 1, 4>{ {aVec.x, aVec.y, aVec.z, taReal{1}} };
}

template <class taReal>
Vec3<taReal> Dehomogenize(const Matrix<taReal, 1, 4>& aMatrix) {
    return Vec3<taReal>{aMatrix[0][0], aMatrix[0][1], aMatrix[0][2]};
}

///////////////////////////////////////////////////////////////////////////////////////////////////

template <class taReal>
using TransformationMatrix = Matrix44<taReal>;

template <class taReal>
TransformationMatrix<taReal> TMatrixCreate() {
    return TransformationMatrix<taReal>::makeIdentity();
}

template <class taReal>
void TMatrixSetXAxisRotation(TransformationMatrix<taReal>& aThis, hg::math::Angle<taReal> aAngle) {
    const auto rad = aAngle.asRadians();
    TransformationMatrix<taReal> temp =
    {{
        {1, 0, 0, 0},
        {0, std::cos(rad), -std::sin(rad), 0},
        {0, std::sin(rad), std::cos(rad), 0},
        {0, 0, 0, 1}
    }};

    aThis = Multiply(aThis, temp);
}

template <class taReal>
void TMatrixSetYAxisRotation(TransformationMatrix<taReal>& aThis) {
    // TODO
}

template <class taReal>
void TMatrixSetZAxisRotation(TransformationMatrix<taReal>& aThis, hg::math::Angle<taReal> aAngle) {
    const auto rad = aAngle.asRadians();
    TransformationMatrix<taReal> temp =
    {{
        {std::cos(rad), -std::sin(rad), 0, 0},
        {std::sin(rad), std::cos(rad), 0, 0},
        {0, 0, 1, 0},
        {0, 0, 0, 1}
    }};
    
    aThis = Multiply(aThis, temp);
}

template <class taReal>
void TMatrixSetTranslation(TransformationMatrix<taReal>& aThis) {
    // TODO
}



} // namespace raynor
} // namespace jbatnozic

// clang-format on
