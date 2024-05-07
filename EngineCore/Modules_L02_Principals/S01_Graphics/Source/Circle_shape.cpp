// Copyright 2024 Jovan Batnozic. Released under MS-PL licence in Serbia.
// See https://github.com/jbatnozic/Hobgoblin?tab=readme-ov-file#licence

// clang-format off

#include <Hobgoblin/Graphics/Circle_shape.hpp>

#include <Hobgoblin/Math/Angle.hpp>

#include <cmath>

#include <Hobgoblin/Private/Pmacro_define.hpp>

HOBGOBLIN_NAMESPACE_BEGIN
namespace gr {

CircleShape::CircleShape(float aRadius, PZInteger aPointCount)
    : _radius{aRadius}
    , _pointCount{aPointCount}
{
    _update();
}

void CircleShape::setRadius(float aRadius) {
    _radius = aRadius;
    _update();
}

float CircleShape::getRadius() const {
    return _radius;
}

void CircleShape::setPointCount(PZInteger aCount) {
    _pointCount = aCount;
    _update();
}

PZInteger CircleShape::getPointCount() const {
    return _pointCount;
}

math::Vector2f CircleShape::getPoint(PZInteger aIndex) const {
    float angle = (static_cast<float>(aIndex) * 2.f * math::PI_F / static_cast<float>(_pointCount)) - (math::PI_F / 2.f);
    float x = std::cos(angle) * _radius;
    float y = std::sin(angle) * _radius;

    return {_radius + x, _radius + y};
}

} // namespace gr
HOBGOBLIN_NAMESPACE_END

// clang-format on
