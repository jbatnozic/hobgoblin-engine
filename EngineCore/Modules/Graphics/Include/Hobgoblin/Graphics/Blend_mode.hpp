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

#ifndef UHOBGOBLIN_GRAPHICS_BLEND_MODE_HPP
#define UHOBGOBLIN_GRAPHICS_BLEND_MODE_HPP

#include <cstdint>

#include <Hobgoblin/Private/Pmacro_define.hpp>

HOBGOBLIN_NAMESPACE_BEGIN
namespace gr {

//! Blending modes for drawing.
struct BlendMode {
    ////////////////////////////////////////////////////////
    /// \brief Enumeration of the blending factors
    ///
    /// The factors are mapped directly to their OpenGL equivalents,
    /// specified by glBlendFunc() or glBlendFuncSeparate().
    ////////////////////////////////////////////////////////
    enum class Factor : std::int8_t {
        Zero,             ///< (0, 0, 0, 0)
        One,              ///< (1, 1, 1, 1)
        SrcColor,         ///< (src.r, src.g, src.b, src.a)
        OneMinusSrcColor, ///< (1, 1, 1, 1) - (src.r, src.g, src.b, src.a)
        DstColor,         ///< (dst.r, dst.g, dst.b, dst.a)
        OneMinusDstColor, ///< (1, 1, 1, 1) - (dst.r, dst.g, dst.b, dst.a)
        SrcAlpha,         ///< (src.a, src.a, src.a, src.a)
        OneMinusSrcAlpha, ///< (1, 1, 1, 1) - (src.a, src.a, src.a, src.a)
        DstAlpha,         ///< (dst.a, dst.a, dst.a, dst.a)
        OneMinusDstAlpha  ///< (1, 1, 1, 1) - (dst.a, dst.a, dst.a, dst.a)
    };

    ////////////////////////////////////////////////////////
    /// \brief Enumeration of the blending equations
    ///
    /// The equations are mapped directly to their OpenGL equivalents,
    /// specified by glBlendEquation() or glBlendEquationSeparate().
    ////////////////////////////////////////////////////////
    enum class Equation : std::int8_t {
        Add,            ///< Pixel = Src * SrcFactor + Dst * DstFactor
        Subtract,       ///< Pixel = Src * SrcFactor - Dst * DstFactor
        ReverseSubtract ///< Pixel = Dst * DstFactor - Src * SrcFactor
    };

    ////////////////////////////////////////////////////////////
    /// \brief Default constructor
    ///
    /// Constructs a blending mode that does alpha blending.
    ///
    ////////////////////////////////////////////////////////////
    constexpr BlendMode();

    ////////////////////////////////////////////////////////////
    /// \brief Construct the blend mode given the factors and equation.
    ///
    /// This constructor uses the same factors and equation for both
    /// color and alpha components. It also defaults to the Add equation.
    ///
    /// \param sourceFactor      Specifies how to compute the source factor for the color and alpha channels.
    /// \param destinationFactor Specifies how to compute the destination factor for the color and alpha channels.
    /// \param blendEquation     Specifies how to combine the source and destination colors and alpha.
    ///
    ////////////////////////////////////////////////////////////
    constexpr BlendMode(Factor sourceFactor, Factor destinationFactor, Equation blendEquation = Equation::Add);

    ////////////////////////////////////////////////////////////
    /// \brief Construct the blend mode given the factors and equation.
    ///
    /// \param colorSourceFactor      Specifies how to compute the source factor for the color channels.
    /// \param colorDestinationFactor Specifies how to compute the destination factor for the color channels.
    /// \param colorBlendEquation     Specifies how to combine the source and destination colors.
    /// \param alphaSourceFactor      Specifies how to compute the source factor.
    /// \param alphaDestinationFactor Specifies how to compute the destination factor.
    /// \param alphaBlendEquation     Specifies how to combine the source and destination alphas.
    ///
    ////////////////////////////////////////////////////////////
    constexpr BlendMode(Factor colorSourceFactor, Factor colorDestinationFactor,
                        Equation colorBlendEquation, Factor alphaSourceFactor,
                        Factor alphaDestinationFactor, Equation alphaBlendEquation);

