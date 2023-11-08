
#include <Hobgoblin/Graphics/View.hpp>
#include <SFML/Graphics/View.hpp>

#include <new>
#include <utility>

#include "SFML_conversions.hpp"

#include <Hobgoblin/Private/Pmacro_define.hpp>

HOBGOBLIN_NAMESPACE_BEGIN
namespace gr {

using ImplType = sf::View;
constexpr auto IMPL_SIZE  = sizeof(ImplType);
constexpr auto IMPL_ALIGN = alignof(ImplType);
#define  IMPLOF(_obj_) (reinterpret_cast<ImplType*>(&((_obj_)._storage)))
#define CIMPLOF(_obj_) (reinterpret_cast<const ImplType*>(&((_obj_)._storage)))
#define  SELF_IMPL (IMPLOF(SELF))
#define SELF_CIMPL (CIMPLOF(SELF))

View::View() {
    static_assert(STORAGE_SIZE  == IMPL_SIZE,  "View::STORAGE_SIZE is inadequate.");
    static_assert(STORAGE_ALIGN == IMPL_ALIGN, "View::STORAGE_ALIGN is inadequate.");

    new (&_storage) ImplType();
}

View::View(const View& aOther) {
    new (&_storage) ImplType(*CIMPLOF(aOther));
}

View& View::operator=(const View& aOther) {
    if (this != &aOther) {
        *SELF_IMPL = *CIMPLOF(aOther);
    }
    return SELF;
}

View::View(View&& aOther) noexcept {
    new (&_storage) ImplType(std::move(*IMPLOF(aOther)));
}

View& View::operator=(View&& aOther) noexcept {
    if (this != &aOther) {
        *SELF_IMPL = std::move(*IMPLOF(aOther));
    }
    return SELF;
}

View::View(const math::Rectangle<float>& aRectangle) {
    new (&_storage) ImplType(ToSf(aRectangle));
}

View::View(const math::Vector2f& aCenter, const math::Vector2f& aSize) {
    new (&_storage) ImplType(ToSf(aCenter), ToSf(aSize));
}

View::~View() {
    SELF_IMPL->~ImplType();
}

void View::setEnabled(bool aEnabled) {
    _enabled = aEnabled;
}

void View::setCenter(float aX, float aY) {
    SELF_IMPL->setCenter(aX, aY);
}

void View::setCenter(const math::Vector2f& aCenter) {
    SELF_IMPL->setCenter(aCenter.x, aCenter.y);
}

void View::setSize(float aWidth, float aHeight) {
    SELF_IMPL->setSize(aWidth, aHeight);
}

void View::setSize(const math::Vector2f& aSize) {
    SELF_IMPL->setSize(aSize.x, aSize.y);
}

void View::setRotation(float aAngle) {
    SELF_IMPL->setRotation(aAngle);
}

void View::setViewport(const math::Rectangle<float>& aViewport) {
    SELF_IMPL->setViewport({
        aViewport.getLeft(),
        aViewport.getTop(),
        aViewport.w,
        aViewport.h
    });
}

void View::reset(const math::Rectangle<float>& aRectangle) {
    SELF_IMPL->reset(ToSf(aRectangle));
}

bool View::isEnabled() const {
    return _enabled;
}

math::Vector2f View::getCenter() const {
    const auto center = SELF_CIMPL->getCenter();
    return {center.x, center.y};
}

math::Vector2f View::getSize() const {
    const auto size = SELF_CIMPL->getSize();
    return {size.x, size.y};
}

float View::getRotation() const {
    return SELF_CIMPL->getRotation();
}

const math::Rectangle<float> View::getViewport() const {
    const auto viewport = SELF_CIMPL->getViewport();
    return {viewport.top, viewport.left, viewport.width, viewport.height};
}

void View::move(float aOffsetX, float aOffsetY) {
    SELF_IMPL->move(aOffsetX, aOffsetY);
}

void View::move(const math::Vector2f& aOffset) {
    SELF_IMPL->move(aOffset.x, aOffset.y);
}

void View::rotate(float aAngle) {
    SELF_IMPL->rotate(aAngle);
}

void View::zoom(float aFactor) {
    SELF_IMPL->zoom(aFactor);
}

Transform View::getTransform() const {
    return ToHg(SELF_CIMPL->getTransform());
}

Transform View::getInverseTransform() const {
    return ToHg(SELF_CIMPL->getInverseTransform());
}

void* View::_getSFMLImpl() {
    return SELF_IMPL;
}

const void* View::_getSFMLImpl() const {
    return SELF_CIMPL;
}

} // namespace gr
HOBGOBLIN_NAMESPACE_END

#include <Hobgoblin/Private/Pmacro_undef.hpp>
