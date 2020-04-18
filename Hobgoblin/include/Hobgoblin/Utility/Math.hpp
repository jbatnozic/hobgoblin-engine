#ifndef UHOBGOBLIN_UTIL_MATH_HPP
#define UHOBGOBLIN_UTIL_MATH_HPP

#include <SFML/System/Vector2.hpp>
#include <SFML/System/Vector3.hpp>

#include <algorithm>
#include <cmath>

#include <Hobgoblin/Private/Pmacro_define.hpp>

HOBGOBLIN_NAMESPACE_START
namespace util {

using sf::Vector2;
using sf::Vector2i;
using sf::Vector2u;
using sf::Vector2f;
using Vector2d = Vector2<double>;

using sf::Vector3;
using sf::Vector3i;
using Vector3u = Vector3<unsigned>;
using sf::Vector3f;
using Vector3d = Vector3<double>;

template <class T>
T Clamp(const T& value, const T& low, const T& high) {
    return std::min(high, std::max(value, low));
}

template <class T>
T Sqr(T value) {
    return value * value;
}

template <class T>
float EuclideanDist(const Vector2<T>& p1, const Vector2<T>& p2) {
    return std::sqrt(Sqr(p2.x - p1.x) + Sqr(p2.y - p1.y));
}

} // namespace util
HOBGOBLIN_NAMESPACE_END

#include <Hobgoblin/Private/Pmacro_undef.hpp>
#include <Hobgoblin/Private/Short_namespace.hpp>

#endif // !UHOBGOBLIN_UTIL_MATH_HPP