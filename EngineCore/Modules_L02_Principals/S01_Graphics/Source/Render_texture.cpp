// Copyright 2024 Jovan Batnozic. Released under MS-PL licence in Serbia.
// See https://github.com/jbatnozic/Hobgoblin?tab=readme-ov-file#licence

// clang-format off

#include <Hobgoblin/Graphics/Render_texture.hpp>
#include <Hobgoblin/HGExcept.hpp>

#include <SFML/Graphics/RenderTexture.hpp>

#include <memory>
#include <new>

#include "Multiview_rendertarget_adapter.hpp"
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

#define GetSfRenderTarget(_cv_) \
    (reinterpret_cast<_cv_ sf::RenderTarget*>(&_storage))

#define GetSRTA(_cv_) \
    (reinterpret_cast<_cv_ SfmlRenderTargetAdapter*>(&_srtaStorage))

#define GetMVA(_cv_) \
    (reinterpret_cast<_cv_ MultiViewRenderTargetAdapter*>(&_mvaStorage))

using ImplType = sf::RenderTexture;
constexpr auto IMPL_SIZE  = sizeof(ImplType);
constexpr auto IMPL_ALIGN = alignof(ImplType);
#define  IMPLOF(_obj_) (reinterpret_cast<ImplType*>(&((_obj_)._storage)))
#define CIMPLOF(_obj_) (reinterpret_cast<const ImplType*>(&((_obj_)._storage)))
#define  SELF_IMPL (IMPLOF(SELF))
#define SELF_CIMPL (CIMPLOF(SELF))

RenderTexture::RenderTexture()
    : _texture{nullptr}
{
    static_assert(SFRT_STORAGE_SIZE  == IMPL_SIZE,  "RenderTexture::SFRT_STORAGE_SIZE is inadequate.");
    static_assert(SFRT_STORAGE_ALIGN == IMPL_ALIGN, "RenderTexture::SFRT_STORAGE_ALIGN is inadequate.");
    static_assert(MVA_STORAGE_SIZE   == sizeof(MultiViewRenderTargetAdapter),  "RenderTexture::MVA_STORAGE_SIZE is inadequate.");
    static_assert(MVA_STORAGE_ALIGN  == alignof(MultiViewRenderTargetAdapter), "RenderTexture::MVA_STORAGE_ALIGN is inadequate.");
    static_assert(SRTA_STORAGE_SIZE  == sizeof(SfmlRenderTargetAdapter),       "RenderTexture::SRTA_STORAGE_SIZE is inadequate.");
    static_assert(SRTA_STORAGE_ALIGN == alignof(SfmlRenderTargetAdapter),      "RenderTexture::SRTA_STORAGE_ALIGN is inadequate.");

    new (&_storage) ImplType();
    new (&_srtaStorage) SfmlRenderTargetAdapter(*GetSfRenderTarget());
    new (&_mvaStorage) MultiViewRenderTargetAdapter(*GetSRTA());

    _texture = Texture(&GetUnderlyingTextureOf(*SELF_IMPL));
}

RenderTexture::~RenderTexture() {
    _texture = Texture(nullptr);

    GetMVA()->~MultiViewRenderTargetAdapter();
    GetSRTA()->~SfmlRenderTargetAdapter();
    SELF_IMPL->~ImplType();
}

#if 0 // TODO(enable move)
RenderTexture::RenderTexture(RenderTexture&& aOther) noexcept {
    new (&_storage) ImplType{std::move(*IMPLOF(aOther))};
}

RenderTexture& RenderTexture::operator=(RenderTexture&& aOther) noexcept {

}
#endif

void RenderTexture::create(const math::Vector2pz& aSize,
                           const win::ContextSettings& aSettings) {
    SFMLErrorCatcher sfErr;
    if (!SELF_IMPL->create(static_cast<unsigned>(aSize.x),
                           static_cast<unsigned>(aSize.y),
                           ToSf(aSettings))) {
        HG_THROW_TRACED(TracedRuntimeError, 0, sfErr.getErrorMessage());
    }
}

PZInteger RenderTexture::getMaximumAntialiasingLevel() {
    return ToPz(sf::RenderTexture::getMaximumAntialiasingLevel());
}

void RenderTexture::setSmooth(bool aSmooth) {
    SELF_IMPL->setSmooth(aSmooth);
}

bool RenderTexture::isSmooth() const {
    return SELF_CIMPL->isSmooth();
}

void RenderTexture::setRepeated(bool aRepeated) {
    SELF_IMPL->setRepeated(aRepeated);
}

bool RenderTexture::isRepeated() const {
    return SELF_CIMPL->isRepeated();
}

void RenderTexture::generateMipmap() {
    SFMLErrorCatcher sfErr;
    if (!SELF_IMPL->generateMipmap()) {
        HG_THROW_TRACED(TracedRuntimeError, 0, sfErr.getErrorMessage());
    }
}

