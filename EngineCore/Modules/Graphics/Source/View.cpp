
#include <Hobgoblin/Graphics/View.hpp>
#include <SFML/Graphics/View.hpp>

#include <new>

#include "SFML_conversions.hpp"

#include <Hobgoblin/Private/Pmacro_define.hpp>

HOBGOBLIN_NAMESPACE_BEGIN
namespace gr {

using ImplType = sf::View;
constexpr auto IMPL_SIZE  = sizeof(ImplType);
constexpr auto IMPL_ALIGN = alignof(ImplType);
#define  IMPL (reinterpret_cast<ImplType*>(&_storage))
#define CIMPL (reinterpret_cast<const ImplType*>(&_storage))

View::View() {
    static_assert(STORAGE_SIZE  == IMPL_SIZE,  "View::STORAGE_SIZE is inadequate.");
    static_assert(STORAGE_ALIGN == IMPL_ALIGN, "View::STORAGE_ALIGN is inadequate.");

    new (&_storage) ImplType();
}

View::View(const math::Rectangle<float>& aRectangle) {
    new (&_storage) ImplType(ToSf(aRectangle));
}

View::View(const math::Vector2f& aCenter, const math::Vector2f& aSize) {
    new (&_storage) ImplType(ToSf(aCenter), ToSf(aSize));
}

View::~View() {
    IMPL->~ImplType();
}

void View::setEnabled(bool aEnabled) {
    _enabled = aEnabled;
}

void View::setCenter(float aX, float aY) {
    IMPL->setCenter(aX, aY);
}

void View::setCenter(const math::Vector2f& aCenter) {
    IMPL->setCenter(aCenter.x, aCenter.y);
}

void View::setSize(float aWidth, float aHeight) {
    IMPL->setSize(aWidth, aHeight);
}

void View::setSize(const math::Vector2f& aSize) {
    IMPL->setSize(aSize.x, aSize.y);
}

void View::setRotation(float aAngle) {
    IMPL->setRotation(aAngle);
}

void View::setViewport(const math::Rectangle<float>& aViewport) {
    IMPL->setViewport({
        aViewport.getLeft(),
        aViewport.getTop(),
        aViewport.w,
        aViewport.h
    });
}

void View::reset(const math::Rectangle<float>& aRectangle) {
    // TODO
}

bool View::isEnabled() const {
    return _enabled;
}

const math::Vector2f& View::getCenter() const {
    const auto center = CIMPL->getCenter();
    return {center.x, center.y};
}

const math::Vector2f& View::getSize() const {
    const auto size = CIMPL->getSize();
    return {size.x, size.y};
}

float View::getRotation() const {
    return CIMPL->getRotation();
}

const math::Rectangle<float> View::getViewport() const {
    const auto viewport = CIMPL->getViewport();
    return {viewport.top, viewport.left, viewport.width, viewport.height};
}

void View::move(float aOffsetX, float aOffsetY) {
    IMPL->move(aOffsetX, aOffsetY);
}

void View::move(const math::Vector2f& aOffset) {
    IMPL->move(aOffset.x, aOffset.y);
}

void View::rotate(float aAngle) {
    IMPL->rotate(aAngle);
}

void View::zoom(float aFactor) {
    IMPL->zoom(aFactor);
}

//const Transform& View::getTransform() const {
//    // TODO
//}
//
//const Transform& View::getInverseTransform() const {
//    // TODO
//}

void* View::_getSFMLImpl() {
    return IMPL;
}

const void* View::_getSFMLImpl() const {
    return CIMPL;
}

} // namespace gr
HOBGOBLIN_NAMESPACE_END

#include <Hobgoblin/Private/Pmacro_undef.hpp>
