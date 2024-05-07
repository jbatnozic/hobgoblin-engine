// Code in this file is adapted from SFML code and retains its original
// open source licence (provided below).
// See https://github.com/SFML/SFML

////////////////////////////////////////////////////////////
//
// SFML - Simple and Fast Multimedia Library
// Copyright (C) 2007-2018 Laurent Gomila (laurent@sfml-dev.org)
//
// This software is provided 'as-is', without any express or implied warranty.
// In no event will the authors be held liable for any damages arising from the use of this software.
//
// Permission is granted to anyone to use this software for any purpose,
// including commercial applications, and to alter it and redistribute it freely,
// subject to the following restrictions:
//
// 1. The origin of this software must not be misrepresented;
//    you must not claim that you wrote the original software.
//    If you use this software in a product, an acknowledgment
//    in the product documentation would be appreciated but is not required.
//
// 2. Altered source versions must be plainly marked as such,
//    and must not be misrepresented as being the original software.
//
// 3. This notice may not be removed or altered from any source distribution.
//
////////////////////////////////////////////////////////////

// clang-format off

#ifndef UHOBGOBLIN_GRAPHICS_SPRITE_HPP
#define UHOBGOBLIN_GRAPHICS_SPRITE_HPP

#include <Hobgoblin/Graphics/Color.hpp>
#include <Hobgoblin/Graphics/Drawable.hpp>
#include <Hobgoblin/Graphics/Texture_rect.hpp>
#include <Hobgoblin/Graphics/Transformable.hpp>
#include <Hobgoblin/Math/Angle.hpp>
#include <Hobgoblin/Math/Rectangle.hpp>
#include <Hobgoblin/Math/Vector.hpp>

#include <type_traits>

#include <Hobgoblin/Private/Pmacro_define.hpp>

HOBGOBLIN_NAMESPACE_BEGIN
namespace gr {

namespace detail {
class GraphicsImplAccessor;
} // namespace detail

class Texture;

//! \brief Drawable representation of a texture, with its
//!        own transformations, color, etc.
class Sprite final : public Drawable, public Transformable {
public:
    //! \brief Default constructor.
    //!
    //! Creates an empty sprite with no source texture.
    Sprite();

    //! \brief Copy constructor.
    Sprite(const Sprite& aOther);

    //! \brief Copy assignment operator.
    Sprite& operator=(const Sprite& aOther);

    //! Destructor.
    ~Sprite();

    //! \brief Construct the sprite from a source texture
    //!
    //! \param texture Source texture
    //!
    //! \see setTexture
    explicit Sprite(const Texture* aTexture);

    //! \brief Construct the sprite from a sub-rectangle of a source texture
    //!
    //! \param texture   Source texture
    //! \param rectangle Sub-rectangle of the texture to assign to the sprite
    //!
    //! \see setTexture, setTextureRect
    Sprite(const Texture* aTexture, TextureRect aTextureRect);

    //! \brief Change the source texture of the sprite
    //!
    //! The \a texture argument refers to a texture that must
    //! exist as long as the sprite uses it. Indeed, the sprite
    //! doesn't store its own copy of the texture, but rather keeps
    //! a pointer to the one that you passed to this function.
    //! If the source texture is destroyed and the sprite tries to
    //! use it, the behavior is undefined.
    //! If \a resetRect is true, the TextureRect property of
    //! the sprite is automatically adjusted to the size of the new
    //! texture. If it is false, the texture rect is left unchanged.
    //!
    //! \param texture   New texture
    //! \param resetRect Should the texture rect be reset to the size of the new texture?
    //!
    //! \see getTexture, setTextureRect
    void setTexture(const Texture* aTexture, bool aResetRect = false);

    //! \brief Set the sub-rectangle of the texture that the sprite will display
    //!
    //! The texture rect is useful when you don't want to display
    //! the whole texture, but rather a part of it.
    //! By default, the texture rect covers the entire texture.
    //!
    //! \param rectangle Rectangle defining the region of the texture to display
    //!
    //! \see getTextureRect, setTexture
    void setTextureRect(TextureRect aTextureRect);

    //! \brief Set the global color of the sprite
    //!
    //! This color is modulated (multiplied) with the sprite's
    //! texture. It can be used to colorize the sprite, or change
    //! its global opacity.
    //! By default, the sprite's color is opaque white.
    //!
    //! \param color New color of the sprite
    //!
    //! \see getColor
    void setColor(const Color& aColor);

    //! \brief Get the source texture of the sprite
    //!
    //! If the sprite has no source texture, a NULL pointer is returned.
    //! The returned pointer is const, which means that you can't
    //! modify the texture when you retrieve it with this function.
    //!
    //! \return Pointer to the sprite's texture
    //!
    //! \see setTexture
    const Texture* getTexture() const;

    //! \brief Get the sub-rectangle of the texture displayed by the sprite
    //!
    //! \return Texture rectangle of the sprite
    //!
    //! \see setTextureRect
    TextureRect getTextureRect() const;

    //! \brief Get the global color of the sprite
    //!
    //! \return Global color of the sprite
    //!
    //! \see setColor
    Color getColor() const;

    //! \brief Get the local bounding rectangle of the entity
    //!
    //! The returned rectangle is in local coordinates, which means
    //! that it ignores the transformations (translation, rotation,
    //! scale, ...) that are applied to the entity.
    //! In other words, this function returns the bounds of the
    //! entity in the entity's coordinate system.
    //!
    //! \return Local bounding rectangle of the entity
    math::Rectangle<float> getLocalBounds() const;

    //! \brief Get the global bounding rectangle of the entity
    //!
    //! The returned rectangle is in global coordinates, which means
    //! that it takes into account the transformations (translation,
    //! rotation, scale, ...) that are applied to the entity.
    //! In other words, this function returns the bounds of the
    //! sprite in the global 2D world's coordinate system.
    //!
    //! \return Global bounding rectangle of the entity
    math::Rectangle<float> getGlobalBounds() const;

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

    Transform getInverseTransform() const override final;

protected:
    void _draw(Canvas& aCanvas, const RenderStates& aStates) const override;
    BatchingType getBatchingType() const final override;

private:
    friend class detail::GraphicsImplAccessor;

    void* _getSFMLImpl();
    const void* _getSFMLImpl() const;

    static constexpr std::size_t STORAGE_SIZE  = 288;
    static constexpr std::size_t STORAGE_ALIGN =   8;
    std::aligned_storage<STORAGE_SIZE, STORAGE_ALIGN>::type _storage;

    const Texture* _texture = nullptr;
};

} // namespace gr
HOBGOBLIN_NAMESPACE_END

#include <Hobgoblin/Private/Pmacro_undef.hpp>
#include <Hobgoblin/Private/Short_namespace.hpp>

#endif // !UHOBGOBLIN_GRAPHICS_SPRITE_HPP

// clang-format on
