// Copyright 2024 Jovan Batnozic. Released under MS-PL licence in Serbia.
// See https://github.com/jbatnozic/Hobgoblin?tab=readme-ov-file#licence

// clang-format off


#include <Hobgoblin/Graphics/Multisprite_blueprint.hpp>
#include <Hobgoblin/HGExcept.hpp>

#include <Hobgoblin/Private/Pmacro_define.hpp>

HOBGOBLIN_NAMESPACE_BEGIN
namespace gr {

// Construct with single subsprite
MultispriteBlueprint::MultispriteBlueprint(const Texture& aTexture, TextureRect aTextureRect)
    : _texture{&aTexture}
    , _textureRects{aTextureRect}
    , _subspriteCount{1}
{
}

MultispriteBlueprint::MultispriteBlueprint(const MultispriteBlueprint& aOther)
    : _texture{aOther._texture}
    , _textureRects{aOther._textureRects}
    , _subspriteCount{aOther._subspriteCount}
{
}

MultispriteBlueprint& MultispriteBlueprint::operator=(const MultispriteBlueprint& aOther) {
    if (this != &aOther) {
        SELF._texture = aOther._texture;
        SELF._textureRects = aOther._textureRects;
        SELF._subspriteCount = aOther._subspriteCount;
    }
    return SELF;
}

MultispriteBlueprint::MultispriteBlueprint(MultispriteBlueprint&& aOther)
    : _texture{aOther._texture}
    , _textureRects{std::move(aOther._textureRects)}
    , _subspriteCount{aOther._subspriteCount}
{
}

MultispriteBlueprint& MultispriteBlueprint::operator=(MultispriteBlueprint&& aOther) {
    if (this != &aOther) {
        SELF._texture = aOther._texture;
        SELF._textureRects = std::move(aOther._textureRects);
        SELF._subspriteCount = aOther._subspriteCount;
    }
    return SELF;
}

PZInteger MultispriteBlueprint::getSubspriteCount() const {
    return _subspriteCount;
}

Sprite MultispriteBlueprint::subspr(PZInteger aSubspriteIndex) const {
    return extractBlueprint(aSubspriteIndex).spr();
}

Multisprite MultispriteBlueprint::multispr() const {
    if (_subspriteCount > 1) {
        const auto& subsprites = std::get<std::vector<TextureRect>>(_textureRects);
        return Multisprite{_texture, subsprites.begin(), subsprites.end()};
    }

    return Multisprite{_texture, std::get<TextureRect>(_textureRects)};
}

SpriteBlueprint MultispriteBlueprint::extractBlueprint(PZInteger aSubspriteIndex) const {
    HG_VALIDATE_ARGUMENT(aSubspriteIndex < _subspriteCount, "Subsprite index out of bounds.");

    if (_subspriteCount > 1) {
        return SpriteBlueprint{
            *_texture, 
            std::get<std::vector<TextureRect>>(_textureRects).at(pztos(aSubspriteIndex))
        };
    }

    return SpriteBlueprint{*_texture, std::get<TextureRect>(_textureRects)};
}

} // namespace gr
HOBGOBLIN_NAMESPACE_END

#include <Hobgoblin/Private/Pmacro_undef.hpp>

// clang-format on
