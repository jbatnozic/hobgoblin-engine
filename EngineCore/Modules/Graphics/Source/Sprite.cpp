
#include <Hobgoblin/Graphics/Sprite.hpp>

#include <SFML/Graphics/Sprite.hpp>
#include <Hobgoblin/Graphics/Texture.hpp>

#include "SFML_conversions.hpp"

#include <Hobgoblin/Private/Pmacro_define.hpp>

HOBGOBLIN_NAMESPACE_BEGIN
namespace gr {

using ImplType = sf::Sprite;
constexpr auto IMPL_SIZE  = sizeof(ImplType);
constexpr auto IMPL_ALIGN = alignof(ImplType);
#define  IMPLOF(_obj_) (reinterpret_cast<ImplType*>(&((_obj_)._storage)))
#define CIMPLOF(_obj_) (reinterpret_cast<const ImplType*>(&((_obj_)._storage)))
#define  SELF_IMPL (IMPLOF(SELF))
#define SELF_CIMPL (CIMPLOF(SELF))

Sprite::Sprite() {
    static_assert(STORAGE_SIZE  == IMPL_SIZE,  "Sprite::STORAGE_SIZE is inadequate.");
    static_assert(STORAGE_ALIGN == IMPL_ALIGN, "Sprite::STORAGE_ALIGN is inadequate.");

    new (&_storage) ImplType();
}

Sprite::~Sprite() {
    SELF_IMPL->~ImplType();
}

Sprite::Sprite(const Texture& aTexture) {
    // TODO
}

Sprite::Sprite(const Texture& aTexture, const math::Rectangle<int>& aRectangle) {
    // TODO
}

void Sprite::setTexture(const Texture& aTexture, bool aResetRect) {
    _texture = &aTexture;
    // SELF_IMPL->setTexture(ToSf(aTexture), aResetRect); TODO
}

void Sprite::setTextureRect(const math::Rectangle<int>& aRectangle) {
    SELF_IMPL->setTextureRect({
        aRectangle.getLeft(),
        aRectangle.getTop(),
        aRectangle.w,
        aRectangle.h
    });
}

void Sprite::setColor(const Color& aColor) {
    SELF_IMPL->setColor(ToSf(aColor));
}

const Texture* Sprite::getTexture() const {
    return _texture;
}

const math::Rectangle<int>& Sprite::getTextureRect() const {
    const auto rect = SELF_CIMPL->getTextureRect();
    return {rect.left, rect.top, rect.width, rect.height};
}

const Color& Sprite::getColor() const {
    return ToHg(SELF_CIMPL->getColor());
}

math::Rectangle<float> Sprite::getLocalBounds() const {
    const auto bounds = SELF_CIMPL->getLocalBounds();
    return {bounds.left, bounds.top, bounds.width, bounds.height};
}

math::Rectangle<float> Sprite::getGlobalBounds() const {
    const auto bounds = SELF_CIMPL->getGlobalBounds();
    return {bounds.left, bounds.top, bounds.width, bounds.height};
}

void Sprite::_draw(Canvas& aCanvas, const RenderStates& aStates) const {
    // TODO
}

Drawable::BatchingType Sprite::getBatchingType() const {
    return Drawable::BatchingType::Sprite;
}

void* Sprite::_getSFMLImpl() {
    return SELF_IMPL;
}

const void* Sprite::_getSFMLImpl() const {
    return SELF_CIMPL;
}

///////////////////////////////////////////////////////////////////////////
// SPRITE BLUEPRINT                                                      //
///////////////////////////////////////////////////////////////////////////

SpriteBlueprint::SpriteBlueprint(const Texture& aTexture, const math::Rectangle<int>& aTextureRect)
    : _texture{aTexture}
    , _textureRect{aTextureRect}
{
}

Sprite SpriteBlueprint::spr() const {
    return Sprite{_texture, _textureRect};
}

} // namespace gr
HOBGOBLIN_NAMESPACE_END

#include <Hobgoblin/Private/Pmacro_undef.hpp>
