// Copyright 2024 Jovan Batnozic. Released under MS-PL licence in Serbia.
// See https://github.com/jbatnozic/Hobgoblin?tab=readme-ov-file#licence

// clang-format off

#ifndef UHOBGOBLIN_MATH_RECTANGLE_HPP
#define UHOBGOBLIN_MATH_RECTANGLE_HPP

#include <cassert>
#include <cmath>
#include <type_traits>

#include <Hobgoblin/Private/Pmacro_define.hpp>

HOBGOBLIN_NAMESPACE_BEGIN
namespace math {

template <class taArithmetic>
class Rectangle {
    static_assert(std::is_arithmetic_v<taArithmetic>, "Class template Rectangle must be instantiated with an arithmetic type");
public:
    using ValueType = taArithmetic;

    //! X-coordinate of the top-left corner of the rectanlge.
    taArithmetic x;
    //! Y-coordinate of the top-left corner of the rectanlge.
    taArithmetic y;
    //! The width of the rectangle (assumed non-negative).
    taArithmetic w;
    //! The height of the rectangle (assumed non-negative).
    taArithmetic h;

    Rectangle(taArithmetic x, taArithmetic y, taArithmetic w, taArithmetic h)
        : x{x}
        , y{y}
        , w{w}
        , h{h}
    {
        assert(w >= static_cast<taArithmetic>(0) && "The width of the rectangle can't be negative");
        assert(h >= static_cast<taArithmetic>(0) && "The height of the rectangle can't be negative");
    }

    //! Initializes the rectangle with zero in all fields.
    Rectangle()
        : Rectangle(
            static_cast<taArithmetic>(0),
            static_cast<taArithmetic>(0),
            static_cast<taArithmetic>(0),
            static_cast<taArithmetic>(0)
        )
    {
    }

    taArithmetic getLeft() const {
        return x;
    }

    taArithmetic getRight() const {
        return x + w;
    }

    taArithmetic getTop() const {
        return y;
    }

    taArithmetic getBottom() const {
        return y + h;
    }

    bool overlaps(const Rectangle& other) const {
        if ((x + w <= other.x) || (x >= other.x + other.w) ||
            (y + h <= other.y) || (y >= other.y + other.h)) {
            return false;
        }
        return true;
    }

    bool isEnvelopedBy(const Rectangle& other) const {
        if ((x >= other.x) && (x + w <= other.x + other.w) &&
            (y >= other.y) && (y + h <= other.y + other.h)) {
            return true;
        }
        return false;
    }

    bool isEqualTo(const Rectangle& aOther, taArithmetic aEpsilon) const {
        return (abs(x - aOther.x) <= aEpsilon &&
                abs(y - aOther.y) <= aEpsilon &&
                abs(w - aOther.w) <= aEpsilon &&
                abs(h - aOther.h) <= aEpsilon);
    }

    void reset(taArithmetic x, taArithmetic y, taArithmetic w, taArithmetic h) {
        SELF.x = x;
        SELF.y = y;
        SELF.w = w;
        SELF.h = h;
    }

    // Copy:
    Rectangle(const Rectangle& other) = default;
    Rectangle& operator=(const Rectangle& other) = default;

    // Move:
    Rectangle(Rectangle&& other) = default;
    Rectangle& operator=(Rectangle&& other) = default;
};

template <class taArithmetic>
bool operator==(const Rectangle<taArithmetic>& aLhs, const Rectangle<taArithmetic>& aRhs) {
    return (aLhs.x == aRhs.x &&
            aLhs.y == aRhs.y &&
            aLhs.w == aRhs.w &&
            aLhs.h == aRhs.h);
}

template <class taArithmetic>
bool operator!=(const Rectangle<taArithmetic>& aLhs, const Rectangle<taArithmetic>& aRhs) {
    return !(aLhs == aRhs);
}

} // namespace math
HOBGOBLIN_NAMESPACE_END

#include <Hobgoblin/Private/Pmacro_undef.hpp>
#include <Hobgoblin/Private/Short_namespace.hpp>

#endif // !UHOBGOBLIN_MATH_RECTANGLE_HPP

// clang-format on
