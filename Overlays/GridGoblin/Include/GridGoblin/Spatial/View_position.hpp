// Copyright 2024 Jovan Batnozic. Released under MS-PL licence in Serbia.
// See https://github.com/jbatnozic/Hobgoblin?tab=readme-ov-file#licence

#pragma once

#include <Hobgoblin/Math/Vector.hpp>

namespace jbatnozic {
namespace gridgoblin {

namespace hg = ::jbatnozic::hobgoblin;

class ViewPosition {
public:
    ViewPosition()                               = default;
    ViewPosition(const ViewPosition&)            = default;
    ViewPosition& operator=(const ViewPosition&) = default;
    ViewPosition(ViewPosition&&)                 = default;
    ViewPosition& operator=(ViewPosition&&)      = default;

    explicit ViewPosition(float aX, float aY)
        : pos{aX, aY} {}

    explicit ViewPosition(hg::math::Vector2f aPos)
        : pos{aPos} {}

    hg::math::Vector2f pos;

    hg::math::Vector2f& operator*() {
        return pos;
    }

    const hg::math::Vector2f& operator*() const {
        return pos;
    }

    hg::math::Vector2f* operator->() {
        return &pos;
    }

    const hg::math::Vector2f* operator->() const {
        return &pos;
    }
};

} // namespace gridgoblin
} // namespace jbatnozic
