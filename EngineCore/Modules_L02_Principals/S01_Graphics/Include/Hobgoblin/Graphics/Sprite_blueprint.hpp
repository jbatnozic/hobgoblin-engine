#ifndef UHOBGOBLIN_GRAPHICS_SPRITE_BLUEPRINT_HPP
#define UHOBGOBLIN_GRAPHICS_SPRITE_BLUEPRINT_HPP

#include <Hobgoblin/Graphics/Sprite.hpp>
#include <Hobgoblin/Graphics/Origin_offset.hpp>
#include <Hobgoblin/Graphics/Texture_rect.hpp>

#include <optional>

#include <Hobgoblin/Private/Pmacro_define.hpp>

HOBGOBLIN_NAMESPACE_BEGIN
namespace gr {

class Texture;

class SpriteBlueprint {
public:
    SpriteBlueprint(const Texture& aTexture, 
                    TextureRect aTextureRect, 
                    std::optional<OriginOffset> aOriginOffset = {});

    //! Constructs a Sprite from the blueprint.
    Sprite spr() const;

    //! Checks whether the origin of the sprite was given explicitly (returns true in this case).
    //! If false is returned, the sprite will have the implicit/default origin (top-left of the image).
    bool hasExplicitOrigin() const;

private:
    const Texture& _texture;
    TextureRect _textureRect;
    std::optional<OriginOffset> _offset;
};

} // namespace gr
HOBGOBLIN_NAMESPACE_END

#include <Hobgoblin/Private/Pmacro_undef.hpp>
#include <Hobgoblin/Private/Short_namespace.hpp>

#endif // !UHOBGOBLIN_GRAPHICS_SPRITE_BLUEPRINT_HPP
