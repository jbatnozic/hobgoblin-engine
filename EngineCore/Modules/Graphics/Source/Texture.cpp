#include <Hobgoblin/Graphics/Texture.hpp>

#include <SFML/Graphics/Texture.hpp>

#include "SFML_conversions.hpp"

#include <new>

#include <Hobgoblin/Private/Pmacro_define.hpp>

HOBGOBLIN_NAMESPACE_BEGIN
namespace gr {

using ImplType = sf::Texture;
constexpr auto IMPL_SIZE  = sizeof(ImplType);
constexpr auto IMPL_ALIGN = alignof(ImplType);
#define  IMPLOF(_obj_) (reinterpret_cast<ImplType*>(&((_obj_)._storage)))
#define CIMPLOF(_obj_) (reinterpret_cast<const ImplType*>(&((_obj_)._storage)))
#define  SELF_IMPL (IMPLOF(SELF))
#define SELF_CIMPL (CIMPLOF(SELF))

Texture::Texture() {
    static_assert(STORAGE_SIZE  == IMPL_SIZE,  "Sprite::STORAGE_SIZE is inadequate.");
    static_assert(STORAGE_ALIGN == IMPL_ALIGN, "Sprite::STORAGE_ALIGN is inadequate.");

    new (&_storage) ImplType();
}

Texture::Texture(const Texture& aOther) {
    new (&_storage) ImplType(*CIMPLOF(aOther));
}

Texture::~Texture() {
    SELF_IMPL->~ImplType();
}

bool Texture::create(PZInteger aWidth, PZInteger aHeight) {
    return SELF_IMPL->create(static_cast<unsigned>(aWidth), static_cast<unsigned>(aHeight));
}

bool Texture::loadFromFile(const std::filesystem::path& aPath, TextureRect aArea) {
    return SELF_IMPL->loadFromFile(FilesystemPathToSfPath(aPath),
                                   {aArea.getLeft(), aArea.getTop(), aArea.w, aArea.h});
}

bool Texture::loadFromMemory(const void* aData, PZInteger aSize, TextureRect aArea) {
    return SELF_IMPL->loadFromMemory(aData, static_cast<std::size_t>(aSize), {aArea.getLeft(), aArea.getTop(), aArea.w, aArea.h});
}

// bool Texture::loadFromStream(InputStream& stream, const math::Rectangle<int>& aArea) {} TODO

bool Texture::loadFromImage(const Image& aImage, TextureRect aArea) {
    // TODO
    return false;
}

math::Vector2pz Texture::getSize() const {
    const auto size = SELF_CIMPL->getSize();
    return {
        static_cast<PZInteger>(size.x),
        static_cast<PZInteger>(size.y)
    };
}

Image Texture::copyToImage() const {
    // TODO
    return {};
}

void Texture::update(const std::uint8_t* aPixels) {
    SELF_IMPL->update(aPixels);
}

void Texture::update(
    const std::uint8_t* aPixels,
    PZInteger aWidth,
    PZInteger aHeight,
    PZInteger aX,
    PZInteger aY
) {
    SELF_IMPL->update(
        aPixels,
        static_cast<unsigned>(aWidth),
        static_cast<unsigned>(aHeight),
        static_cast<unsigned>(aX),
        static_cast<unsigned>(aY)
    );
}

void Texture::update(const Texture& aTexture) {
    SELF_IMPL->update(*CIMPLOF(aTexture));
}

void Texture::update(const Texture& aTexture, PZInteger aX, PZInteger aY) {
    SELF_IMPL->update(*CIMPLOF(aTexture), static_cast<unsigned>(aX), static_cast<unsigned>(aY));
}

void Texture::update(const Image& aImage) {
    const auto& sfImage = detail::GraphicsImplAccessor::getImplOf<sf::Image>(aImage);
    SELF_IMPL->update(sfImage);
}

void Texture::update(const Image& aImage, PZInteger aX, PZInteger aY) {
    const auto& sfImage = detail::GraphicsImplAccessor::getImplOf<sf::Image>(aImage);
    SELF_IMPL->update(sfImage, static_cast<unsigned>(aX), static_cast<unsigned>(aY));
}

//void Texture::update(const Window& Window) {} TODO
//
//void Texture::update(const Window& aWindow, PZInteger aX, PZInteger aY) {} TODO

void Texture::setSmooth(bool aSmooth) {
    SELF_IMPL->setSmooth(aSmooth);
}

bool Texture::isSmooth() const {
    return SELF_CIMPL->isSmooth();
}

void Texture::setSrgb(bool aSRgb) {
    SELF_IMPL->setSrgb(aSRgb);
}

bool Texture::isSrgb() const {
    return SELF_CIMPL->isSrgb();
}

void Texture::setRepeated(bool aRepeated) {
    SELF_IMPL->setRepeated(aRepeated);
}

bool Texture::isRepeated() const {
    return SELF_CIMPL->isRepeated();
}

bool Texture::generateMipmap() {
    return SELF_IMPL->generateMipmap();
}

Texture& Texture::operator=(const Texture& aRhs) {
    SELF_IMPL->operator=(*CIMPLOF(aRhs));
    return SELF;
}

void Texture::swap(Texture& aOther) {
    SELF_IMPL->swap(*IMPLOF(aOther));
}

OpenGLHandle Texture::getNativeHandle() const {
    return SELF_CIMPL->getNativeHandle();
}

void Texture::bind(const Texture* aTexture, CoordinateType aCoordinateType) {
    if (!aTexture) {
        sf::Texture::bind(nullptr);
    }

    sf::Texture::bind(CIMPLOF(*aTexture), ToSf(aCoordinateType));
}

PZInteger Texture::getMaximumSize() {
    return static_cast<PZInteger>(sf::Texture::getMaximumSize());
}

void* Texture::_getSFMLImpl() {
    return SELF_IMPL;
}

const void* Texture::_getSFMLImpl() const {
    return SELF_CIMPL;
}

} // namespace gr
HOBGOBLIN_NAMESPACE_END

#include <Hobgoblin/Private/Pmacro_undef.hpp>
