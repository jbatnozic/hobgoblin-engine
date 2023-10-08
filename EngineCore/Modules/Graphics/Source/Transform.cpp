#include <Hobgoblin/Graphics/Transform.hpp>

#include <SFML/Graphics/Transform.hpp>

#include <new>

#include "SFML_conversions.hpp"

#include <Hobgoblin/Private/Pmacro_define.hpp>

HOBGOBLIN_NAMESPACE_BEGIN
namespace gr {

using ImplType = sf::Transform;
constexpr auto IMPL_SIZE  = sizeof(ImplType);
constexpr auto IMPL_ALIGN = alignof(ImplType);
#define  IMPLOF(_obj_) (reinterpret_cast<ImplType*>(&((_obj_)._storage)))
#define CIMPLOF(_obj_) (reinterpret_cast<const ImplType*>(&((_obj_)._storage)))
#define  SELF_IMPL (IMPLOF(SELF))
#define SELF_CIMPL (CIMPLOF(SELF))

Transform::Transform() {
    static_assert(STORAGE_SIZE == IMPL_SIZE,   "Transform::STORAGE_SIZE is inadequate.");
    static_assert(STORAGE_ALIGN == IMPL_ALIGN, "Transform::STORAGE_ALIGN is inadequate.");

    new (&_storage) ImplType();
}

Transform::Transform(float a00, float a01, float a02,
                     float a10, float a11, float a12,
                     float a20, float a21, float a22) {
    new (&_storage) ImplType(a00, a01, a02, a10, a11, a12, a20, a21, a22);
}

Transform::~Transform() {
    SELF_IMPL->~ImplType();
}

const float* Transform::getMatrix() const {
    return SELF_CIMPL->getMatrix();
}

Transform Transform::getInverse() const {
    return ToHg(SELF_CIMPL->getInverse());
}

math::Vector2f Transform::transformPoint(float aX, float aY) const {
    const auto vec = SELF_CIMPL->transformPoint(aX, aY);
    return {vec.x, vec.y};
}

math::Vector2f Transform::transformPoint(const math::Vector2f& aPoint) const {
    const auto vec = SELF_CIMPL->transformPoint(aPoint.x, aPoint.y);
    return {vec.x, vec.y};
}

math::Rectangle<float> Transform::transformRect(const math::Rectangle<float>& aRectangle) const {
    const auto rect = SELF_CIMPL->transformRect({
        aRectangle.getLeft(),
        aRectangle.getTop(),
        aRectangle.w,
        aRectangle.h,
    });
    return {
        rect.left,
        rect.top,
        rect.width,
        rect.height
    };
}

Transform& Transform::combine(const Transform& aTransform) {
    SELF_IMPL->combine(ToSf(aTransform));
    return SELF;
}

Transform& Transform::translate(float aX, float aY) {
    SELF_IMPL->translate(aX, aY);
    return SELF;
}

Transform& Transform::translate(const math::Vector2f& aOffset) {
    SELF_IMPL->translate({aOffset.x, aOffset.y});
    return SELF;
}

Transform& Transform::rotate(math::AngleF aAngle) {
    SELF_IMPL->rotate(-aAngle.asDeg());
    return SELF;
}

Transform& Transform::rotate(math::AngleF aAngle, float aCenterX, float aCenterY) {
    SELF_IMPL->rotate(-aAngle.asDeg(), aCenterX, aCenterY);
    return SELF;
}

Transform& Transform::rotate(math::AngleF aAngle, const math::Vector2f& aCenter) {
    SELF_IMPL->rotate(-aAngle.asDeg(), {aCenter.x, aCenter.y});
    return SELF;
}

Transform& Transform::scale(float aScaleX, float aScaleY) {
    SELF_IMPL->scale(aScaleX, aScaleY);
    return SELF;
}

Transform& Transform::scale(float aScaleX, float aScaleY, float aCenterX, float aCenterY) {
    SELF_IMPL->scale(aScaleX, aScaleY, aCenterX, aCenterY);
    return SELF;
}

Transform& Transform::scale(const math::Vector2f& aFactors) {
    SELF_IMPL->scale({aFactors.x, aFactors.y});
    return SELF;
}

Transform& Transform::scale(const math::Vector2f& aFactors, const math::Vector2f& aCenter) {
    SELF_IMPL->scale({aFactors.x, aFactors.y}, {aCenter.x, aCenter.y});
    return SELF;
}

static const Transform Identity; // TODO

void* Transform::_getSFMLImpl() {
    return SELF_IMPL;
}

const void* Transform::_getSFMLImpl() const {
    return SELF_CIMPL;
}

Transform operator*(const Transform& aLhs, const Transform& aRhs) {
    auto result = aLhs;
    result.combine(aRhs);
    return result;
}

Transform& operator*=(Transform& aLhs, const Transform& aRhs) {
    return aLhs.combine(aRhs);
}

math::Vector2f operator*(const Transform& aLhs, const math::Vector2f& aRhs) {
    // TODO
    return {};
}

bool operator==(const Transform& aLhs, const Transform& aRhs) {
    return ToSf(aLhs) == ToSf(aRhs);
}

bool operator!=(const Transform& aLhs, const Transform& aRhs) {
    return !(aLhs == aRhs);
}

} // namespace gr
HOBGOBLIN_NAMESPACE_END

#include <Hobgoblin/Private/Pmacro_undef.hpp>
