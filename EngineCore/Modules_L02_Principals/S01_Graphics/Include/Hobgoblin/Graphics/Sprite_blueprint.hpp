// Copyright 2024 Jovan Batnozic. Released under MS-PL licence in Serbia.
// See https://github.com/jbatnozic/Hobgoblin?tab=readme-ov-file#licence

// clang-format off

#ifndef UHOBGOBLIN_GRAPHICS_SPRITE_BLUEPRINT_HPP
#define UHOBGOBLIN_GRAPHICS_SPRITE_BLUEPRINT_HPP

#include <Hobgoblin/Graphics/Sprite.hpp>
#include <Hobgoblin/Graphics/Texture_rect.hpp>

#include <Hobgoblin/Private/Pmacro_define.hpp>

HOBGOBLIN_NAMESPACE_BEGIN
namespace gr {

class Texture;

class SpriteBlueprint {
public:
    SpriteBlueprint(const Texture& aTexture, TextureRect aTextureRect);

    //! Constructs a Sprite from the blueprint.
    Sprite spr() const;

private:
    const Texture& _texture;
    TextureRect _textureRect;
};

} // namespace gr
HOBGOBLIN_NAMESPACE_END

#include <Hobgoblin/Private/Pmacro_undef.hpp>
#include <Hobgoblin/Private/Short_namespace.hpp>

#endif // !UHOBGOBLIN_GRAPHICS_SPRITE_BLUEPRINT_HPP

// clang-format on