void RenderTexture::display() {
    SELF_IMPL->display();
}

const Texture& RenderTexture::getTexture() const {
    return _texture;
}

///////////////////////////////////////////////////////////////////////////
// RENDER TARGET                                                         //
///////////////////////////////////////////////////////////////////////////

void RenderTexture::draw(const Drawable& aDrawable,
                         const RenderStates& aStates) {
    GetMVA()->draw(aDrawable, aStates);
}

void RenderTexture::draw(const Vertex* aVertices,
                         PZInteger aVertexCount,
                         PrimitiveType aPrimitiveType,
                         const RenderStates& aStates) {
    GetMVA()->draw(aVertices, aVertexCount, aPrimitiveType, aStates);
}

void RenderTexture::draw(const VertexBuffer& aVertexBuffer,
                         const RenderStates& aStates) {
    GetMVA()->draw(aVertexBuffer, aStates);
}

void RenderTexture::draw(const VertexBuffer& aVertexBuffer,
                         PZInteger aFirstVertex,
                         PZInteger aVertexCount,
                         const RenderStates& aStates) {
    GetMVA()->draw(aVertexBuffer, aFirstVertex, aVertexCount, aStates);
}

void RenderTexture::flush() {
    GetMVA()->flush();
}

void RenderTexture::getCanvasDetails(CanvasType& aType, void*& aRenderingBackend) {
    GetMVA()->getCanvasDetails(aType, aRenderingBackend);
}

void RenderTexture::clear(const Color& aColor) {
    GetSRTA()->clear(aColor);
}

void RenderTexture::setViewCount(PZInteger aViewCount) {
    GetMVA()->setViewCount(aViewCount);
}

void RenderTexture::setView(const View& aView) {
    GetMVA()->getView(0) = aView;
}

void RenderTexture::setView(PZInteger aViewIdx, const View& aView) {
    GetMVA()->getView(aViewIdx) = aView;
}

PZInteger RenderTexture::getViewCount() const {
    return GetMVA(const)->getViewCount();
}

const View& RenderTexture::getView(PZInteger aViewIdx) const {
    return GetMVA(const)->getView(aViewIdx);
}

View& RenderTexture::getView(PZInteger aViewIdx) {
    return GetMVA()->getView(aViewIdx);
}

View RenderTexture::getDefaultView() const {
    return GetSRTA(const)->getDefaultView();
}

math::Rectangle<PZInteger> RenderTexture::getViewport(const View& aView) const {
    return GetSRTA(const)->getViewport(aView);
}

math::Rectangle<PZInteger> RenderTexture::getViewport(PZInteger aViewIdx) const {
    return GetSRTA(const)->getViewport(getView(aViewIdx));
}

math::Vector2f RenderTexture::mapPixelToCoords(const math::Vector2i& aPoint, const View& aView) const {
    return GetSRTA(const)->mapPixelToCoords(aPoint, aView);
}

math::Vector2f RenderTexture::mapPixelToCoords(const math::Vector2i& aPoint, PZInteger aViewIdx) const {
    return GetSRTA(const)->mapPixelToCoords(aPoint, getView(aViewIdx));
}

math::Vector2i RenderTexture::mapCoordsToPixel(const math::Vector2f& aPoint, const View& aView) const {
    return GetSRTA(const)->mapCoordsToPixel(aPoint, aView);
}

math::Vector2i RenderTexture::mapCoordsToPixel(const math::Vector2f& aPoint, PZInteger aViewIdx) const {
    return GetSRTA(const)->mapCoordsToPixel(aPoint, getView(aViewIdx));
}

math::Vector2pz RenderTexture::getSize() const {
    return GetSRTA(const)->getSize();
}

bool RenderTexture::setActive(bool aActive) {
    return GetSRTA()->setActive(aActive);
}

void RenderTexture::pushGLStates() {
    GetSRTA()->pushGLStates();
}

void RenderTexture::popGLStates() {
    GetSRTA()->popGLStates();
}

void RenderTexture::resetGLStates() {
    GetSRTA()->resetGLStates();
}

bool RenderTexture::isSrgb() const {
    return GetSRTA(const)->isSrgb();
}

///////////////////////////////////////////////////////////////////////////
// RENDERTEXTURE - PRIVATE                                               //
///////////////////////////////////////////////////////////////////////////

void* RenderTexture::_getSFMLImpl() {
    return SELF_IMPL;
}

const void* RenderTexture::_getSFMLImpl() const {
    return SELF_CIMPL;
}

} // namespace gr
HOBGOBLIN_NAMESPACE_END

#include <Hobgoblin/Private/Pmacro_undef.hpp>

// clang-format on
