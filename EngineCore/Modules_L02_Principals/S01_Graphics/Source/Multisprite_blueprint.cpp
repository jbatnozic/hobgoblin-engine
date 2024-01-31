
#include <Hobgoblin/Graphics/Multisprite_blueprint.hpp>
#include <Hobgoblin/HGExcept.hpp>

#include <Hobgoblin/Private/Pmacro_define.hpp>

HOBGOBLIN_NAMESPACE_BEGIN
namespace gr {

namespace {
void ApplyOffset(Multisprite& aMultisprite, const OriginOffset& aOffset) {
    switch (aOffset.kind) {
    case OriginOffset::RELATIVE_TO_TOP_LEFT:
        aMultisprite.setOrigin(aOffset.offset);
        break;

    case OriginOffset::RELATIVE_TO_CENTER:
        {
            const auto size = aMultisprite.getLocalBounds();
            aMultisprite.setOrigin(size.w / 2.f + aOffset.offset.x,
                                   size.h / 2.f + aOffset.offset.y);
        }
        break;

    default:
        HG_UNREACHABLE("Invalid value for OriginOffset::Kind ({}).", (int)aOffset.kind);
    }
}
} // namespace

// Construct with single subsprite
MultispriteBlueprint::MultispriteBlueprint(const Texture& aTexture,
                                           TextureRect aTextureRect, 
                                           std::optional<OriginOffset> aOriginOffset)
    : _texture{&aTexture}
    , _textureRects{aTextureRect}
    , _subspriteCount{1}
    , _offset{aOriginOffset}
{
}

MultispriteBlueprint::MultispriteBlueprint(const MultispriteBlueprint& aOther)
    : _texture{aOther._texture}
    , _textureRects{aOther._textureRects}
    , _subspriteCount{aOther._subspriteCount}
    , _offset{aOther._offset}
{
}

MultispriteBlueprint& MultispriteBlueprint::operator=(const MultispriteBlueprint& aOther) {
    if (this != &aOther) {
        SELF._texture = aOther._texture;
        SELF._textureRects = aOther._textureRects;
        SELF._subspriteCount = aOther._subspriteCount;
        SELF._offset = aOther._offset;
    }
    return SELF;
}

MultispriteBlueprint::MultispriteBlueprint(MultispriteBlueprint&& aOther)
    : _texture{aOther._texture}
    , _textureRects{std::move(aOther._textureRects)}
    , _subspriteCount{aOther._subspriteCount}
    , _offset{aOther._offset}
{
}

MultispriteBlueprint& MultispriteBlueprint::operator=(MultispriteBlueprint&& aOther) {
    if (this != &aOther) {
        SELF._texture = aOther._texture;
        SELF._textureRects = std::move(aOther._textureRects);
        SELF._subspriteCount = aOther._subspriteCount;
        SELF._offset = aOther._offset;
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
        auto result = Multisprite{_texture, subsprites.begin(), subsprites.end()};
        if (_offset.has_value()) {
            ApplyOffset(result, *_offset);
        }
        return result;
    }

    auto result = Multisprite{_texture, std::get<TextureRect>(_textureRects)};
    if (_offset.has_value()) {
        ApplyOffset(result, *_offset);
    }
    return result;
}

SpriteBlueprint MultispriteBlueprint::extractBlueprint(PZInteger aSubspriteIndex) const {
    HG_VALIDATE_ARGUMENT(aSubspriteIndex < _subspriteCount, "Subsprite index out of bounds.");

    if (_subspriteCount > 1) {
        return SpriteBlueprint{
            *_texture, 
            std::get<std::vector<TextureRect>>(_textureRects).at(pztos(aSubspriteIndex)),
            _offset
        };
    }

    return SpriteBlueprint{*_texture, std::get<TextureRect>(_textureRects), _offset};
}

bool MultispriteBlueprint::hasExplicitOrigin() const {
    return _offset.has_value();
}

} // namespace gr
HOBGOBLIN_NAMESPACE_END

#include <Hobgoblin/Private/Pmacro_undef.hpp>
