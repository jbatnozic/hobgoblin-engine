
#include <Hobgoblin/Graphics/Sprite_blueprint.hpp>

#include <Hobgoblin/Private/Pmacro_define.hpp>

HOBGOBLIN_NAMESPACE_BEGIN
namespace gr {

SpriteBlueprint::SpriteBlueprint(const Texture& aTexture, TextureRect aTextureRect)
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
