
#include <Hobgoblin/Graphics/Canvas.hpp>
#include <Hobgoblin/Graphics/Multisprite.hpp>
#include <Hobgoblin/Graphics/Vertex.hpp>
#include <Hobgoblin/Math/Vector.hpp>

#include <cassert>
#include <cmath>
#include <cstring>

#include "SFML_conversions.hpp"

#include <Hobgoblin/Private/Pmacro_define.hpp>

HOBGOBLIN_NAMESPACE_BEGIN
namespace gr {

///////////////////////////////////////////////////////////////////////////
// MULTISPRITE                                                           //
///////////////////////////////////////////////////////////////////////////

Multisprite::Multisprite(const Texture& aTexture)
    : _texture{aTexture}
{
}

Multisprite::Multisprite(const Texture& aTexture, TextureRect aTextureRect)
    : _texture{aTexture}
{
    addSubsprite(aTextureRect);
}

Multisprite::BatchingType Multisprite::getBatchingType() const {
    return BatchingType::Custom; // TODO - enable batching!
}

const Texture& Multisprite::getTexture() const {
    return _texture;
}

///////////////////////////////////////////////////////////////////////////
// SUBSPRITES                                                            //
///////////////////////////////////////////////////////////////////////////

void Multisprite::addSubsprite(TextureRect aTextureRect) {
    if (_subspriteCount == 0) {
        _subsprites.emplace<Subsprite>(aTextureRect);
    }
    else if (_subspriteCount == 1) {
        const auto subsprite0 = *_firstSubspritePtr();
        _subsprites = std::vector<Subsprite>{};
        std::get<std::vector<Subsprite>>(_subsprites).push_back(subsprite0);
        std::get<std::vector<Subsprite>>(_subsprites).emplace_back(aTextureRect);
    }
    else {
        std::get<std::vector<Subsprite>>(_subsprites).emplace_back(aTextureRect);
    }

    _subspriteCount += 1;
}

void Multisprite::removeSubsprite(PZInteger aSubspriteIndex) {
    if (aSubspriteIndex < 0 || aSubspriteIndex >= _subspriteCount) {
        throw TracedLogicError{"Multisprite - Subsprite index out of bounds!"};
    }

    if (_subspriteCount > 2) {
        auto& vec = std::get<std::vector<Subsprite>>(_subsprites);
        vec.erase(vec.begin() + aSubspriteIndex);
        _subspriteCount -= 1;
    }
    else if (_subspriteCount == 2) {
        auto& vec = std::get<std::vector<Subsprite>>(_subsprites);
        vec.erase(vec.begin() + aSubspriteIndex);

        const Subsprite temp = vec.front();
        _subsprites = temp;

        _subspriteCount = 1;
    }
    else /* count = 1 */ {
        _subspriteCount = 0; // There's not really anything to delete so we just mark it as empty
    }
}

PZInteger Multisprite::getSubspriteCount() const {
    return _subspriteCount;
}

float Multisprite::getSubspriteSelector() const {
    return _subspriteSelector;
}

PZInteger Multisprite::getCurrentSubspriteIndex() const {
    if (_subspriteCount == 0) {
        throw TracedLogicError{"Multisprite - No subsprites have been added!"};
    }

    float temp = _subspriteSelector;
    while (temp < 0.f) {
        temp += static_cast<float>(_subspriteCount);
    }
    return ToPz(std::trunc(temp)) % _subspriteCount;
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
    if (aSubspriteIndex < 0 || aSubspriteIndex >= _subspriteCount) {
        throw TracedLogicError{"Multisprite - Subsprite index out of bounds!"};
    }

    const auto& subsprite = *(_firstSubspritePtr() + aSubspriteIndex);

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
    if (aSubspriteIndex < 0 || aSubspriteIndex >= _subspriteCount) {
        throw TracedLogicError{"Multisprite - Subsprite index out of bounds!"};
    }

    return _firstSubspritePtr()[aSubspriteIndex].getLocalBounds();
}

math::Rectangle<float> Multisprite::getLocalBounds() const {
    math::Rectangle<float> rect; // default-initializes to all zeros

    if (_subspriteCount > 0) {
        rect = _firstSubspritePtr()[getCurrentSubspriteIndex()].getLocalBounds();
    }

    return rect;
}

math::Rectangle<float> Multisprite::getGlobalBounds(PZInteger aSubspriteIndex) const {
    if (aSubspriteIndex < 0 || aSubspriteIndex >= _subspriteCount) {
        throw TracedLogicError{"Multisprite - Subsprite index out of bounds!"};
    }

    return getTransform().transformRect(
        _firstSubspritePtr()[aSubspriteIndex].getLocalBounds()
    );
}

math::Rectangle<float> Multisprite::getGlobalBounds() const {
    math::Rectangle<float> rect; // default-initializes to all zeros

    if (_subspriteCount > 0) {
        rect = _firstSubspritePtr()[getCurrentSubspriteIndex()].getLocalBounds();
    }

    return getTransform().transformRect(rect);
}

bool Multisprite::isNormalized() const {
    const auto* subspsrites = _firstSubspritePtr();

    for (PZInteger i = 1; i < getSubspriteCount(); i += 1) {
        if (subspsrites[i].getLocalBounds() != subspsrites[i - 1].getLocalBounds()) {
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
    _transformableData.setRotation(aAngle);
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
    return _transformableData.getRotation();
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

Multisprite::Subsprite::Subsprite(TextureRect aTextureRect)
    : textureRect{aTextureRect}
{
    // World positions
    {
        const auto bounds = getLocalBounds();

        vertices[0].position = math::Vector2f{0.f, 0.f};
        vertices[1].position = math::Vector2f{0.f, bounds.h};
        vertices[2].position = math::Vector2f{bounds.w, 0.f};
        vertices[3].position = math::Vector2f{bounds.w, bounds.h};
    }

    // Texture positions
    {
        const float left   = static_cast<float>(aTextureRect.getLeft());
        const float right  = static_cast<float>(aTextureRect.getRight());
        const float top    = static_cast<float>(aTextureRect.getTop());
        const float bottom = static_cast<float>(aTextureRect.getBottom());

        vertices[0].texCoords = math::Vector2f{left, top};
        vertices[1].texCoords = math::Vector2f{left, bottom};
        vertices[2].texCoords = math::Vector2f{right, top};
        vertices[3].texCoords = math::Vector2f{right, bottom};
    }
}

math::Rectangle<float> Multisprite::Subsprite::getLocalBounds() const {
    return {
        0.f,
        0.f,
        static_cast<float>(std::abs(textureRect.w)),
        static_cast<float>(std::abs(textureRect.h))
    };
}

///////////////////////////////////////////////////////////////////////////
// PROTECTED/PRIVATE                                                     //
///////////////////////////////////////////////////////////////////////////

void Multisprite::_draw(Canvas& aCanvas, const RenderStates& aStates) const {
    if (_subspriteCount == 0) {
        return;
    }

    const auto& subspr = *(_firstSubspritePtr() + getCurrentSubspriteIndex());

    // Prepare vertices
    Vertex vertices[4];
    std::memcpy(vertices, subspr.vertices, sizeof(Vertex) * 4);
    vertices[0].color = vertices[1].color = vertices[2].color = vertices[3].color = _color;

    // Prepare render states
    RenderStates statesCopy{aStates};
    statesCopy.transform *= getTransform();
    statesCopy.texture = &_texture;

    aCanvas.draw(vertices, 4, PrimitiveType::TriangleStrip, statesCopy);
}

Multisprite::Subsprite* Multisprite::_firstSubspritePtr() {
    if (_subspriteCount <= 1) {
        return std::addressof(std::get<Subsprite>(_subsprites));
    }
    else {
        return std::get<std::vector<Subsprite>>(_subsprites).data();
    }
}

const Multisprite::Subsprite* Multisprite::_firstSubspritePtr() const {
    if (_subspriteCount <= 1) {
        return std::addressof(std::get<Subsprite>(_subsprites));
    }
    else {
        return std::get<std::vector<Subsprite>>(_subsprites).data();
    }
}

} // namespace gr
HOBGOBLIN_NAMESPACE_END

#include <Hobgoblin/Private/Pmacro_undef.hpp>