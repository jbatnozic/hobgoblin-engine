#ifndef UHOBGOBLIN_GR_MULTISPRITE_HPP
#define UHOBGOBLIN_GR_MULTISPRITE_HPP

#include <Hobgoblin/Common.hpp>
#include <Hobgoblin/Graphics/Color.hpp>
#include <Hobgoblin/Graphics/Drawable.hpp>
#include <Hobgoblin/Graphics/Sprite.hpp>
#include <SFML/Graphics.hpp>

#include <variant>
#include <vector>

#include <Hobgoblin/Private/Pmacro_define.hpp>

HOBGOBLIN_NAMESPACE_BEGIN
namespace gr {

class Multisprite : public Drawable, public sf::Transformable {
public:
    //! Constructs a multisprite with a single subsprite.
    Multisprite(const sf::Texture& aTexture, const sf::IntRect& aTextureRect);

    //! Constructs a multisprite with one or more subsprite from a collection of
    //! texture rects.
    template <class taRects>
    Multisprite(const sf::Texture& aTexture, const taRects& aTextureRects);

    ///////////////////////////////////////////////////////////////////////////
    // TEXTURE                                                               //
    ///////////////////////////////////////////////////////////////////////////

    const sf::Texture& getTexture() const;

    ///////////////////////////////////////////////////////////////////////////
    // SUBSPRITES                                                            //
    ///////////////////////////////////////////////////////////////////////////

    PZInteger getSubspriteCount() const;

    void selectSubsprite(PZInteger aSubspriteIndex);

    void addSubsprite(const sf::IntRect& aTextureRect);

    void removeSubsprite(PZInteger aSubspriteIndex);

    Sprite extractSubsprite(PZInteger aSubspriteIndex) const;

    ///////////////////////////////////////////////////////////////////////////
    // COLOUR                                                                //
    ///////////////////////////////////////////////////////////////////////////

    void setColor(sf::Color aColor);

    sf::Color getColor() const;

    ///////////////////////////////////////////////////////////////////////////
    // BOUNDS                                                                //
    ///////////////////////////////////////////////////////////////////////////

    //! Returns the local bounding rectangle of a specific subsprite.
    //! The returned rectangle is in local coordinates, which means that it 
    //! ignores the transformations (translation, rotation, scale, ...) that are
    //! applied to the subsprite. In other words, this function returns the 
    //! bounds of the subsprite in the subsprite's coordinate system.
    sf::FloatRect getLocalBounds(PZInteger aSubspriteIndex) const;

    //! Returns the local bounding rectangle of the surrently selected subsprite.
    sf::FloatRect getLocalBounds() const;

    //! Returns the global bounding rectangle of a specific subsprite.
    //! The returned rectangle is in global coordinates, which means that it takes
    //! into account the transformations (translation, rotation, scale, ...) that
    //! are applied to the subsprite. In other words, this function returns the
    //! bounds of the subsprite in the global 2D world's coordinate system.
    sf::FloatRect getGlobalBounds(PZInteger aSubspriteIndex) const;

    //! Returns the global bounding rectangle of the currently selected subsprite.
    sf::FloatRect getGlobalBounds() const;

    //! Returns true if all the subsprites are of the same size.
    bool isNormalized() const;

protected:
    void _draw(Canvas& aCanvas, const sf::RenderStates& aStates) const override;

private:
    const sf::Texture& _texture;

    struct Subsprite {
        Subsprite() = default;
        explicit Subsprite(const sf::IntRect& aTextureRect);

        sf::Vertex  vertices[4];
        sf::IntRect textureRect;

        sf::FloatRect getLocalBounds() const;
    };

    std::variant<Subsprite, std::vector<Subsprite>> _subsprites;

    PZInteger _subspriteCount = 0;
    PZInteger _selectedSubsprite = 0;
    sf::Color _color = sf::Color::White;

    Subsprite* _firstSubspritePtr();

    const Subsprite* _firstSubspritePtr() const;
};

template <class taRects>
Multisprite::Multisprite(const sf::Texture& aTexture, const taRects& aTextureRects)
    : _texture{aTexture}
{
    for (const auto& rect : aTextureRects) {
        addSubsprite(rect);
    }
}

class MultispriteBlueprint {
public:
    ///////////////////////////////////////////////////////////////////////////
    // CONSTRUCTORS                                                          //
    ///////////////////////////////////////////////////////////////////////////

    // Construct with single subsprite
    MultispriteBlueprint(const sf::Texture& aTexture, const sf::IntRect& aTextureRect);

    // Construct with one or more subsprites
    template <class taRects>
    MultispriteBlueprint(const sf::Texture& aTexture, const taRects& aTextureRects);

    ///////////////////////////////////////////////////////////////////////////
    // COPIES & MOVES                                                        //
    ///////////////////////////////////////////////////////////////////////////

    MultispriteBlueprint(const MultispriteBlueprint& aOther);

    MultispriteBlueprint& operator=(const MultispriteBlueprint& aOther);

    MultispriteBlueprint(MultispriteBlueprint&& aOther);

    MultispriteBlueprint& operator=(MultispriteBlueprint&& aOther);

    ///////////////////////////////////////////////////////////////////////////
    // GETTERS                                                               //
    ///////////////////////////////////////////////////////////////////////////

    //! Returns number of contained subsprites.
    PZInteger getSubspriteCount() const;

    //! Make a regular Sprite from a single subsprite from within the blueprint.
    Sprite subspr(PZInteger aSubspriteIndex) const;

    //! Make a complete Multisprite from the whole blueprint.
    Multisprite multispr() const;

private:
    const sf::Texture* _texture;

    std::variant<sf::IntRect, std::vector<sf::IntRect>> _textureRects;

    PZInteger _subspriteCount;
};

template <class taRects>
MultispriteBlueprint::MultispriteBlueprint(const sf::Texture& aTexture, const taRects& aTextureRects)
    : _texture{&aTexture}
    , _subspriteCount{stopz(aTextureRects.size())}
{
    if (_subspriteCount > 1) {
        _textureRects = std::vector<sf::IntRect>{};
        for (const auto& rect : aTextureRects) {
            std::get<std::vector<sf::IntRect>>(_textureRects).push_back(rect);
        }
        return;
    }

    if (_subspriteCount == 1) {
        _textureRects = *(aTextureRects.begin());
        return;
    }

    throw TracedLogicError{"MultispriteBlueprint - Must be constructed with at least 1 subsprite!"};
}

} // namespace gr
HOBGOBLIN_NAMESPACE_END

#include <Hobgoblin/Private/Pmacro_undef.hpp>
#include <Hobgoblin/Private/Short_namespace.hpp>

#endif // !UHOBGOBLIN_GR_MULTISPRITE_HPP