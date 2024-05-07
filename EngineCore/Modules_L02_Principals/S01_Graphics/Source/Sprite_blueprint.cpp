// Copyright 2024 Jovan Batnozic. Released under MS-PL licence in Serbia.
// See https://github.com/jbatnozic/Hobgoblin?tab=readme-ov-file#licence

// clang-format off


#include <Hobgoblin/Graphics/Sprite_blueprint.hpp>

#include <Hobgoblin/HGExcept.hpp>

#include <Hobgoblin/Private/Pmacro_define.hpp>

HOBGOBLIN_NAMESPACE_BEGIN
namespace gr {

SpriteBlueprint::SpriteBlueprint(const Texture& aTexture,
                                 TextureRect aTextureRect,
                                 std::optional<OriginOffset> aOriginOffset)
    : _texture{aTexture}
    , _textureRect{aTextureRect}
    , _offset{aOriginOffset}
{
}

Sprite SpriteBlueprint::spr() const {
    auto result = Sprite{&_texture, _textureRect};

    if (_offset.has_value()) {
        switch (_offset->kind) {
        case OriginOffset::RELATIVE_TO_TOP_LEFT:
            result.setOrigin(_offset->offset);
            break;

        case OriginOffset::RELATIVE_TO_CENTER:
            {
                const auto size = result.getLocalBounds();
                result.setOrigin(size.w / 2.f + _offset->offset.x,
                                 size.h / 2.f + _offset->offset.y);
            }
            break;

        default:
            HG_UNREACHABLE("Invalid value for OriginOffset::Kind ({}).", (int)_offset->kind);
        }
    }

    return result;
}

bool SpriteBlueprint::hasExplicitOrigin() const {
    return _offset.has_value();
}

} // namespace gr
HOBGOBLIN_NAMESPACE_END

#include <Hobgoblin/Private/Pmacro_undef.hpp>

// clang-format on
