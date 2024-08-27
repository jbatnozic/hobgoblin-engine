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

#ifndef UHOBGOBLIN_GRAPHICS_TEXT_HPP
#define UHOBGOBLIN_GRAPHICS_TEXT_HPP

#include <Hobgoblin/Common.hpp>
#include <Hobgoblin/Graphics/Color.hpp>
#include <Hobgoblin/Graphics/Drawable.hpp>
#include <Hobgoblin/Graphics/Transformable.hpp>
#include <Hobgoblin/Math.hpp>
#include <Hobgoblin/Unicode.hpp>

#include <string>
#include <type_traits>

#include <Hobgoblin/Private/Pmacro_define.hpp>

HOBGOBLIN_NAMESPACE_BEGIN
namespace gr {

class Font;

//!/ \brief Graphical text that can be drawn to a render target
class Text : public Drawable, public Transformable {
public:
    
    //! \brief Enumeration of the string drawing styles
    enum Style {
        REGULAR       = 0,      //!!< Regular characters, no style
        BOLD          = 1 << 0, //!!< Bold characters
        ITALIC        = 1 << 1, //!!< Italic characters
        UNDERLINED    = 1 << 2, //!!< Underlined characters
        STRIKETHROUGH = 1 << 3  //!!< Strike through characters
    };

    static constexpr PZInteger DEFAULT_CHARACTER_SIZE = 30;

    //! \brief Construct the text from a string, font and size
    //!
    //! Note that if the used font is a bitmap font, it is not
    //! scalable, thus not all requested sizes will be available
    //! to use. This needs to be taken into consideration when
    //! setting the character size. If you need to display text
    //! of a certain size, make sure the corresponding bitmap
    //! font that supports that size is used.
    //!
    //! \param string         Text assigned to the string
    //! \param font           Font used to draw the string
    //! \param characterSize  Base size of characters, in pixels
    Text(const Font& aFont,
         const char* aString = "",
         PZInteger aCharacterSize = DEFAULT_CHARACTER_SIZE);

    Text(const Font& aFont,
         const std::string& aString,
         PZInteger aCharacterSize = DEFAULT_CHARACTER_SIZE);

    Text(const Font& aFont,
         const UnicodeString& aString,
         PZInteger aCharacterSize = DEFAULT_CHARACTER_SIZE);

    //!/ \brief Disallow construction from a temporary font
    Text(Font&& aFont,
         std::string aString = "",
         PZInteger aCharacterSize = DEFAULT_CHARACTER_SIZE) = delete;

    //! Copy constructor
    Text(const Text& aOther);

    //! Copy assignment operator
    Text& operator=(const Text& aOther);

    //! Move constructor
    Text(Text&& aOther) noexcept;

    //! Move assignment operator
    Text& operator=(Text&& aOther) noexcept;

    //! Destructor
    ~Text();

    //! \brief Set the text's string
    //!
    //! The \a string argument is a sf::String, which can
    //! automatically be constructed from standard string types.
    //! So, the following calls are all valid:
    //! \code
    //! text.setString("hello");
    //! text.setString(L"hello");
    //! text.setString(std::string("hello"));
    //! text.setString(std::wstring(L"hello"));
    //! \endcode
    //! A text's string is empty by default.
    //!
    //! \param string New string
    //!
    //! \see getString
    void setString(const std::string& aString);

    //! TODO
    void setString(const UnicodeString& aString);

    //! \brief Set the text's font
    //!
    //! The \a font argument refers to a font that must
    //! exist as long as the text uses it. Indeed, the text
    //! doesn't store its own copy of the font, but rather keeps
    //! a pointer to the one that you passed to this function.
    //! If the font is destroyed and the text tries to
    //! use it, the behavior is undefined.
    //!
    //! \param font New font
    //!
    //! \see getFont
    void setFont(const Font& aFont);

    //! \brief Disallow setting from a temporary font
    void setFont(Font&& aFont) = delete;

