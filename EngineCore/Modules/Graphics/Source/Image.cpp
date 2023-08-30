
#include <Hobgoblin/Graphics/Image.hpp>

#include <SFML/Graphics/Image.hpp>

#include <new>

#include "SFML_conversions.hpp"

#include <Hobgoblin/Private/Pmacro_define.hpp>

HOBGOBLIN_NAMESPACE_BEGIN
namespace gr {

using ImplType = sf::Image;
constexpr auto IMPL_SIZE  = sizeof(ImplType);
constexpr auto IMPL_ALIGN = alignof(ImplType);
#define  IMPL (reinterpret_cast<ImplType*>(&_storage))
#define CIMPL (reinterpret_cast<const ImplType*>(&_storage))

Image::Image() {
    static_assert(STORAGE_SIZE == IMPL_SIZE,   "Image::STORAGE_SIZE is inadequate.");
    static_assert(STORAGE_ALIGN == IMPL_ALIGN, "Image::STORAGE_ALIGN is inadequate.");

    new (&_storage) ImplType();
}

Image::~Image() {
    IMPL->~ImplType();
}

void Image::create(PZInteger aWidth, PZInteger aHeight, const Color& aColor) {
    IMPL->create(
        static_cast<unsigned>(aWidth),
        static_cast<unsigned>(aHeight),
        ToSf(aColor)
    );
}

void Image::create(PZInteger aWidth, PZInteger aHeight, const std::uint8_t* aPixels) {
    IMPL->create(
        static_cast<unsigned>(aWidth),
        static_cast<unsigned>(aHeight),
        aPixels
    );
}

bool Image::loadFromFile(const std::string& aFilename) {
    return IMPL->loadFromFile(aFilename);
}

bool Image::loadFromMemory(const void* aData, std::size_t aSize) {
    return IMPL->loadFromMemory(aData, aSize);
}

bool Image::saveToFile(const std::string& aFilename) const {
    return CIMPL->saveToFile(aFilename);
}

math::Vector2pz Image::getSize() const {
    const auto size = CIMPL->getSize();
    return {
        static_cast<PZInteger>(size.x),
        static_cast<PZInteger>(size.y)
    };
}

void Image::createMaskFromColor(const Color& aColor, std::uint8_t aAlpha) {
    IMPL->createMaskFromColor(ToSf(aColor), aAlpha);
}

void Image::copy(
    const Image& source,
    PZInteger destX,
    PZInteger destY,
    const math::Rectangle<PZInteger>& sourceRect,
    bool applyAlpha
) {
    // TODO
}

void Image::setPixel(PZInteger aX, PZInteger aY, const Color& aColor) {
    IMPL->setPixel(static_cast<unsigned>(aX), static_cast<unsigned>(aY), ToSf(aColor));
}

Color Image::getPixel(PZInteger aX, PZInteger aY) const {
    return ToHg(CIMPL->getPixel(static_cast<unsigned>(aX), static_cast<unsigned>(aY)));
}

const std::uint8_t* Image::getPixelsPtr() const {
    return CIMPL->getPixelsPtr();
}

void Image::flipHorizontally() {
    IMPL->flipHorizontally();
}

void Image::flipVertically() {
    IMPL->flipVertically();
}

void* Image::_getSFMLImpl() {
    return IMPL;
}

const void* Image::_getSFMLImpl() const {
    return CIMPL;
}

} // namespace gr
HOBGOBLIN_NAMESPACE_END

#include <Hobgoblin/Private/Pmacro_undef.hpp>
