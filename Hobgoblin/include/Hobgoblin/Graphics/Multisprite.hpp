#ifndef UHOBGOBLIN_GR_MULTISPRITE_HPP
#define UHOBGOBLIN_GR_MULTISPRITE_HPP

#include <Hobgoblin/Common.hpp>
#include <SFML/Graphics.hpp>

#include <variant>
#include <vector>

#include <Hobgoblin/Private/Pmacro_define.hpp>

HOBGOBLIN_NAMESPACE_START
namespace gr {

class Multisprite : public sf::Drawable, public sf::Transformable {
public:

    PZInteger getSubspriteCount();

    void draw(sf::RenderTarget& target, sf::RenderStates states) const override {}

private:
    std::variant<sf::Sprite, std::vector<sf::Sprite>> _subsprites;
    PZInteger _subspriteCount;
};

} // namespace gr
HOBGOBLIN_NAMESPACE_END

#include <Hobgoblin/Private/Pmacro_undef.hpp>
#include <Hobgoblin/Private/Short_namespace.hpp>

#endif // !UHOBGOBLIN_GR_MULTISPRITE_HPP