    //! \brief Set the character size
    //!
    //! The default size is 30.
    //!
    //! Note that if the used font is a bitmap font, it is not
    //! scalable, thus not all requested sizes will be available
    //! to use. This needs to be taken into consideration when
    //! setting the character size. If you need to display text
    //! of a certain size, make sure the corresponding bitmap
    //! font that supports that size is used.
    //!
    //! \param size New character size, in pixels
    //!
    //! \see getCharacterSize
    void setCharacterSize(PZInteger aSize);

    //! \brief Set the line spacing factor
    //!
    //! The default spacing between lines is defined by the font.
    //! This method enables you to set a factor for the spacing
    //! between lines. By default the line spacing factor is 1.
    //!
    //! \param spacingFactor New line spacing factor
    //!
    //! \see getLineSpacing
    void setLineSpacing(float aSpacingFactor);

    //! \brief Set the letter spacing factor
    //!
    //! The default spacing between letters is defined by the font.
    //! This factor doesn't directly apply to the existing
    //! spacing between each character, it rather adds a fixed
    //! space between them which is calculated from the font
    //! metrics and the character size.
    //! Note that factors below 1 (including negative numbers) bring
    //! characters closer to each other.
    //! By default the letter spacing factor is 1.
    //!
    //! \param spacingFactor New letter spacing factor
    //!
    //! \see getLetterSpacing
    void setLetterSpacing(float aSpacingFactor);

    //! \brief Set the text's style
    //!
    //! You can pass a combination of one or more styles, for
    //! example sf::Text::Bold | sf::Text::Italic.
    //! The default style is sf::Text::Regular.
    //!
    //! \param style New style
    //!
    //! \see getStyle
    void setStyle(std::uint32_t aStyle);

    //! \brief Set the fill color of the text
    //!
    //! By default, the text's fill color is opaque white.
    //! Setting the fill color to a transparent color with an outline
    //! will cause the outline to be displayed in the fill area of the text.
    //!
    //! \param color New fill color of the text
    //!
    //! \see getFillColor
    void setFillColor(Color aColor);

    //! \brief Set the outline color of the text
    //!
    //! By default, the text's outline color is opaque black.
    //!
    //! \param color New outline color of the text
    //!
    //! \see getOutlineColor
    void setOutlineColor(Color aColor);

    //! \brief Set the thickness of the text's outline
    //!
    //! By default, the outline thickness is 0.
    //!
    //! Be aware that using a negative value for the outline
    //! thickness will cause distorted rendering.
    //!
    //! \param thickness New outline thickness, in pixels
    //!
    //! \see getOutlineThickness
    void setOutlineThickness(float aThickness);

    //! \brief Get the text's string
    //!
    //! The returned string is a sf::String, which can automatically
    //! be converted to standard string types. So, the following
    //! lines of code are all valid:
    //! \code
    //! sf::String   s1 = text.getString();
    //! std::string  s2 = text.getString();
    //! std::wstring s3 = text.getString();
    //! \endcode
    //!
    //! \return Text's string
    //! 
    //! \warning As `Text` internally stores the string to display as UTF-32
    //!          and `UnicodeString` is UTF-16, for long strings this function
    //!          could be quite slow (because of the required transcoding).
    //!
    //! \see setString
    UnicodeString getString() const;

    //! \brief Get the text's font
    //!
    //! The returned reference is const, which means that you
    //! cannot modify the font when you get it from this function.
    //!
    //! \return Reference to the text's font
    //!
    //! \see setFont
    const Font& getFont() const;

    //! \brief Get the character size
    //!
    //! \return Size of the characters, in pixels
    //!
    //! \see setCharacterSize
    PZInteger getCharacterSize() const;

    //! \brief Get the size of the letter spacing factor
    //!
    //! \return Size of the letter spacing factor
    //!
    //! \see setLetterSpacing
    float getLetterSpacing() const;

    //! \brief Get the size of the line spacing factor
    //!
    //! \return Size of the line spacing factor
    //!
    //! \see setLineSpacing
    float getLineSpacing() const;

    //! \brief Get the text's style
    //!
    //! \return Text's style
    //!
    //! \see setStyle
    std::uint32_t getStyle() const;

    //! \brief Get the fill color of the text
    //!
    //! \return Fill color of the text
    //!
    //! \see setFillColor
    Color getFillColor() const;

