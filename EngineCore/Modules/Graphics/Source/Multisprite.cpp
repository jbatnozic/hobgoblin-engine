
#include <Hobgoblin/Graphics/Canvas.hpp>
#include <Hobgoblin/Graphics/Multisprite.hpp>

#include <cassert>
#include <cmath>
#include <cstring>

#include <Hobgoblin/Private/Pmacro_define.hpp>

HOBGOBLIN_NAMESPACE_BEGIN
namespace gr {

///////////////////////////////////////////////////////////////////////////
// MULTISPRITE                                                           //
///////////////////////////////////////////////////////////////////////////

Multisprite::Multisprite(const sf::Texture& aTexture, const sf::IntRect& aTextureRect)
    : _texture{aTexture}
{
    addSubsprite(aTextureRect);
}

const sf::Texture& Multisprite::getTexture() const {
    return _texture;
}

PZInteger Multisprite::getSubspriteCount() const {
    return _subspriteCount;
}

void Multisprite::selectSubsprite(PZInteger aSubspriteIndex) {
    _selectedSubsprite = aSubspriteIndex;
}

void Multisprite::addSubsprite(const sf::IntRect& aTextureRect) {
    if (_subspriteCount == 0) {
        _subsprites = _makeSubsprite(aTextureRect);
    }
    else if (_subspriteCount == 1) {
        const auto subsprite0 = *_firstSubspritePtr();
        _subsprites = std::vector<Subsprite>{};
        std::get<std::vector<Subsprite>>(_subsprites).push_back(subsprite0);
        std::get<std::vector<Subsprite>>(_subsprites).push_back(_makeSubsprite(aTextureRect));
    }
    else {
        std::get<std::vector<Subsprite>>(_subsprites).push_back(_makeSubsprite(aTextureRect));
    }

    _subspriteCount += 1;
}

void Multisprite::removeSubsprite(PZInteger aSubspriteIndex) {
    // TODO
}

void Multisprite::setColor(sf::Color aColor) {
    _color = aColor;
}

sf::Color Multisprite::getColor() const {
    return _color;
}

void Multisprite::_draw(Canvas& aCanvas, const sf::RenderStates& aStates) const {
    if (_subspriteCount == 0) {
        return;
    }

    if (_selectedSubsprite < 0 || _selectedSubsprite >= _subspriteCount) {
        // TODO Exception (out of bounds)
    }

    const auto& subspr = *(_firstSubspritePtr() + _selectedSubsprite);

    // Prepare vertices
    sf::Vertex vertices[4];
    std::memcpy(vertices, subspr.vertices, sizeof(sf::Vertex) * 4);
    vertices[0].color = vertices[1].color = vertices[2].color = vertices[3].color = _color;

    // Prepare render states
    sf::RenderStates statesCopy{aStates};
    statesCopy.transform *= getTransform();
    statesCopy.texture = &_texture;

    aCanvas.draw(vertices, 4, sf::PrimitiveType::TriangleStrip, statesCopy);
}

Multisprite::Subsprite* Multisprite::_firstSubspritePtr() {
    if (_subspriteCount <= 1) {
        return std::addressof(std::get<Subsprite>(_subsprites));
    }
    else {
        return std::get<std::vector<Subsprite>>(_subsprites).data();
    }
}

const Multisprite::Subsprite* Multisprite::_firstSubspritePtr() const {
    if (_subspriteCount <= 1) {
        return std::addressof(std::get<Subsprite>(_subsprites));
    }
    else {
        return std::get<std::vector<Subsprite>>(_subsprites).data();
    }
}

// TODO This could be a Subsprite c-tor
Multisprite::Subsprite Multisprite::_makeSubsprite(const sf::IntRect& aTextureRect) {
    Subsprite result;
    result.textureRect = aTextureRect;

    // World positions
    {
        // TODO This is result.getLocalBounds()
        const sf::FloatRect bounds = {
            {
                0.f,
                0.f
            },
        {
            static_cast<float>(std::abs(aTextureRect.width)),
            static_cast<float>(std::abs(aTextureRect.height))
        }
        };

        result.vertices[0].position = sf::Vector2f{0.f, 0.f};
        result.vertices[1].position = sf::Vector2f{0.f, bounds.height};
        result.vertices[2].position = sf::Vector2f{bounds.width, 0.f};
        result.vertices[3].position = sf::Vector2f{bounds.width, bounds.height};
    }

    // Texture positions
    {
        const sf::FloatRect convertedTextureRect{aTextureRect};

        const float left = convertedTextureRect.left;
        const float right = left + convertedTextureRect.width;
        const float top = convertedTextureRect.top;
        const float bottom = top + convertedTextureRect.height;

        result.vertices[0].texCoords = sf::Vector2f{left, top};
        result.vertices[1].texCoords = sf::Vector2f{left, bottom};
        result.vertices[2].texCoords = sf::Vector2f{right, top};
        result.vertices[3].texCoords = sf::Vector2f{right, bottom};
    }

    return result;
}

///////////////////////////////////////////////////////////////////////////
// MULTISPRITE BLUEPRINT                                                 //
///////////////////////////////////////////////////////////////////////////

    // Construct with single subsprite
MultispriteBlueprint::MultispriteBlueprint(const sf::Texture& aTexture, const sf::IntRect& aTextureRect)
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
    if (aSubspriteIndex >= _subspriteCount) {
        throw TracedLogicError{"MultispriteBlueprint - Subsprite index out of bounds!"};
    }

    if (_subspriteCount > 1) {
        return Sprite{
            *_texture, 
            std::get<std::vector<sf::IntRect>>(_textureRects).at(pztos(aSubspriteIndex))
        };
    }

    return Sprite{*_texture, std::get<sf::IntRect>(_textureRects)};
}

Multisprite MultispriteBlueprint::multispr() const {
    if (_subspriteCount > 1) {
        return Multisprite{*_texture, std::get<std::vector<sf::IntRect>>(_textureRects)};
    }

    return Multisprite{*_texture, std::get<sf::IntRect>(_textureRects)};
}

} // namespace gr
HOBGOBLIN_NAMESPACE_END

#include <Hobgoblin/Private/Pmacro_undef.hpp>