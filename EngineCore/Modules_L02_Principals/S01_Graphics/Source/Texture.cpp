#include <Hobgoblin/Graphics/Texture.hpp>
#include <Hobgoblin/HGExcept.hpp>

#include <SFML/Graphics/Image.hpp>
#include <SFML/Graphics/RenderWindow.hpp>
#include <SFML/Graphics/Texture.hpp>

#include "SFML_conversions.hpp"
#include "SFML_err.hpp"

#include <cstring>
#include <new>

#include <Hobgoblin/Private/Pmacro_define.hpp>

HOBGOBLIN_NAMESPACE_BEGIN
namespace gr {

using ImplType = sf::Texture;
constexpr auto IMPL_SIZE  = sizeof(ImplType);
constexpr auto IMPL_ALIGN = alignof(ImplType);

#define IMPLOF(_obj_) ([](Texture& aTex) -> ImplType* { \
    return (aTex._isProxy) ? *reinterpret_cast<ImplType**>(&(aTex._storage)) \
                           :  reinterpret_cast<ImplType*>(&(aTex._storage)); \
\
}(_obj_))

#define CIMPLOF(_obj_) ([](const Texture& aTex) -> const ImplType* { \
    return (aTex._isProxy) ? *reinterpret_cast<const ImplType* const*>(&(aTex._storage)) \
                           :  reinterpret_cast<const ImplType*>(&(aTex._storage)); \
\
}(_obj_))

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

Texture& Texture::operator=(const Texture& aOther) {
    if (this != &aOther) {
        if (!_isProxy) {
            *SELF_IMPL = *CIMPLOF(aOther);
        }
        else {
            _isProxy = false;
            new (&_storage) ImplType(*CIMPLOF(aOther));
        }
    }
    return SELF;
}

Texture::Texture(Texture&& aOther) {
    HG_VALIDATE_ARGUMENT(!aOther._isProxy, "Moving is only allowed between non-proxy textures.");
    new (&_storage) ImplType(std::move(*IMPLOF(aOther)));
}

Texture& Texture::operator=(Texture&& aOther) {
    if (this != &aOther) {
        HG_VALIDATE_ARGUMENT(aOther._isProxy == _isProxy,
                             "Moving is not allowed between proxy and non-proxy texture.");
        // A special case, where both textures are proxies (and we just copy the pointer)
        // is allowed to enable construction of RenderTexture.
        if (_isProxy) {
            std::memcpy(&_storage, &(aOther._storage), sizeof(void*));
        } else {
            *SELF_IMPL = std::move(*IMPLOF(aOther));
        }
    }
    return SELF;
}

Texture::Texture(void* sfTexture) {
    new (&_storage) sf::Texture*{static_cast<sf::Texture*>(sfTexture)};
    _isProxy = true;
}

Texture::~Texture() {
    if (!_isProxy) {
        SELF_IMPL->~ImplType();
    }
}

void Texture::create(PZInteger aWidth, PZInteger aHeight) {
    SFMLErrorCatcher sfErr;
    if (!SELF_IMPL->create(static_cast<unsigned>(aWidth), static_cast<unsigned>(aHeight))) {
        HG_THROW_TRACED(IOError, 0, sfErr.getErrorMessage());
    }
}

void Texture::loadFromFile(const std::filesystem::path& aPath, TextureRect aArea) {
    SFMLErrorCatcher sfErr;
    if (!SELF_IMPL->loadFromFile(FilesystemPathToSfPath(aPath),
                                 {aArea.getLeft(), aArea.getTop(), aArea.w, aArea.h})) {
        HG_THROW_TRACED(IOError, 0, sfErr.getErrorMessage());
    }
}

void Texture::loadFromMemory(const void* aData, PZInteger aSize, TextureRect aArea) {
    SFMLErrorCatcher sfErr;
    if (!SELF_IMPL->loadFromMemory(aData, 
                                   static_cast<std::size_t>(aSize), 
                                   {aArea.getLeft(), aArea.getTop(), aArea.w, aArea.h})) {
        HG_THROW_TRACED(IOError, 0, sfErr.getErrorMessage());
    }
}

// bool Texture::loadFromStream(InputStream& stream, const math::Rectangle<int>& aArea) {} TODO(streams)

void Texture::loadFromImage(const Image& aImage, TextureRect aArea) {
    const auto& sfImage = detail::GraphicsImplAccessor::getImplOf<sf::Image>(aImage);

    SFMLErrorCatcher sfErr;
    if (!SELF_IMPL->loadFromImage(sfImage)) {
        HG_THROW_TRACED(IOError, 0, sfErr.getErrorMessage());
    }
}

math::Vector2pz Texture::getSize() const {
    const auto size = SELF_CIMPL->getSize();
    return {
        static_cast<PZInteger>(size.x),
        static_cast<PZInteger>(size.y)
    };
}

Image Texture::copyToImage() const {
    Image image;
    auto& sfImage = detail::GraphicsImplAccessor::getImplOf<sf::Image>(image);
    sfImage = SELF_CIMPL->copyToImage();
    return image;
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

void Texture::update(const win::Window& aWindow) {
    const auto& sfWindow = detail::GraphicsImplAccessor::getImplOf<sf::RenderWindow>(aWindow);
    SELF_IMPL->update(sfWindow);
}

void Texture::update(const win::Window& aWindow, PZInteger aX, PZInteger aY) {
    const auto& sfWindow = detail::GraphicsImplAccessor::getImplOf<sf::RenderWindow>(aWindow);
    SELF_IMPL->update(sfWindow, static_cast<unsigned>(aX), static_cast<unsigned>(aY));
}

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

void Texture::generateMipmap() {
    SFMLErrorCatcher sfErr;
    if (!SELF_IMPL->generateMipmap()) {
        HG_THROW_TRACED(TracedRuntimeError, 0, "Mipmap generation failed: {}", sfErr.getErrorMessage());
    }
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
