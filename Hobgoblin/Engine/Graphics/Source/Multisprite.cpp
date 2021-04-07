
#include <Hobgoblin/Graphics/Multisprite.hpp>

#include <cassert>

#include <Hobgoblin/Private/Pmacro_define.hpp>

HOBGOBLIN_NAMESPACE_START
namespace gr {

void Multisprite::setSubspriteIndex(PZInteger subspriteIndex) {
    assert(subspriteIndex < _subspriteCount);
    _subspriteIndex = subspriteIndex;
}

void Multisprite::setColor(const sf::Color& color) {
    _color = color;
}

sf::Color Multisprite::getColor() const {
    return _color;
}

void Multisprite::addSubsprite(sf::Texture& texture, util::Rectangle<PZInteger>& textureRect) {
    sf::Sprite newSubsprite(texture, sf::IntRect(textureRect.x, textureRect.y, textureRect.w, textureRect.h));
    
    if (_subspriteCount == 0) {
        _subsprites = newSubsprite;
    }
    else if (_subspriteCount == 1) {
        sf::Sprite subsprite0 = *addressOfFirstSprite();
        _subsprites = std::vector<sf::Sprite>{};
        std::get<std::vector<sf::Sprite>>(_subsprites).push_back(subsprite0);
        std::get<std::vector<sf::Sprite>>(_subsprites).push_back(newSubsprite);
    }
    else {
        std::get<std::vector<sf::Sprite>>(_subsprites).push_back(newSubsprite);
    }

    _subspriteCount += 1;
}

PZInteger Multisprite::getSubspriteCount() const {
    return _subspriteCount;
}

const sf::Sprite& Multisprite::getSubsprite(PZInteger subspriteIndex) const {
    return *(addressOfFirstSprite() + subspriteIndex);
}

void Multisprite::draw(sf::RenderTarget& target, sf::RenderStates states) const {
    if (_subspriteCount == 0) {
        return;
    }

    sf::Sprite currentSprite = *(addressOfFirstSprite() + _subspriteIndex);
    states.transform *= getTransform();
    currentSprite.setColor(_color);
    target.draw(currentSprite, states);
}

const sf::Sprite* Multisprite::addressOfFirstSprite() const {
    if (_subspriteCount <= 1) {
        return std::addressof(std::get<sf::Sprite>(_subsprites));
    }
    else {
        return std::get<std::vector<sf::Sprite>>(_subsprites).data();
    }
}

} // namespace gr
HOBGOBLIN_NAMESPACE_END

#include <Hobgoblin/Private/Pmacro_undef.hpp>