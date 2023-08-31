#include <Hobgoblin/Graphics/Transformable.hpp>

#include <Hobgoblin/Private/Pmacro_define.hpp>

HOBGOBLIN_NAMESPACE_BEGIN
namespace gr {

Transformable::Transformable() {}

Transformable::~Transformable() {}

void Transformable::setPosition(float aX, float aY) {}

void Transformable::setPosition(const math::Vector2f& aPosition) {}

void Transformable::setRotation(float aAngle) {}

void Transformable::setScale(float aFactorX, float aFactorY) {}

void Transformable::setScale(const math::Vector2f& aFactors) {}

void Transformable::setOrigin(float aX, float aY) {}

void Transformable::setOrigin(const math::Vector2f& aOrigin) {}

const math::Vector2f& Transformable::getPosition() const {}

float Transformable::getRotation() const {}

const math::Vector2f& Transformable::getScale() const {}

const math::Vector2f& Transformable::getOrigin() const {}

void Transformable::move(float aOffsetX, float aOffsetY) {}

void Transformable::move(const math::Vector2f& aOffset) {}

void Transformable::rotate(float aAngle) {}

void Transformable::scale(float aFactorX, float aFactorY) {}

void Transformable::scale(const math::Vector2f& aFactor) {}

const Transform& Transformable::getTransform() const {}

const Transform& Transformable::getInverseTransform() const {}

void* Transformable::_getSFMLImpl() {}

const void* Transformable::_getSFMLImpl() const {}

} // namespace gr
HOBGOBLIN_NAMESPACE_END

#include <Hobgoblin/Private/Pmacro_undef.hpp>