    //! \brief Get the outline color of the text
    //!
    //! \return Outline color of the text
    //!
    //! \see setOutlineColor
    Color getOutlineColor() const;

    //! \brief Get the outline thickness of the text
    //!
    //! \return Outline thickness of the text, in pixels
    //!
    //! \see setOutlineThickness
    float getOutlineThickness() const;

    //! \brief Return the position of the \a index-th character
    //!
    //! This function computes the visual position of a character
    //! from its index in the string. The returned position is
    //! in global coordinates (translation, rotation, scale and
    //! origin are applied).
    //! If \a index is out of range, the position of the end of
    //! the string is returned.
    //!
    //! \param index Index of the character
    //!
    //! \return Position of the character
    math::Vector2f findCharacterPos(PZInteger aIndex) const;

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
    //! text in the global 2D world's coordinate system.
    //!
    //! \return Global bounding rectangle of the entity
    math::Rectangle<float> getGlobalBounds() const;

    Drawable::BatchingType getBatchingType() const override;

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
    friend class detail::GraphicsImplAccessor;

    void* _getSFMLImpl();
    const void* _getSFMLImpl() const;

#ifdef _MSC_VER
    #if HG_BUILD_TYPE == HG_DEBUG
        static constexpr std::size_t STORAGE_SIZE = 392;
    #else
        static constexpr std::size_t STORAGE_SIZE = 368;
    #endif
#elif defined(__linux__)
    static constexpr std::size_t STORAGE_SIZE = 368;
#else
    static constexpr std::size_t STORAGE_SIZE = 360;
#endif

    static constexpr std::size_t STORAGE_ALIGN = 8;
    std::aligned_storage<STORAGE_SIZE, STORAGE_ALIGN>::type _storage;

    const Font* _font = nullptr;
};

} // namespace gr
HOBGOBLIN_NAMESPACE_END

#include <Hobgoblin/Private/Pmacro_undef.hpp>
#include <Hobgoblin/Private/Short_namespace.hpp>

#endif // !UHOBGOBLIN_GRAPHICS_TEXT_HPP

////////////////////////////////////////////////////////////
/// \class sf::Text
/// \ingroup graphics
///
/// sf::Text is a drawable class that allows to easily display
/// some text with custom style and color on a render target.
///
/// It inherits all the functions from sf::Transformable:
/// position, rotation, scale, origin. It also adds text-specific
/// properties such as the font to use, the character size,
/// the font style (bold, italic, underlined and strike through), the
/// text color, the outline thickness, the outline color, the character
/// spacing, the line spacing and the text to display of course.
/// It also provides convenience functions to calculate the
/// graphical size of the text, or to get the global position
/// of a given character.
///
/// sf::Text works in combination with the sf::Font class, which
/// loads and provides the glyphs (visual characters) of a given font.
///
/// The separation of sf::Font and sf::Text allows more flexibility
/// and better performances: indeed a sf::Font is a heavy resource,
/// and any operation on it is slow (often too slow for real-time
/// applications). On the other side, a sf::Text is a lightweight
/// object which can combine the glyphs data and metrics of a sf::Font
/// to display any text on a render target.
///
/// It is important to note that the sf::Text instance doesn't
/// copy the font that it uses, it only keeps a reference to it.
/// Thus, a sf::Font must not be destructed while it is
/// used by a sf::Text (i.e. never write a function that
/// uses a local sf::Font instance for creating a text).
///
/// See also the note on coordinates and undistorted rendering in sf::Transformable.
///
/// Usage example:
/// \code
/// //! Declare and load a font
/// sf::Font font;
/// if (!font.loadFromFile("arial.ttf"))
/// {
///     //! Handle error...
/// }
///
/// //! Create a text
/// sf::Text text(font, "hello");
/// text.setCharacterSize(30);
/// text.setStyle(sf::Text::Bold);
/// text.setFillColor(sf::Color::Red);
///
/// //! Draw it
/// window.draw(text);
/// \endcode
///
/// \see sf::Font, sf::Transformable
///
////////////////////////////////////////////////////////////

// clang-format on
