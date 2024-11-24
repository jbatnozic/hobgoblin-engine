// Copyright 2024 Jovan Batnozic. Released under MS-PL licence in Serbia.
// See https://github.com/jbatnozic/Hobgoblin?tab=readme-ov-file#licence

// clang-format off

#include <Hobgoblin/Graphics/Canvas.hpp>
#include <Hobgoblin/Graphics/Multisprite.hpp>
#include <Hobgoblin/Graphics/Vertex.hpp>
#include <Hobgoblin/HGExcept.hpp>
#include <Hobgoblin/Math/Vector.hpp>

#include <cmath>
#include <cstring>

#include "SFML_conversions.hpp"

#include <Hobgoblin/Private/Pmacro_define.hpp>

HOBGOBLIN_NAMESPACE_BEGIN
namespace gr {

///////////////////////////////////////////////////////////////////////////
// MULTISPRITE                                                           //
///////////////////////////////////////////////////////////////////////////

Multisprite::Multisprite()
    : _texture{nullptr}
{
}

Multisprite::Multisprite(const Texture* aTexture)
    : _texture{aTexture}
{
}

Multisprite::Multisprite(const Texture* aTexture, TextureRect aTextureRect)
    : _texture{aTexture}
{
    addSubsprite(aTextureRect);
}

Multisprite::BatchingType Multisprite::getBatchingType() const {
    return BatchingType::Aggregate;
}

void Multisprite::setTexture(const Texture* aTexture) {
    _texture = aTexture;
}

const Texture* Multisprite::getTexture() const {
    return _texture;
}

///////////////////////////////////////////////////////////////////////////
// SUBSPRITES                                                            //
///////////////////////////////////////////////////////////////////////////

void Multisprite::addSubsprite(TextureRect aTextureRect) {
    _subsprites.emplace_back(aTextureRect);
}

void Multisprite::removeSubsprite(PZInteger aSubspriteIndex) {
    const auto subspriteCount = getSubspriteCount();

    HG_VALIDATE_ARGUMENT(aSubspriteIndex >= 0 && aSubspriteIndex < subspriteCount,
                         "Subsprite index ({}) out of bounds.", aSubspriteIndex);

    _subsprites.erase(_subsprites.begin() + aSubspriteIndex);
}

PZInteger Multisprite::getSubspriteCount() const {
    return stopz(_subsprites.size());
}

float Multisprite::getSubspriteSelector() const {
    return _subspriteSelector;
}

PZInteger Multisprite::getCurrentSubspriteIndex() const {
    const auto subspriteCount = getSubspriteCount();

    HG_VALIDATE_PRECONDITION(subspriteCount > 0);

    float temp = _subspriteSelector;
    while (temp < 0.f) {
        temp += static_cast<float>(subspriteCount);
    }
    return ToPz(std::trunc(temp)) % subspriteCount;
}

void Multisprite::selectSubsprite(int aSubspriteIndex) {
    _subspriteSelector = static_cast<float>(aSubspriteIndex);
}

void Multisprite::selectSubsprite(float aSubspriteIndex) {
    _subspriteSelector = aSubspriteIndex;
}

void Multisprite::advanceSubsprite(int aSubspriteCount) {
    _subspriteSelector += static_cast<float>(aSubspriteCount);
}

void Multisprite::advanceSubsprite(float aSubspriteCount) {
    _subspriteSelector += aSubspriteCount;
}

Sprite Multisprite::extractSubsprite(PZInteger aSubspriteIndex) const {
    const auto subspriteCount = getSubspriteCount();

    HG_VALIDATE_ARGUMENT(aSubspriteIndex >= 0 && aSubspriteIndex < subspriteCount,
                         "Subsprite index ({}) out of bounds.", aSubspriteIndex);

    const auto& subsprite =  _subsprites[pztos(aSubspriteIndex)];

    return Sprite{_texture, subsprite.textureRect};
}

///////////////////////////////////////////////////////////////////////////
// COLOUR                                                                //
///////////////////////////////////////////////////////////////////////////

void Multisprite::setColor(Color aColor) {
    _color = aColor;
}

Color Multisprite::getColor() const {
    return _color;
}

///////////////////////////////////////////////////////////////////////////
// BOUNDS                                                                //
///////////////////////////////////////////////////////////////////////////

math::Rectangle<float> Multisprite::getLocalBounds(PZInteger aSubspriteIndex) const {
    const auto subspriteCount = getSubspriteCount();

    HG_VALIDATE_ARGUMENT(aSubspriteIndex >= 0 && aSubspriteIndex < subspriteCount,
                         "Subsprite index ({}) out of bounds.", aSubspriteIndex);

    return _subsprites[pztos(aSubspriteIndex)].getLocalBounds();
}

math::Rectangle<float> Multisprite::getLocalBounds() const {
    math::Rectangle<float> rect; // default-initializes to all zeros

    if (!_subsprites.empty()) {
        rect = _subsprites[pztos(getCurrentSubspriteIndex())].getLocalBounds();
    }

    return rect;
}

math::Rectangle<float> Multisprite::getGlobalBounds(PZInteger aSubspriteIndex) const {
    const auto subspriteCount = getSubspriteCount();

    HG_VALIDATE_ARGUMENT(aSubspriteIndex >= 0 && aSubspriteIndex < subspriteCount,
                         "Subsprite index ({}) out of bounds.", aSubspriteIndex);

    return getTransform().transformRect(
        _subsprites[pztos(aSubspriteIndex)].getLocalBounds()
    );
}

math::Rectangle<float> Multisprite::getGlobalBounds() const {
    math::Rectangle<float> rect; // default-initializes to all zeros

    if (!_subsprites.empty()) {
        rect = _subsprites[pztos(getCurrentSubspriteIndex())].getLocalBounds();
    }

    return getTransform().transformRect(rect);
}

bool Multisprite::isNormalized() const {
    const auto subspriteCount = getSubspriteCount();
    const auto* subsprites = _subsprites.data();

    for (PZInteger i = 1; i < subspriteCount; i += 1) {
        if (subsprites[i].getLocalBounds() != subsprites[i - 1].getLocalBounds()) {
            return false;
        }
    }

    return true;
}

bool Multisprite::areAllSubspritesOfSameSize() const {
    return isNormalized();
}

///////////////////////////////////////////////////////////////////////////
// TRANSFORMABLE                                                         //
///////////////////////////////////////////////////////////////////////////

void Multisprite::setPosition(float aX, float aY) {
    _transformableData.setPosition(aX, aY);
}

void Multisprite::setPosition(const math::Vector2f& aPosition) {
    _transformableData.setPosition(aPosition);
}

void Multisprite::setRotation(math::AngleF aAngle) {
    _transformableData.setRotation(-aAngle);
}

void Multisprite::setScale(float aFactorX, float aFactorY) {
    _transformableData.setScale(aFactorX, aFactorY);
}

void Multisprite::setScale(const math::Vector2f& aFactors) {
    _transformableData.setScale(aFactors);
}

void Multisprite::setOrigin(float aX, float aY) {
    _transformableData.setOrigin(aX, aY);
}

void Multisprite::setOrigin(const math::Vector2f& aOrigin) {
    _transformableData.setOrigin(ToSf(aOrigin));
}

math::Vector2f Multisprite::getPosition() const {
    return _transformableData.getPosition();
}

math::AngleF Multisprite::getRotation() const {
    return -_transformableData.getRotation();
}

math::Vector2f Multisprite::getScale() const {
    return _transformableData.getScale();
}

math::Vector2f Multisprite::getOrigin() const {
    return _transformableData.getOrigin();
}

void Multisprite::move(float aOffsetX, float aOffsetY) {
    _transformableData.move(aOffsetX, aOffsetY);
}

void Multisprite::move(const math::Vector2f& aOffset) {
    _transformableData.move(aOffset);
}

void Multisprite::rotate(math::AngleF aAngle) {
    _transformableData.rotate(aAngle);
}

void Multisprite::scale(float aFactorX, float aFactorY) {
    _transformableData.scale(aFactorX, aFactorY);
}

void Multisprite::scale(const math::Vector2f& aFactor) {
    _transformableData.scale(aFactor);
}

Transform Multisprite::getTransform() const {
    return _transformableData.getTransform();
}

Transform Multisprite::getInverseTransform() const {
    return _transformableData.getInverseTransform();
}

///////////////////////////////////////////////////////////////////////////
// SUBSPRITE                                                             //
///////////////////////////////////////////////////////////////////////////

// FIRST: top-left, top-right, bottom-left
//  THEN: top-right, bottom-right, bottom-left

Multisprite::Subsprite::Subsprite(TextureRect aTextureRect)
    : textureRect{aTextureRect}
{
    // World positions
    {
        const auto bounds = getLocalBounds();

        vertices[0].position = math::Vector2f{0.f, 0.f};
        vertices[1].position = math::Vector2f{bounds.w, 0.f};
        vertices[2].position = math::Vector2f{0.f, bounds.h};
        vertices[3].position = math::Vector2f{bounds.w, 0.f};
        vertices[4].position = math::Vector2f{bounds.w, bounds.h};
        vertices[5].position = math::Vector2f{0.f, bounds.h};
    }

    // Texture positions
    {
        const float left   = static_cast<float>(aTextureRect.getLeft());
        const float right  = static_cast<float>(aTextureRect.getRight());
        const float top    = static_cast<float>(aTextureRect.getTop());
        const float bottom = static_cast<float>(aTextureRect.getBottom());

        vertices[0].texCoords = math::Vector2f{left, top};
        vertices[1].texCoords = math::Vector2f{right, top};
        vertices[2].texCoords = math::Vector2f{left, bottom};
        vertices[3].texCoords = math::Vector2f{right, top};
        vertices[4].texCoords = math::Vector2f{right, bottom};
        vertices[5].texCoords = math::Vector2f{left, bottom};
    }
}

math::Rectangle<float> Multisprite::Subsprite::getLocalBounds() const {
    return {
        0.f,
        0.f,
        static_cast<float>(textureRect.w),
        static_cast<float>(textureRect.h)
    };
}

///////////////////////////////////////////////////////////////////////////
// PROTECTED/PRIVATE                                                     //
///////////////////////////////////////////////////////////////////////////

void Multisprite::_drawOnto(Canvas& aCanvas, const RenderStates& aStates) const {
    const auto subspriteCount = getSubspriteCount();

    if (subspriteCount == 0 || _texture == nullptr) {
        return;
    }

    const auto& subspr =  _subsprites[pztos(getCurrentSubspriteIndex())];

    // Prepare vertices
    static constexpr PZInteger VERTEXT_COUNT = 6;
    Vertex vertices[VERTEXT_COUNT];
    std::memcpy(vertices, subspr.vertices, sizeof(Vertex) * VERTEXT_COUNT);

    const auto transform = getTransform();
    const auto& sfTransform = ToSf(transform);
    for (auto& vertex : vertices) {
        vertex.color = _color;
        vertex.position = transform.transformPoint(vertex.position);
    }

    // Prepare render states
    RenderStates statesCopy{aStates};
    statesCopy.texture = _texture;

    aCanvas.draw(vertices, VERTEXT_COUNT, PrimitiveType::TRIANGLES, statesCopy);
}

} // namespace gr
HOBGOBLIN_NAMESPACE_END

#include <Hobgoblin/Private/Pmacro_undef.hpp>

// clang-format on
