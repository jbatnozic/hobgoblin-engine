#ifndef UHOBGOBLIN_GR_SPRITE_HPP
#define UHOBGOBLIN_GR_SPRITE_HPP

#include <SFML/Graphics.hpp>

#include <Hobgoblin/Private/Pmacro_define.hpp>

HOBGOBLIN_NAMESPACE_BEGIN
namespace gr {

using Sprite = sf::Sprite;

class SpriteBlueprint {
public:
    SpriteBlueprint(const sf::Texture& aTexture, const sf::IntRect& aTextureRect);

    //! Constructs a Sprite from the blueprint.
    Sprite spr() const;

private:
    const sf::Texture& _texture;
    sf::IntRect _textureRect;
};

} // namespace gr
HOBGOBLIN_NAMESPACE_END

#include <Hobgoblin/Private/Pmacro_undef.hpp>
#include <Hobgoblin/Private/Short_namespace.hpp>

#endif // !UHOBGOBLIN_GR_SPRITE_HPP
