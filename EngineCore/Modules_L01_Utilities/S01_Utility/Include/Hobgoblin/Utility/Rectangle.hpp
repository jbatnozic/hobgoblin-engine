// Copyright 2024 Jovan Batnozic. Released under MS-PL licence in Serbia.
// See https://github.com/jbatnozic/Hobgoblin?tab=readme-ov-file#licence

// clang-format off

#ifndef UHOBGOBLIN_UTIL_RECTANGLE_HPP
#define UHOBGOBLIN_UTIL_RECTANGLE_HPP

#include <type_traits>

#include <Hobgoblin/Private/Pmacro_define.hpp>

HOBGOBLIN_NAMESPACE_BEGIN
namespace util {

template <class T>
class Rectangle {
    static_assert(std::is_arithmetic_v<T>, "Class template Rectangle must be instantiated with an arithmetic type");
public:
    T x, y, w, h;

    Rectangle(T x, T y, T w, T h)
        : x{x}
        , y{y}
        , w{w}
        , h{h}
    {
    }

    Rectangle()
        : Rectangle(T{}, T{}, T{}, T{})
    {
    }

    bool overlaps(const Rectangle& other) const {
        if ((x + w <= other.x) || (x >= other.x + other.w) ||
            (y + h <= other.y) || (y >= other.y + other.h)) {
            return false;
        }
        return true;
    }

    bool envelopedBy(const Rectangle& other) const {
        if ((x >= other.x) && (x + w <= other.x + other.w) &&
            (y >= other.y) && (y + h <= other.y + other.h)) {
            return true;
        }
        return false;
    }

    void reset(T x, T y, T w, T h) {
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

} // namespace util
HOBGOBLIN_NAMESPACE_END

#include <Hobgoblin/Private/Pmacro_undef.hpp>
#include <Hobgoblin/Private/Short_namespace.hpp>

#endif // !UHOBGOBLIN_UTIL_RECTANGLE_HPP

// clang-format on
