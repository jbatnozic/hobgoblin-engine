// Copyright 2024 Jovan Batnozic. Released under MS-PL licence in Serbia.
// See https://github.com/jbatnozic/Hobgoblin?tab=readme-ov-file#licence

#include <Hobgoblin/Graphics/Render_texture.hpp>
#include <Hobgoblin/HGExcept.hpp>

#include <SFML/Graphics/RenderTexture.hpp>

#include <memory>
#include <new>

#include "Multiview_decorator.hpp"
#include "SFML_conversions.hpp"
#include "SFML_err.hpp"
#include "SFML_rendertarget_adapter.hpp"

#include <Hobgoblin/Private/Pmacro_define.hpp>

HOBGOBLIN_NAMESPACE_BEGIN
namespace gr {

namespace {

sf::Texture& GetUnderlyingTextureOf(sf::RenderTexture& aRenderTexture) {
    return const_cast<sf::Texture&>(aRenderTexture.getTexture());
}

} // namespace

#define GetSfRenderTexture(_cv_) (reinterpret_cast<_cv_ sf::RenderTexture*>(&_sfrtStorage))

using ImplType            = MultiViewDecorator<SfmlRenderTargetAdapter>;
constexpr auto IMPL_SIZE  = sizeof(ImplType);
constexpr auto IMPL_ALIGN = alignof(ImplType);
#define IMPLOF(_obj_)  (reinterpret_cast<ImplType*>(&((_obj_)._implStorage)))
#define CIMPLOF(_obj_) (reinterpret_cast<const ImplType*>(&((_obj_)._implStorage)))
#define SELF_IMPL      (IMPLOF(SELF))
#define SELF_CIMPL     (CIMPLOF(SELF))

RenderTexture::RenderTexture()
    : _texture{nullptr} {
    // clang-format off
    static_assert(SFRT_STORAGE_SIZE  == sizeof(sf::RenderTexture),  "RenderTexture::SFRT_STORAGE_SIZE is inadequate.");
    static_assert(SFRT_STORAGE_ALIGN == alignof(sf::RenderTexture), "RenderTexture::SFRT_STORAGE_ALIGN is inadequate.");
    static_assert(IMPL_STORAGE_SIZE  == IMPL_SIZE,  "RenderTexture::IMPL_STORAGE_SIZE is inadequate.");
    static_assert(IMPL_STORAGE_ALIGN == IMPL_ALIGN, "RenderTexture::IMPL_STORAGE_ALIGN is inadequate.");
    // clang-format on

    new (&_sfrtStorage) sf::RenderTexture();
    new (&_implStorage) ImplType(*GetSfRenderTexture());

    _texture = Texture(&GetUnderlyingTextureOf(*GetSfRenderTexture()));
}

RenderTexture::~RenderTexture() {
    _texture = Texture(nullptr);

    SELF_IMPL->~ImplType();
    GetSfRenderTexture()->~RenderTexture();
}

#if 0 // TODO(enable move)
RenderTexture::RenderTexture(RenderTexture&& aOther) noexcept {
    new (&_storage) ImplType{std::move(*IMPLOF(aOther))};
}

RenderTexture& RenderTexture::operator=(RenderTexture&& aOther) noexcept {

}
#endif

void RenderTexture::create(const math::Vector2pz& aSize, const win::ContextSettings& aSettings) {
    SFMLErrorCatcher sfErr;
    if (!GetSfRenderTexture()->create(static_cast<unsigned>(aSize.x),
                                      static_cast<unsigned>(aSize.y),
                                      ToSf(aSettings))) {
        HG_THROW_TRACED(TracedRuntimeError, 0, sfErr.getErrorMessage());
    }
}

PZInteger RenderTexture::getMaximumAntialiasingLevel() {
    return ToPz(sf::RenderTexture::getMaximumAntialiasingLevel());
}

void RenderTexture::setSmooth(bool aSmooth) {
    GetSfRenderTexture()->setSmooth(aSmooth);
}

bool RenderTexture::isSmooth() const {
    return GetSfRenderTexture(const)->isSmooth();
}

void RenderTexture::setRepeated(bool aRepeated) {
    GetSfRenderTexture()->setRepeated(aRepeated);
}

bool RenderTexture::isRepeated() const {
    return GetSfRenderTexture(const)->isRepeated();
}

void RenderTexture::generateMipmap() {
    SFMLErrorCatcher sfErr;
    if (!GetSfRenderTexture()->generateMipmap()) {
        HG_THROW_TRACED(TracedRuntimeError, 0, sfErr.getErrorMessage());
    }
}

void RenderTexture::display() {
    GetSfRenderTexture()->display();
}

const Texture& RenderTexture::getTexture() const {
    return _texture;
}

///////////////////////////////////////////////////////////////////////////
// CANVAS - BASIC                                                        //
///////////////////////////////////////////////////////////////////////////

math::Vector2pz RenderTexture::getSize() const {
    return SELF_CIMPL->getSize();
}

RenderingBackendRef RenderTexture::getRenderingBackend() {
    return SELF_IMPL->getRenderingBackend();
}

bool RenderTexture::isSrgb() const {
    return SELF_CIMPL->isSrgb();
}

///////////////////////////////////////////////////////////////////////////
// CANVAS - DRAWING                                                      //
///////////////////////////////////////////////////////////////////////////

void RenderTexture::clear(const Color& aColor) {
    SELF_IMPL->clear(aColor);
}

void RenderTexture::draw(const Drawable& aDrawable, const RenderStates& aStates) {
    SELF_IMPL->draw(aDrawable, aStates);
}

void RenderTexture::draw(const Vertex*       aVertices,
                         PZInteger           aVertexCount,
                         PrimitiveType       aPrimitiveType,
                         const RenderStates& aStates) {
    SELF_IMPL->draw(aVertices, aVertexCount, aPrimitiveType, aStates);
}

void RenderTexture::draw(const VertexBuffer& aVertexBuffer, const RenderStates& aStates) {
    SELF_IMPL->draw(aVertexBuffer, aStates);
}

void RenderTexture::draw(const VertexBuffer& aVertexBuffer,
                         PZInteger           aFirstVertex,
                         PZInteger           aVertexCount,
                         const RenderStates& aStates) {
    SELF_IMPL->draw(aVertexBuffer, aFirstVertex, aVertexCount, aStates);
}

void RenderTexture::flush() {
    SELF_IMPL->flush();
}

///////////////////////////////////////////////////////////////////////////
// CANVAS - OPEN GL                                                      //
///////////////////////////////////////////////////////////////////////////

bool RenderTexture::setActive(bool aActive) {
    return SELF_IMPL->setActive(aActive);
}

void RenderTexture::pushGLStates() {
    SELF_IMPL->pushGLStates();
}

void RenderTexture::popGLStates() {
    SELF_IMPL->popGLStates();
}

void RenderTexture::resetGLStates() {
    SELF_IMPL->resetGLStates();
}

///////////////////////////////////////////////////////////////////////////
// VIEW CONTROLLER                                                       //
///////////////////////////////////////////////////////////////////////////

void RenderTexture::setViewCount(PZInteger aViewCount) {
    SELF_IMPL->setViewCount(aViewCount);
}

void RenderTexture::setView(const View& aView) {
    SELF_IMPL->setView(aView);
}

void RenderTexture::setView(PZInteger aViewIdx, const View& aView) {
    SELF_IMPL->setView(aViewIdx, aView);
}

PZInteger RenderTexture::getViewCount() const {
    return SELF_CIMPL->getViewCount();
}

const View& RenderTexture::getView(PZInteger aViewIdx) const {
    return SELF_CIMPL->getView(aViewIdx);
}

View& RenderTexture::getView(PZInteger aViewIdx) {
    return SELF_IMPL->getView(aViewIdx);
}

View RenderTexture::getDefaultView() const {
    return SELF_CIMPL->getDefaultView();
}

math::Rectangle<PZInteger> RenderTexture::getViewport(const View& aView) const {
    return SELF_CIMPL->getViewport(aView);
}

math::Rectangle<PZInteger> RenderTexture::getViewport(PZInteger aViewIdx) const {
    return SELF_CIMPL->getViewport(aViewIdx);
}

math::Vector2f RenderTexture::mapPixelToCoords(const math::Vector2i& aPoint, const View& aView) const {
    return SELF_CIMPL->mapPixelToCoords(aPoint, aView);
}

math::Vector2f RenderTexture::mapPixelToCoords(const math::Vector2i& aPoint, PZInteger aViewIdx) const {
    return SELF_CIMPL->mapPixelToCoords(aPoint, getView(aViewIdx));
}

math::Vector2i RenderTexture::mapCoordsToPixel(const math::Vector2f& aPoint, const View& aView) const {
    return SELF_CIMPL->mapCoordsToPixel(aPoint, aView);
}

math::Vector2i RenderTexture::mapCoordsToPixel(const math::Vector2f& aPoint, PZInteger aViewIdx) const {
    return SELF_CIMPL->mapCoordsToPixel(aPoint, getView(aViewIdx));
}

///////////////////////////////////////////////////////////////////////////
// PRIVATE METHODS                                                       //
///////////////////////////////////////////////////////////////////////////

void* RenderTexture::_getSFMLImpl() {
    return GetSfRenderTexture();
}

const void* RenderTexture::_getSFMLImpl() const {
    return GetSfRenderTexture(const);
}

} // namespace gr
HOBGOBLIN_NAMESPACE_END

#include <Hobgoblin/Private/Pmacro_undef.hpp>
