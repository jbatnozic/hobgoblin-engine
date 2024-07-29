// Copyright 2024 Jovan Batnozic. Released under MS-PL licence in Serbia.
// See https://github.com/jbatnozic/Hobgoblin?tab=readme-ov-file#licence

// clang-format off


#include <Hobgoblin/Graphics/Shape.hpp>

#include <Hobgoblin/Common.hpp>

#include <SFML/Graphics/Shape.hpp>
#include <SFML/Graphics/Texture.hpp>

#include <functional>
#include <new>
#include <utility>

#include "Draw_bridge.hpp"
#include "SFML_conversions.hpp"

#include <Hobgoblin/Private/Pmacro_define.hpp>

HOBGOBLIN_NAMESPACE_BEGIN
namespace gr {

namespace {
class SfShapeAdapter : public sf::Shape {
public:
    SfShapeAdapter(detail::ShapePolymorphismAdapter& aShapePolyAdapter)
        : _shapePolyAdapter{&aShapePolyAdapter}
    {
    }

    std::size_t getPointCount() const override {
        return pztos(_shapePolyAdapter->getPointCount());
    }

    sf::Vector2f getPoint(std::size_t aIndex) const override {
        const auto point = _shapePolyAdapter->getPoint(stopz(aIndex));
        return {point.x, point.y};
    }

    void update() {
        Shape::update();
    }

private:
    friend class ::jbatnozic::hobgoblin::gr::Shape;

    NeverNull<detail::ShapePolymorphismAdapter*> _shapePolyAdapter;
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

Shape::Shape(const Shape& aOther) {
    new (&_storage) ImplType(*CIMPLOF(aOther));
    SELF_IMPL->_shapePolyAdapter = this;
}

Shape& Shape::operator=(const Shape& aOther) {
    if (this != &aOther) {
        *SELF_IMPL = *CIMPLOF(aOther);
        SELF_IMPL->_shapePolyAdapter = this;
    }
    return SELF;
}

Shape::Shape(Shape&& aOther) {
    new (&_storage) ImplType(std::move(*IMPLOF(aOther)));
    SELF_IMPL->_shapePolyAdapter = this;
}

Shape& Shape::operator=(Shape&& aOther) {
    if (this != &aOther) {
        *SELF_IMPL = std::move(*IMPLOF(aOther));
        SELF_IMPL->_shapePolyAdapter = this;
    }
    return SELF;
}

Shape::~Shape() {
    SELF_IMPL->~SfShapeAdapter();
}

void Shape::setTexture(const Texture* aTexture, bool aResetRect) {
    if (aTexture == nullptr) {
        SELF_IMPL->setTexture(nullptr, aResetRect);
    }
    else {
        const auto& sfTexture = detail::GraphicsImplAccessor::getImplOf<sf::Texture>(*aTexture);
        SELF_IMPL->setTexture(&sfTexture, aResetRect);
    }
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

void Shape::_update() {
    SELF_IMPL->update();
}

///////////////////////////////////////////////////////////////////////////
// DRAWABLE                                                              //
///////////////////////////////////////////////////////////////////////////

Drawable::BatchingType Shape::getBatchingType() const {
    return BatchingType::Custom; // TODO(optimization: could maybe draw in terms of VertexArray?)
}

///////////////////////////////////////////////////////////////////////////
// TRANSFORMABLE                                                         //
///////////////////////////////////////////////////////////////////////////

void Shape::setPosition(float aX, float aY) {
    SELF_IMPL->setPosition(aX, aY);
}

void Shape::setPosition(const math::Vector2f& aPosition) {
    SELF_IMPL->setPosition(ToSf(aPosition));
}

void Shape::setRotation(math::AngleF aAngle) {
    SELF_IMPL->setRotation(-aAngle.asDeg());
}

void Shape::setScale(float aFactorX, float aFactorY) {
    SELF_IMPL->setScale(aFactorX, aFactorY);
}

void Shape::setScale(const math::Vector2f& aFactors) {
    SELF_IMPL->setScale(ToSf(aFactors));
}

void Shape::setOrigin(float aX, float aY) {
    SELF_IMPL->setOrigin(aX, aY);
}

void Shape::setOrigin(const math::Vector2f& aOrigin) {
    SELF_IMPL->setOrigin(ToSf(aOrigin));
}

math::Vector2f Shape::getPosition() const {
    return ToHg(SELF_CIMPL->getPosition());
}

math::AngleF Shape::getRotation() const {
    return math::AngleF::fromDegrees(-(SELF_CIMPL->getRotation()));
}

math::Vector2f Shape::getScale() const {
    return ToHg(SELF_CIMPL->getScale());
}

math::Vector2f Shape::getOrigin() const {
    return ToHg(SELF_CIMPL->getOrigin());
}

void Shape::move(float aOffsetX, float aOffsetY) {
    SELF_IMPL->move(aOffsetX, aOffsetY);
}

void Shape::move(const math::Vector2f& aOffset) {
    SELF_IMPL->move(ToSf(aOffset));
}

void Shape::rotate(math::AngleF aAngle) {
    SELF_IMPL->rotate(-aAngle.asDeg());
}

void Shape::scale(float aFactorX, float aFactorY) {
    SELF_IMPL->scale(aFactorX, aFactorY);
}

void Shape::scale(const math::Vector2f& aFactor) {
    SELF_IMPL->scale(ToSf(aFactor));
}

Transform Shape::getTransform() const {
    return ToHg(SELF_CIMPL->getTransform());
}

Transform Shape::getInverseTransform() const {
    return ToHg(SELF_CIMPL->getInverseTransform());
}

///////////////////////////////////////////////////////////////////////////
// PRIVATE                                                               //
///////////////////////////////////////////////////////////////////////////

void* Shape::_getSFMLImpl() {
    return SELF_IMPL;
}

const void* Shape::_getSFMLImpl() const {
    return SELF_CIMPL;
}

void Shape::_drawOnto(Canvas& aCanvas, const RenderStates& aStates) const {
    const auto drawingWasSuccessful = 
        Draw(aCanvas, [this, &aStates](sf::RenderTarget& aSfRenderTarget) {
        aSfRenderTarget.draw(*CIMPLOF(*this), ToSf(aStates));
    });
    assert(drawingWasSuccessful);
}

} // namespace gr
HOBGOBLIN_NAMESPACE_END

#include <Hobgoblin/Private/Pmacro_undef.hpp>

// clang-format on
