
#include <Hobgoblin/Graphics/Shape.hpp>

#include <SFML/Graphics/Shape.hpp>

#include <functional>
#include <new>
#include <utility>

#include "SFML_conversions.hpp"

#include <Hobgoblin/Private/Pmacro_define.hpp>

HOBGOBLIN_NAMESPACE_BEGIN
namespace gr {

namespace {
class SfShapeAdapter : public sf::Shape {
public:
    SfShapeAdapter(detail::ShapePolymorphismAdapter& aShapePolyAdapter)
        : _shapePolyAdapter{aShapePolyAdapter}
    {
    }

    std::size_t getPointCount() const override {
        return pztos(_shapePolyAdapter.getPointCount());
    }

    sf::Vector2f getPoint(std::size_t aIndex) const override {
        const auto point = _shapePolyAdapter.getPoint(stopz(aIndex));
        return {point.x, point.y};
    }

    void update() {
        Shape::update();
    }

private:
    detail::ShapePolymorphismAdapter& _shapePolyAdapter;
};
} // namespace

using ImplType = SfShapeAdapter;
constexpr auto IMPL_SIZE  = sizeof(ImplType);
constexpr auto IMPL_ALIGN = alignof(ImplType);
#define  IMPLOF(_obj_) (reinterpret_cast<ImplType*>(&((_obj_)._storage)))
#define CIMPLOF(_obj_) (reinterpret_cast<const ImplType*>(&((_obj_)._storage)))
#define  SELF_IMPL (IMPLOF(SELF))
#define SELF_CIMPL (CIMPLOF(SELF))

Shape::Shape() {
    static_assert(STORAGE_SIZE  == IMPL_SIZE,  "Shape::STORAGE_SIZE is inadequate.");
    static_assert(STORAGE_ALIGN == IMPL_ALIGN, "Shape::STORAGE_ALIGN is inadequate.");

    new (&_storage) ImplType(SELF);
}

Shape::~Shape() {
    SELF_IMPL->~SfShapeAdapter();
}

void Shape::setTexture(const Texture* aTexture, bool aResetRect) {
    // TODO
}

void Shape::setTextureRect(const math::Rectangle<PZInteger>& aRect) {
    SELF_IMPL->setTextureRect({
        aRect.getLeft(),
        aRect.getTop(),
        aRect.w,
        aRect.h
    });
}

void Shape::setFillColor(const Color& aColor) {
    SELF_IMPL->setFillColor(ToSf(aColor));
}

void Shape::setOutlineColor(const Color& aColor) {
    SELF_IMPL->setOutlineColor(ToSf(aColor));
}

void Shape::setOutlineThickness(float aThickness) {
    SELF_IMPL->setOutlineThickness(aThickness);
}

const Texture* Shape::getTexture() const {
    return _texture;
}

math::Rectangle<PZInteger> Shape::getTextureRect() const {
    const auto rect = SELF_CIMPL->getTextureRect();
    return {
        rect.left,
        rect.top,
        rect.width,
        rect.height
    };
}

Color Shape::getFillColor() const {
    return ToHg(SELF_CIMPL->getFillColor());
}

Color Shape::getOutlineColor() const {
    return ToHg(SELF_CIMPL->getOutlineColor());
}

float Shape::getOutlineThickness() const {
    return SELF_CIMPL->getOutlineThickness();
}

math::Rectangle<float> Shape::getLocalBounds() const {
    const auto bounds = SELF_CIMPL->getLocalBounds();
    return {
        bounds.left,
        bounds.top,
        bounds.width,
        bounds.height
    };
}

math::Rectangle<float> Shape::getGlobalBounds() const {
    const auto bounds = SELF_CIMPL->getGlobalBounds();
    return {
        bounds.left,
        bounds.top,
        bounds.width,
        bounds.height
    };
}

void Shape::update() {
    SELF_IMPL->update();
}

void* Shape::_getSFMLImpl() {
    return SELF_IMPL;
}

const void* Shape::_getSFMLImpl() const {
    return SELF_CIMPL;
}

} // namespace gr
HOBGOBLIN_NAMESPACE_END

#include <Hobgoblin/Private/Pmacro_undef.hpp>
