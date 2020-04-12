#ifndef UHOBGOBLIN_GR_MULTISPRITE_HPP
#define UHOBGOBLIN_GR_MULTISPRITE_HPP

#include <Hobgoblin/Common.hpp>
#include <Hobgoblin/Utility/Rectangle.hpp>
#include <SFML/Graphics.hpp>

#include <variant>
#include <vector>

#include <Hobgoblin/Private/Pmacro_define.hpp>

HOBGOBLIN_NAMESPACE_START
namespace gr {

class Multisprite : public sf::Drawable, public sf::Transformable {
public:
    void setSubspriteIndex(PZInteger subspriteIndex);
    void setColor(const sf::Color& color);
    sf::Color getColor() const;

    void addSubsprite(sf::Texture& texture, util::Rectangle<PZInteger>& textureRect);
    PZInteger getSubspriteCount() const;

private:
    std::variant<sf::Sprite, std::vector<sf::Sprite>> _subsprites;
    PZInteger _subspriteCount = 0;
    PZInteger _subspriteIndex = 0;
    sf::Color _color;

    void draw(sf::RenderTarget& target, sf::RenderStates states) const override;
    const sf::Sprite* addressOfFirstSprite() const;
};

} // namespace gr
HOBGOBLIN_NAMESPACE_END

#include <Hobgoblin/Private/Pmacro_undef.hpp>
#include <Hobgoblin/Private/Short_namespace.hpp>

#endif // !UHOBGOBLIN_GR_MULTISPRITE_HPP