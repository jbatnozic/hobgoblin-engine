#include <Hobgoblin/Graphics/Rectangle_shape.hpp>

#include <Hobgoblin/Math/Angle.hpp>

#include <cmath>

#include <Hobgoblin/Private/Pmacro_define.hpp>

HOBGOBLIN_NAMESPACE_BEGIN
namespace gr {

RectangleShape::RectangleShape(const math::Vector2f& aSize)
    : _size{aSize}
{
    _update();
}

void RectangleShape::setSize(const math::Vector2f& aSize) {
    _size = aSize;
    _update();
}

const math::Vector2f& RectangleShape::getSize() const {
    return _size;
}

PZInteger RectangleShape::getPointCount() const {
    return 4;
}

math::Vector2f RectangleShape::getPoint(PZInteger aIndex) const {
    switch (aIndex) {
        default:
        case 0: return math::Vector2f{0.f, 0.f};
        case 1: return math::Vector2f{_size.x, 0.f};
        case 2: return math::Vector2f{_size.x, _size.y};
        case 3: return math::Vector2f{0.f, _size.y};
    }
}

} // namespace gr
HOBGOBLIN_NAMESPACE_END