    Factor   colorSrcFactor; ///< Source blending factor for the color channels
    Factor   colorDstFactor; ///< Destination blending factor for the color channels
    Equation colorEquation;  ///< Blending equation for the color channels
    Factor   alphaSrcFactor; ///< Source blending factor for the alpha channel
    Factor   alphaDstFactor; ///< Destination blending factor for the alpha channel
    Equation alphaEquation;  ///< Blending equation for the alpha channel
};

constexpr
BlendMode::BlendMode()
    : colorSrcFactor(BlendMode::Factor::SrcAlpha)
    , colorDstFactor(BlendMode::Factor::OneMinusSrcAlpha)
    , colorEquation (BlendMode::Equation::Add)
    , alphaSrcFactor(BlendMode::Factor::One)
    , alphaDstFactor(BlendMode::Factor::OneMinusSrcAlpha)
    , alphaEquation (BlendMode::Equation::Add)
{
}

constexpr
BlendMode::BlendMode(Factor sourceFactor, Factor destinationFactor, Equation blendEquation)
    : colorSrcFactor(sourceFactor)
    , colorDstFactor(destinationFactor)
    , colorEquation (blendEquation)
    , alphaSrcFactor(sourceFactor)
    , alphaDstFactor(destinationFactor)
    , alphaEquation (blendEquation)
{
}

constexpr
BlendMode::BlendMode(Factor colorSourceFactor, Factor colorDestinationFactor,
                     Equation colorBlendEquation, Factor alphaSourceFactor,
                     Factor alphaDestinationFactor, Equation alphaBlendEquation)
    : colorSrcFactor(colorSourceFactor)
    , colorDstFactor(colorDestinationFactor)
    , colorEquation (colorBlendEquation)
    , alphaSrcFactor(alphaSourceFactor)
    , alphaDstFactor(alphaDestinationFactor)
    , alphaEquation (alphaBlendEquation)
{
}

////////////////////////////////////////////////////////////
// Commonly used blending modes
////////////////////////////////////////////////////////////

constexpr
BlendMode BLEND_ALPHA    = {BlendMode::Factor::SrcAlpha, BlendMode::Factor::OneMinusSrcAlpha, BlendMode::Equation::Add,
                            BlendMode::Factor::One, BlendMode::Factor::OneMinusSrcAlpha, BlendMode::Equation::Add};

constexpr
BlendMode BLEND_ADD      = {BlendMode::Factor::SrcAlpha, BlendMode::Factor::One, BlendMode::Equation::Add,
                            BlendMode::Factor::One, BlendMode::Factor::One, BlendMode::Equation::Add};

constexpr
BlendMode BLEND_MULTIPLY = {BlendMode::Factor::DstColor, BlendMode::Factor::Zero};

constexpr
BlendMode BLEND_NONE     = {BlendMode::Factor::One, BlendMode::Factor::Zero};

constexpr
BlendMode BLEND_NORMAL   = BLEND_ALPHA;

////////////////////////////////////////////////////////////
/// \relates BlendMode
/// \brief Overload of the == operator
///
/// \param aLhs  Left operand
/// \param aRhs Right operand
///
/// \return True if blending modes are equal, false if they are different
///
////////////////////////////////////////////////////////////
constexpr
bool operator==(const BlendMode& aLhs, const BlendMode& aRhs) {
    return (aLhs.colorSrcFactor == aRhs.colorSrcFactor) &&
           (aLhs.colorDstFactor == aRhs.colorDstFactor) &&
           (aLhs.colorEquation  == aRhs.colorEquation)  &&
           (aLhs.alphaSrcFactor == aRhs.alphaSrcFactor) &&
           (aLhs.alphaDstFactor == aRhs.alphaDstFactor) &&
           (aLhs.alphaEquation  == aRhs.alphaEquation);
}

////////////////////////////////////////////////////////////
/// \relates BlendMode
/// \brief Overload of the != operator
///
/// \param aLhs  Left operand
/// \param aRhs Right operand
///
/// \return True if blending modes are different, false if they are equal
///
////////////////////////////////////////////////////////////
constexpr
bool operator!=(const BlendMode& aLhs, const BlendMode& aRhs) {
    return !(aLhs == aRhs);
}

} // namespace gr
HOBGOBLIN_NAMESPACE_END

#include <Hobgoblin/Private/Pmacro_undef.hpp>
#include <Hobgoblin/Private/Short_namespace.hpp>

#endif // UHOBGOBLIN_GRAPHICS_BLEND_MODE_HPP

////////////////////////////////////////////////////////////
/// \class sf::BlendMode
/// \ingroup graphics
///
/// sf::BlendMode is a class that represents a blend mode. A blend
/// mode determines how the colors of an object you draw are
/// mixed with the colors that are already in the buffer.
///
/// The class is composed of 6 components, each of which has its
/// own public member variable:
/// \li %Color Source Factor (@ref colorSrcFactor)
/// \li %Color Destination Factor (@ref colorDstFactor)
/// \li %Color Blend Equation (@ref colorEquation)
/// \li Alpha Source Factor (@ref alphaSrcFactor)
/// \li Alpha Destination Factor (@ref alphaDstFactor)
/// \li Alpha Blend Equation (@ref alphaEquation)
///
/// The source factor specifies how the pixel you are drawing contributes
/// to the final color. The destination factor specifies how the pixel
/// already drawn in the buffer contributes to the final color.
///
/// The color channels RGB (red, green, blue; simply referred to as
/// color) and A (alpha; the transparency) can be treated separately. This
/// separation can be useful for specific blend modes, but most often you
/// won't need it and will simply treat the color as a single unit.
///
/// The blend factors and equations correspond to their OpenGL equivalents.
/// In general, the color of the resulting pixel is calculated according
/// to the following formula (\a src is the color of the source pixel, \a dst
/// the color of the destination pixel, the other variables correspond to the
/// public members, with the equations being + or - operators):
/// \code
/// dst.rgb = colorSrcFactor * src.rgb (colorEquation) colorDstFactor * dst.rgb
/// dst.a   = alphaSrcFactor * src.a   (alphaEquation) alphaDstFactor * dst.a
/// \endcode
/// All factors and colors are represented as floating point numbers between
/// 0 and 1. Where necessary, the result is clamped to fit in that range.
///
/// The most common blending modes are defined as constants
/// in the sf namespace:
///
/// \code
/// sf::BlendMode alphaBlending          = sf::BlendAlpha;
/// sf::BlendMode additiveBlending       = sf::BlendAdd;
/// sf::BlendMode multiplicativeBlending = sf::BlendMultiply;
/// sf::BlendMode noBlending             = sf::BlendNone;
/// \endcode
///
/// In SFML, a blend mode can be specified every time you draw a sf::Drawable
/// object to a render target. It is part of the sf::RenderStates compound
/// that is passed to the member function sf::RenderTarget::draw().
///
/// \see sf::RenderStates, sf::RenderTarget
///
////////////////////////////////////////////////////////////
