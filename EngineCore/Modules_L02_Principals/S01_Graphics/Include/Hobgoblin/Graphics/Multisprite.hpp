// Copyright 2024 Jovan Batnozic. Released under MS-PL licence in Serbia.
// See https://github.com/jbatnozic/Hobgoblin?tab=readme-ov-file#licence

// clang-format off

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
#include <Hobgoblin/Graphics/Vertex.hpp>
#include <Hobgoblin/Math/Rectangle.hpp>

#include <variant>
#include <vector>

#include <Hobgoblin/Private/Pmacro_define.hpp>

HOBGOBLIN_NAMESPACE_BEGIN
namespace gr {

class Multisprite : public Drawable, public Transformable {
public:
    //! Default-constructs the multisprite.
    //! Note that the multisprite won't be visible when drawn until it's given
    //! a non-null texture (see `setTexture`) and at least 1 subsprite (see
    //! `addSubsprite`).
    Multisprite();

    //!
    explicit Multisprite(const Texture* aTexture);

    //! Constructs a multisprite with a single subsprite.
    Multisprite(const Texture* aTexture, TextureRect aTextureRect);

    //! Constructs a multisprite with one or more subsprite from a collection of
    //! texture rects.
    template <class taRectsBeginIterator, class taRectsEndIterator>
    Multisprite(const Texture* aTexture, 
                taRectsBeginIterator aTextureRectsBeginIterator,
                taRectsEndIterator aTextureRectsEndIterator);

    //! Copy constructor.
    Multisprite(const Multisprite& aOther) = default;

    //! Copy assignment operator.
    Multisprite& operator=(const Multisprite& aOther) = default;

    BatchingType getBatchingType() const override final;

    ///////////////////////////////////////////////////////////////////////////
    // TEXTURE                                                               //
    ///////////////////////////////////////////////////////////////////////////

    void setTexture(const Texture* aTexture);

    const Texture* getTexture() const;

    ///////////////////////////////////////////////////////////////////////////
    // SUBSPRITES                                                            //
    ///////////////////////////////////////////////////////////////////////////

    void addSubsprite(TextureRect aTextureRect);

    void removeSubsprite(PZInteger aSubspriteIndex);

    //! Returns the number of subsprites in the multisprite.
    PZInteger getSubspriteCount() const;

    //! Returns the value of the underlying subsprite selector.
    float getSubspriteSelector() const;

    //! Returns the index of the subsprite that's currently considered active
    //! based on the selector's value (see `getSubspriteSelector`). As the
    //! selector is a floating-point value, there are some extra considerations:
    //! - If the selector is not a whole number, it will be rounded down.
    //! - If the selector is out of bounds [0..subsprite_count], it will be brought
    //!   back into bounds by modulo arithmetic.
    //! 
    //! If the multisprite has no subsprites, this method will throw.
    PZInteger getCurrentSubspriteIndex() const;

    void selectSubsprite(int aSelectorValue);

    void selectSubsprite(float aSelectorValue);

    void advanceSubsprite(int aSelectorOffset);

    void advanceSubsprite(float aSelectorOffset);

    //! Returns a single subsprite as a standalone `Sprite` object.
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
    void _drawOnto(Canvas& aCanvas, const RenderStates& aStates) const override;

private:
    const Texture* _texture;

    TransformableData _transformableData;

    struct Subsprite {
        Subsprite() = default;
        explicit Subsprite(TextureRect aTextureRect);

        Vertex      vertices[6];
        TextureRect textureRect;

        math::Rectangle<float> getLocalBounds() const;
    };

    std::variant<Subsprite, std::vector<Subsprite>> _subsprites;

    PZInteger _subspriteCount = 0;
    float _subspriteSelector = 0.f;
    Color _color = COLOR_WHITE;

    Subsprite* _firstSubspritePtr();
    const Subsprite* _firstSubspritePtr() const;
};

template <class taRectsBeginIterator, class taRectsEndIterator>
Multisprite::Multisprite(const Texture* aTexture, 
                         taRectsBeginIterator aTextureRectsBeginIterator,
                         taRectsEndIterator aTextureRectsEndIterator)
    : _texture{aTexture}
{
    for (auto iter = aTextureRectsBeginIterator; iter != aTextureRectsEndIterator; iter = std::next(iter)) {
        const auto& rect = *iter;
        addSubsprite(rect);
    }
}

} // namespace gr
HOBGOBLIN_NAMESPACE_END

#include <Hobgoblin/Private/Pmacro_undef.hpp>
#include <Hobgoblin/Private/Short_namespace.hpp>

#endif // !UHOBGOBLIN_GRAPHICS_MULTISPRITE_HPP

// clang-format on
