#ifndef UHOBGOBLIN_GRAPHICS_MULTISPRITE_HPP
#define UHOBGOBLIN_GRAPHICS_MULTISPRITE_HPP

#include <Hobgoblin/Common.hpp>
#include <Hobgoblin/Graphics/Color.hpp>
#include <Hobgoblin/Graphics/Drawable.hpp>
#include <Hobgoblin/Graphics/Sprite.hpp>
#include <Hobgoblin/Graphics/Texture_rect.hpp>
#include <Hobgoblin/Graphics/Transform.hpp>
#include <Hobgoblin/Graphics/Transformable.hpp>
#include <Hobgoblin/Graphics/Transformable_data.hpp>
#include <Hobgoblin/Math/Rectangle.hpp>

#include <variant>
#include <vector>

#include <Hobgoblin/Private/Pmacro_define.hpp>

HOBGOBLIN_NAMESPACE_BEGIN
namespace gr {

class Multisprite : public Drawable, public Transformable {
public:
    //!
    Multisprite(const Texture& aTexture);

    //! Constructs a multisprite with a single subsprite.
    Multisprite(const Texture& aTexture, TextureRect aTextureRect);

    //! Constructs a multisprite with one or more subsprite from a collection of
    //! texture rects.
    template <class taRectCollection>
    Multisprite(const Texture& aTexture, const taRectCollection& aTextureRects);

    BatchingType getBatchingType() const override;

    ///////////////////////////////////////////////////////////////////////////
    // TEXTURE                                                               //
    ///////////////////////////////////////////////////////////////////////////

    const Texture& getTexture() const;

    ///////////////////////////////////////////////////////////////////////////
    // SUBSPRITES                                                            //
    ///////////////////////////////////////////////////////////////////////////

    PZInteger getSubspriteCount() const;

    PZInteger getCurrentSubspriteIndex() const;

    void selectSubsprite(PZInteger aSubspriteIndex);

    void advanceSubsprite(PZInteger aSubspriteCount);

    void addSubsprite(TextureRect aTextureRect);

    void removeSubsprite(PZInteger aSubspriteIndex);

    Sprite extractSubsprite(PZInteger aSubspriteIndex) const;

    ///////////////////////////////////////////////////////////////////////////
    // COLOUR                                                                //
    ///////////////////////////////////////////////////////////////////////////

    void setColor(Color aColor);

    Color getColor() const;

    ///////////////////////////////////////////////////////////////////////////
    // BOUNDS                                                                //
    ///////////////////////////////////////////////////////////////////////////

    //! Returns the local bounding rectangle of a specific subsprite.
    //! The returned rectangle is in local coordinates, which means that it 
    //! ignores the transformations (translation, rotation, scale, ...) that are
    //! applied to the subsprite. In other words, this function returns the 
    //! bounds of the subsprite in the subsprite's coordinate system.
    math::Rectangle<float> getLocalBounds(PZInteger aSubspriteIndex) const;

    //! Returns the local bounding rectangle of the surrently selected subsprite.
    math::Rectangle<float> getLocalBounds() const;

    //! Returns the global bounding rectangle of a specific subsprite.
    //! The returned rectangle is in global coordinates, which means that it takes
    //! into account the transformations (translation, rotation, scale, ...) that
    //! are applied to the subsprite. In other words, this function returns the
    //! bounds of the subsprite in the global 2D world's coordinate system.
    math::Rectangle<float> getGlobalBounds(PZInteger aSubspriteIndex) const;

    //! Returns the global bounding rectangle of the currently selected subsprite.
    math::Rectangle<float> getGlobalBounds() const;

    //! Returns true if all the subsprites are of the same size.
    bool isNormalized() const;

    //! Same as `isNormalized`.
    bool areAllSubspritesOfSameSize() const;

    ///////////////////////////////////////////////////////////////////////////
    // TRANSFORMABLE                                                         //
    ///////////////////////////////////////////////////////////////////////////

    // TODO: see which overriden methods should be final

    void setPosition(float aX, float aY) override;

    void setPosition(const math::Vector2f& aPosition) override;

    void setRotation(math::AngleF aAngle) override;

    void setScale(float aFactorX, float aFactorY) override;

    void setScale(const math::Vector2f& aFactors) override;

    void setOrigin(float aX, float aY) override;

    void setOrigin(const math::Vector2f& aOrigin) override;

    math::Vector2f getPosition() const override;

    math::AngleF getRotation() const override;

    math::Vector2f getScale() const override;

    math::Vector2f getOrigin() const override;

    void move(float aOffsetX, float aOffsetY) override;

    void move(const math::Vector2f& aOffset) override;

    void rotate(math::AngleF aAngle) override;

    void scale(float aFactorX, float aFactorY) override;

    void scale(const math::Vector2f& aFactor) override;

    Transform getTransform() const override;

    Transform getInverseTransform() const override;

protected:
    void _draw(Canvas& aCanvas, const RenderStates& aStates) const override;

private:
    const Texture& _texture;

    TransformableData _transformableData;

    struct Subsprite {
        Subsprite() = default;
        explicit Subsprite(TextureRect aTextureRect);

        Vertex      vertices[4];
        TextureRect textureRect;

        math::Rectangle<float> getLocalBounds() const;
    };

    std::variant<Subsprite, std::vector<Subsprite>> _subsprites;

    PZInteger _subspriteCount = 0;
    PZInteger _selectedSubsprite = 0;
    Color _color = COLOR_WHITE;

    Subsprite* _firstSubspritePtr();

    const Subsprite* _firstSubspritePtr() const;
};

template <class taRects>
Multisprite::Multisprite(const Texture& aTexture, const taRects& aTextureRects)
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
    MultispriteBlueprint(const Texture& aTexture, TextureRect aTextureRect);

    // Construct with one or more subsprites
    template <class taRects>
    MultispriteBlueprint(const Texture& aTexture, const taRects& aTextureRects);

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
    const Texture* _texture;

    std::variant<TextureRect, std::vector<TextureRect>> _textureRects;

    PZInteger _subspriteCount;
};

template <class taRects>
MultispriteBlueprint::MultispriteBlueprint(const Texture& aTexture, const taRects& aTextureRects)
    : _texture{&aTexture}
    , _subspriteCount{stopz(aTextureRects.size())}
{
    if (_subspriteCount > 1) {
        _textureRects = std::vector<TextureRect>{};
        for (const auto& rect : aTextureRects) {
            std::get<std::vector<TextureRect>>(_textureRects).push_back(rect);
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

#endif // !UHOBGOBLIN_GRAPHICS_MULTISPRITE_HPP
