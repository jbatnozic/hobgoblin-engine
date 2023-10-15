#include <Hobgoblin/Graphics/Render_window.hpp>

#include <SFML/Graphics/RenderWindow.hpp>

#include "Multiview_rendertarget_adapter.hpp"
#include "SFML_conversions.hpp"
#include "SFML_rendertarget_adapter.hpp"

#include <cstring>
#include <iostream>
#include <new>

#include <Hobgoblin/Private/Pmacro_define.hpp>

HOBGOBLIN_NAMESPACE_BEGIN
namespace gr {

#define GetSfRenderWindow(_cv_) \
    (&detail::GraphicsImplAccessor::getImplOf<_cv_ sf::RenderWindow>(static_cast<_cv_ win::Window&>(SELF)))

#define GetSRTA(_cv_) \
    (reinterpret_cast<_cv_ SfmlRenderTargetAdapter*>(&_srtaStorage))

#define GetMVA(_cv_) \
    (reinterpret_cast<_cv_ MultiViewRenderTargetAdapter*>(&_mvaStorage))

RenderWindow::RenderWindow() {
    static_assert(MVA_STORAGE_SIZE  == sizeof(MultiViewRenderTargetAdapter),  "RenderWindow::MVA_STORAGE_SIZE is inadequate.");
    static_assert(MVA_STORAGE_ALIGN == alignof(MultiViewRenderTargetAdapter), "RenderWindow::MVA_STORAGE_ALIGN is inadequate.");
    static_assert(SRTA_STORAGE_SIZE  == sizeof(SfmlRenderTargetAdapter),      "RenderWindow::SRTA_STORAGE_SIZE is inadequate.");
    static_assert(SRTA_STORAGE_ALIGN == alignof(SfmlRenderTargetAdapter),     "RenderWindow::SRTA_STORAGE_ALIGN is inadequate.");

    new (&_srtaStorage) SfmlRenderTargetAdapter(*GetSfRenderWindow());
    new (&_mvaStorage) MultiViewRenderTargetAdapter(*GetSRTA());
}

RenderWindow::RenderWindow(
    win::VideoMode aMode,
    const std::string& aTitle,
    win::WindowStyle aStyle,
    const win::ContextSettings& aSettings
)
    : RenderWindow()
{
    Window::create(aMode, aTitle, aStyle, aSettings);
}

RenderWindow::RenderWindow(win::WindowHandle aHandle, const win::ContextSettings& aSettings)
    : RenderWindow()
{
    Window::create(aHandle, aSettings);
}

RenderWindow::~RenderWindow() {
    GetSRTA()->~SfmlRenderTargetAdapter();
    GetMVA()->~MultiViewRenderTargetAdapter();
}

bool RenderWindow::isSrgb() const {
    return getSettings().sRgbCapable;
}

void RenderWindow::onCreate() {
    // TODO
}

void RenderWindow::onResize() {
    // TODO
}

// RENDER TARGET INTERFACE

void RenderWindow::draw(const Drawable& aDrawable,
                        const RenderStates& aStates) {
    GetMVA()->draw(aDrawable, aStates);
}

void RenderWindow::draw(const Vertex* aVertices,
                        PZInteger aVertexCount,
                        PrimitiveType aPrimitiveType,
                        const RenderStates& aStates) {
    GetMVA()->draw(aVertices, aVertexCount, aPrimitiveType, aStates);
}

void RenderWindow::draw(const VertexBuffer& aVertexBuffer,
                        const RenderStates& aStates) {
    GetMVA()->draw(aVertexBuffer, aStates);
}

void RenderWindow::draw(const VertexBuffer& aVertexBuffer,
                        PZInteger aFirstVertex,
                        PZInteger aVertexCount,
                        const RenderStates& aStates) {
    GetMVA()->draw(aVertexBuffer, aFirstVertex, aVertexCount, aStates);
}

void RenderWindow::flush() {
    GetMVA()->flush();
}

void RenderWindow::getCanvasDetails(CanvasType& aType, void*& aRenderingBackend) {
    aType = CanvasType::Proxy;
    aRenderingBackend = static_cast<Canvas*>(GetMVA());
}

void RenderWindow::clear(const Color& aColor) {
    GetSRTA()->clear(aColor);
}

void RenderWindow::setViewCount(PZInteger aViewCount) {
    GetMVA()->setViewCount(aViewCount);
}

void RenderWindow::setView(const View& aView) {
    GetMVA()->getView(0) = aView;
}

void RenderWindow::setView(PZInteger aViewIdx, const View& aView) {
    GetMVA()->getView(aViewIdx) = aView;
}

PZInteger RenderWindow::getViewCount() const {
    return GetMVA(const)->getViewCount();
}

const View& RenderWindow::getView(PZInteger aViewIdx) const {
    return GetMVA(const)->getView(aViewIdx);
}

View& RenderWindow::getView(PZInteger aViewIdx) {
    return GetMVA()->getView(aViewIdx);
}

View RenderWindow::getDefaultView() const {
    return GetSRTA(const)->getDefaultView();
}

math::Rectangle<PZInteger> RenderWindow::getViewport(const View& aView) const {
    return GetSRTA(const)->getViewport(aView);
}

math::Rectangle<PZInteger> RenderWindow::getViewport(PZInteger aViewIdx) const {
    return GetSRTA(const)->getViewport(getView(aViewIdx));
}

math::Vector2f RenderWindow::mapPixelToCoords(const math::Vector2i& aPoint, const View& aView) const {
    return GetSRTA(const)->mapPixelToCoords(aPoint, aView);
}

math::Vector2f RenderWindow::mapPixelToCoords(const math::Vector2i& aPoint, PZInteger aViewIdx) const {
    return GetSRTA(const)->mapPixelToCoords(aPoint, getView(aViewIdx));
}

math::Vector2i RenderWindow::mapCoordsToPixel(const math::Vector2f& aPoint, const View& aView) const {
    return GetSRTA(const)->mapCoordsToPixel(aPoint, aView);
}

math::Vector2i RenderWindow::mapCoordsToPixel(const math::Vector2f& aPoint, PZInteger aViewIdx) const {
    return GetSRTA(const)->mapCoordsToPixel(aPoint, getView(aViewIdx));
}

math::Vector2pz RenderWindow::getSize() const {
    return GetSRTA(const)->getSize();
}

bool RenderWindow::setActive(bool aActive) {
    return GetSRTA()->setActive(aActive);
}

void RenderWindow::pushGLStates() {
    GetSRTA()->pushGLStates();
}

void RenderWindow::popGLStates() {
    GetSRTA()->popGLStates();
}

void RenderWindow::resetGLStates() {
    GetSRTA()->resetGLStates();
}

} // namespace gr
HOBGOBLIN_NAMESPACE_END

#include <Hobgoblin/Private/Pmacro_undef.hpp>
