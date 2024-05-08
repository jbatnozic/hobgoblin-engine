// Copyright 2024 Jovan Batnozic. Released under MS-PL licence in Serbia.
// See https://github.com/jbatnozic/Hobgoblin?tab=readme-ov-file#licence

#include <Hobgoblin/Graphics/Text.hpp>
#include <Hobgoblin/Graphics/Font.hpp>
#include <Hobgoblin/HGExcept.hpp>

#include <SFML/Graphics/Text.hpp>

#include <cassert>
#include <new>
#include <utility>

#include "Draw_bridge.hpp"
#include "SFML_conversions.hpp"
#include "SFML_err.hpp"

#include <Hobgoblin/Private/Pmacro_define.hpp>

HOBGOBLIN_NAMESPACE_BEGIN
namespace gr {

using ImplType = sf::Text;
constexpr auto IMPL_SIZE = sizeof(ImplType);
constexpr auto IMPL_ALIGN = alignof(ImplType);
#define  IMPLOF(_obj_) (reinterpret_cast<ImplType*>(&((_obj_)._storage)))
#define CIMPLOF(_obj_) (reinterpret_cast<const ImplType*>(&((_obj_)._storage)))
#define  SELF_IMPL (IMPLOF(SELF))
#define SELF_CIMPL (CIMPLOF(SELF))

Text::Text(const Font& aFont,
           const char* aString,
           PZInteger aCharacterSize)
    : Text{aFont, std::string{aString}, aCharacterSize}
{
    static_assert(STORAGE_SIZE  == IMPL_SIZE,  "Text::STORAGE_SIZE is inadequate.");
    static_assert(STORAGE_ALIGN == IMPL_ALIGN, "Text::STORAGE_ALIGN is inadequate.");
}

Text::Text(const Font& aFont,
           const std::string& aString,
           PZInteger aCharacterSize)
    : _font{&aFont}
{
    const auto& sfFont = detail::GraphicsImplAccessor::getImplOf<sf::Font>(aFont);
    new (&_storage) ImplType(aString, sfFont, static_cast<unsigned>(aCharacterSize));
}

Text::Text(const Font& aFont,
           const UnicodeString& aString,
           PZInteger aCharacterSize)
    : _font{&aFont}
{
    const auto& sfFont = detail::GraphicsImplAccessor::getImplOf<sf::Font>(aFont);
    sf::String sfString;
    hobgoblin::detail::StoreUStringInSfString(aString, &sfString);
    new (&_storage) ImplType(sfString, sfFont, static_cast<unsigned>(aCharacterSize));
}

Text::Text(const Text& aOther)
    : _font{aOther._font}
{
    new (&_storage) ImplType(*CIMPLOF(aOther));
}

Text& Text::operator=(const Text& aOther) {
    if (this != &aOther) {
        *SELF_IMPL = *CIMPLOF(aOther);
        _font = aOther._font;
    }
    return SELF;
}

Text::Text(Text&& aOther) noexcept
    : _font{aOther._font}
{
    new (&_storage) ImplType(std::move(*IMPLOF(aOther)));
}

Text& Text::operator=(Text&& aOther) noexcept {
    if (this != &aOther) {
        *SELF_IMPL = std::move(*IMPLOF(aOther));
        _font = aOther._font;
    }
    return SELF;
}

Text::~Text() {
    SELF_IMPL->~ImplType();
}

void Text::setString(const std::string& aString) {
    SELF_IMPL->setString(aString);
}

void Text::setString(const UnicodeString& aString) {
    sf::String sfStr;
    hobgoblin::detail::StoreUStringInSfString(aString, &sfStr);
    SELF_IMPL->setString(sfStr);
}

void Text::setFont(const Font& aFont) {
    const auto& sfFont = detail::GraphicsImplAccessor::getImplOf<sf::Font>(aFont);
    SELF_IMPL->setFont(sfFont);
}

void Text::setCharacterSize(PZInteger aSize) {
    SELF_IMPL->setCharacterSize(aSize);
}

void Text::setLineSpacing(float aSpacingFactor) {
    SELF_IMPL->setLineSpacing(aSpacingFactor);
}

void Text::setLetterSpacing(float aSpacingFactor) {
    SELF_IMPL->setLetterSpacing(aSpacingFactor);
}

void Text::setStyle(std::uint32_t aStyle) {
    SELF_IMPL->setStyle(aStyle); // TODO static assert flag equality
}

void Text::setFillColor(Color aColor) {
    SELF_IMPL->setFillColor(ToSf(aColor));
}

void Text::setOutlineColor(Color aColor) {
    SELF_IMPL->setOutlineColor(ToSf(aColor));
}

void Text::setOutlineThickness(float aThickness) {
    SELF_IMPL->setOutlineThickness(aThickness);
}

UnicodeString Text::getString() const {
    const auto& sfStr = SELF_CIMPL->getString();
    UnicodeString result;
    hobgoblin::detail::LoadUStringFromSfString(result, &sfStr);
    return result;
}

const Font& Text::getFont() const {
    HG_ASSERT(_font != nullptr);
    return *_font;
}

PZInteger Text::getCharacterSize() const {
    return static_cast<PZInteger>(SELF_CIMPL->getCharacterSize());
}

float Text::getLetterSpacing() const {
    return SELF_CIMPL->getLetterSpacing();
}

float Text::getLineSpacing() const {
    return SELF_CIMPL->getLineSpacing();
}

std::uint32_t Text::getStyle() const {
    return SELF_CIMPL->getStyle();
}

Color Text::getFillColor() const {
    return ToHg(SELF_CIMPL->getFillColor());
}

Color Text::getOutlineColor() const {
    return ToHg(SELF_CIMPL->getOutlineColor());
}

float Text::getOutlineThickness() const {
    return SELF_CIMPL->getOutlineThickness();
}

math::Vector2f Text::findCharacterPos(PZInteger aIndex) const {
    return ToHg(SELF_CIMPL->findCharacterPos(pztos(aIndex)));
}

math::Rectangle<float> Text::getLocalBounds() const {
    return ToHg(SELF_CIMPL->getLocalBounds());
}

math::Rectangle<float> Text::getGlobalBounds() const {
    return ToHg(SELF_CIMPL->getGlobalBounds());
}

Drawable::BatchingType Text::getBatchingType() const {
    return Drawable::BatchingType::Custom;
}

void Text::_draw(Canvas& aCanvas, const RenderStates& aStates) const {
    const auto drawingWasSuccessful = 
        Draw(aCanvas, [this, &aStates](sf::RenderTarget& aSfRenderTarget) {
        aSfRenderTarget.draw(*CIMPLOF(*this), ToSf(aStates));
    });
    assert(drawingWasSuccessful);
}

///////////////////////////////////////////////////////////////////////////
// TRANSFORMABLE                                                         //
///////////////////////////////////////////////////////////////////////////

void Text::setPosition(float aX, float aY) {
    SELF_IMPL->setPosition(aX, aY);
}

void Text::setPosition(const math::Vector2f& aPosition) {
    SELF_IMPL->setPosition(ToSf(aPosition));
}

void Text::setRotation(math::AngleF aAngle) {
    SELF_IMPL->setRotation(-aAngle.asDeg());
}

void Text::setScale(float aFactorX, float aFactorY) {
    SELF_IMPL->setScale(aFactorX, aFactorY);
}

void Text::setScale(const math::Vector2f& aFactors) {
    SELF_IMPL->setScale(ToSf(aFactors));
}

void Text::setOrigin(float aX, float aY) {
    SELF_IMPL->setOrigin(aX, aY);
}

void Text::setOrigin(const math::Vector2f& aOrigin) {
    SELF_IMPL->setOrigin(ToSf(aOrigin));
}

math::Vector2f Text::getPosition() const {
    return ToHg(SELF_CIMPL->getPosition());
}

math::AngleF Text::getRotation() const {
    return math::AngleF::fromDegrees(-(SELF_CIMPL->getRotation()));
}

math::Vector2f Text::getScale() const {
    return ToHg(SELF_CIMPL->getScale());
}

math::Vector2f Text::getOrigin() const {
    return ToHg(SELF_CIMPL->getOrigin());
}

void Text::move(float aOffsetX, float aOffsetY) {
    SELF_IMPL->move(aOffsetX, aOffsetY);
}

void Text::move(const math::Vector2f& aOffset) {
    SELF_IMPL->move(ToSf(aOffset));
}

void Text::rotate(math::AngleF aAngle) {
    SELF_IMPL->rotate(-aAngle.asDeg());
}

void Text::scale(float aFactorX, float aFactorY) {
    SELF_IMPL->scale(aFactorX, aFactorY);
}

void Text::scale(const math::Vector2f& aFactor) {
    SELF_IMPL->scale(ToSf(aFactor));
}

Transform Text::getTransform() const {
    return ToHg(SELF_CIMPL->getTransform());
}

Transform Text::getInverseTransform() const {
    return ToHg(SELF_CIMPL->getInverseTransform());
}

///////////////////////////////////////////////////////////////////////////
// TEST - PRIVATE                                                        //
///////////////////////////////////////////////////////////////////////////

void* Text::_getSFMLImpl() {
    return SELF_IMPL;
}

const void* Text::_getSFMLImpl() const {
    return SELF_CIMPL;
}

} // namespace gr
HOBGOBLIN_NAMESPACE_END

#include <Hobgoblin/Private/Pmacro_undef.hpp>
