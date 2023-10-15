
#include <Hobgoblin/Graphics/Image.hpp>

#include <SFML/Graphics/Image.hpp>

#include <new>
#include <utility>

#include "SFML_conversions.hpp"

#include <Hobgoblin/Private/Pmacro_define.hpp>

HOBGOBLIN_NAMESPACE_BEGIN
namespace gr {

using ImplType = sf::Image;
constexpr auto IMPL_SIZE  = sizeof(ImplType);
constexpr auto IMPL_ALIGN = alignof(ImplType);
#define  IMPLOF(_obj_) (reinterpret_cast<ImplType*>(&((_obj_)._storage)))
#define CIMPLOF(_obj_) (reinterpret_cast<const ImplType*>(&((_obj_)._storage)))
#define  SELF_IMPL (IMPLOF(SELF))
#define SELF_CIMPL (CIMPLOF(SELF))

Image::Image() {
    static_assert(STORAGE_SIZE == IMPL_SIZE,   "Image::STORAGE_SIZE is inadequate.");
    static_assert(STORAGE_ALIGN == IMPL_ALIGN, "Image::STORAGE_ALIGN is inadequate.");

    new (&_storage) ImplType();
}

Image::Image(const Image& aOther) {
    new (&_storage) ImplType(*CIMPLOF(aOther));
}

Image& Image::operator=(const Image& aOther) {
    if (this != &aOther) {
        *SELF_IMPL = *CIMPLOF(aOther);
    }
    return SELF;
}

Image::Image(Image&& aOther) noexcept {
    new (&_storage) ImplType(std::move(*IMPLOF(aOther)));
}

Image& Image::operator=(Image&& aOther) noexcept {
    if (this != &aOther) {
        *SELF_IMPL = std::move(*IMPLOF(aOther));
    }
    return SELF;
}

Image::~Image() {
    SELF_IMPL->~ImplType();
}

void Image::create(PZInteger aWidth, PZInteger aHeight, const Color& aColor) {
    SELF_IMPL->create(
        static_cast<unsigned>(aWidth),
        static_cast<unsigned>(aHeight),
        ToSf(aColor)
    );
}

void Image::create(PZInteger aWidth, PZInteger aHeight, const std::uint8_t* aPixels) {
    SELF_IMPL->create(
        static_cast<unsigned>(aWidth),
        static_cast<unsigned>(aHeight),
        aPixels
    );
}

bool Image::loadFromFile(const std::filesystem::path& aPath) {
    return SELF_IMPL->loadFromFile(FilesystemPathToSfPath(aPath));
}

bool Image::loadFromMemory(const void* aData, std::size_t aSize) {
    return SELF_IMPL->loadFromMemory(aData, aSize);
}

bool Image::saveToFile(const std::filesystem::path& aPath) const {
    return SELF_CIMPL->saveToFile(FilesystemPathToSfPath(aPath));
}

math::Vector2pz Image::getSize() const {
    const auto size = SELF_CIMPL->getSize();
    return {
        static_cast<PZInteger>(size.x),
        static_cast<PZInteger>(size.y)
    };
}

void Image::createMaskFromColor(const Color& aColor, std::uint8_t aAlpha) {
    SELF_IMPL->createMaskFromColor(ToSf(aColor), aAlpha);
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
    SELF_IMPL->setPixel(static_cast<unsigned>(aX), static_cast<unsigned>(aY), ToSf(aColor));
}

Color Image::getPixel(PZInteger aX, PZInteger aY) const {
    return ToHg(SELF_CIMPL->getPixel(static_cast<unsigned>(aX), static_cast<unsigned>(aY)));
}

const std::uint8_t* Image::getPixelsPtr() const {
    return SELF_CIMPL->getPixelsPtr();
}

void Image::flipHorizontally() {
    SELF_IMPL->flipHorizontally();
}

void Image::flipVertically() {
    SELF_IMPL->flipVertically();
}

void* Image::_getSFMLImpl() {
    return SELF_IMPL;
}

const void* Image::_getSFMLImpl() const {
    return SELF_CIMPL;
}

} // namespace gr
HOBGOBLIN_NAMESPACE_END

#include <Hobgoblin/Private/Pmacro_undef.hpp>
