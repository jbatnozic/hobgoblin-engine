#include <Hobgoblin/Graphics/Render_window.hpp>

#include <SFML/Graphics/RenderWindow.hpp>

#include "Multiview_rendertarget_adapter.hpp"
#include "SFML_conversions.hpp"

#include <cstring>
#include <iostream>

#include <Hobgoblin/Private/Pmacro_define.hpp>

HOBGOBLIN_NAMESPACE_BEGIN
namespace gr {

#define GetSfRenderWindow(_cv_) \
    (&detail::GraphicsImplAccessor::getImplOf<_cv_ sf::RenderWindow>(static_cast<_cv_ Window&>(SELF)))

#define GetMVA(_cv_) \
    (reinterpret_cast<_cv_ MultiViewRenderTargetAdapter*>(&_mvaStorage))

RenderWindow::RenderWindow() {
    static_assert(MVA_STORAGE_SIZE  == sizeof(MultiViewRenderTargetAdapter),  "RenderWindow::MVA_STORAGE_SIZE is inadequate.");
    static_assert(MVA_STORAGE_ALIGN == alignof(MultiViewRenderTargetAdapter), "RenderWindow::MVA_STORAGE_ALIGN is inadequate.");

    new (&_mvaStorage) MultiViewRenderTargetAdapter(*GetSfRenderWindow());
}

RenderWindow::RenderWindow(VideoMode aMode, const std::string& aTitle, WindowStyle aStyle, const ContextSettings& aSettings)
    : RenderWindow()
{
    Window::create(aMode, aTitle, aStyle, aSettings);
}

RenderWindow::RenderWindow(WindowHandle aHandle, const ContextSettings& aSettings)
    : RenderWindow()
{
    Window::create(aHandle, aSettings);
}

RenderWindow::~RenderWindow() {
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

void RenderWindow::clear(const Color& aColor) {
    GetSfRenderWindow()->clear(ToSf(aColor));
}

void RenderWindow::setViewCount(PZInteger aViewCount) {
    GetMVA()->setViewCount(aViewCount);
}

void RenderWindow::setView(const View& aView, PZInteger aViewIdx) {
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
    return ToHg(GetSfRenderWindow(const)->getDefaultView());
}

math::Rectangle<PZInteger> RenderWindow::getViewport(const View& aView) const {
    const auto viewport = GetSfRenderWindow(const)->getViewport(ToSf(aView));
    return {
        viewport.left,
        viewport.top,
        viewport.width,
        viewport.height
    };
}

math::Rectangle<PZInteger> RenderWindow::getViewport(PZInteger aViewIdx) const {
    // TODO
    return {};
}

math::Vector2f RenderWindow::mapPixelToCoords(const math::Vector2i& aPoint, const View& aView) const {
    // TODO
    return {};
}

math::Vector2f RenderWindow::mapPixelToCoords(const math::Vector2i& aPoint, PZInteger aViewIdx) const {
    // TODO
    return {};
}

math::Vector2i RenderWindow::mapCoordsToPixel(const math::Vector2f& point, const View& view) const {
    // TODO
    return {};
}

math::Vector2i RenderWindow::mapCoordsToPixel(const math::Vector2f& point, PZInteger aViewIdx) const {
    // TODO
    return {};
}

math::Vector2pz RenderWindow::getSize() const {
    // TODO
    return {};
}

bool RenderWindow::setActive(bool aActive) {
    return GetSfRenderWindow()->setActive(aActive);
}

void RenderWindow::pushGLStates() {
    GetSfRenderWindow()->pushGLStates();
}

void RenderWindow::popGLStates() {
    GetSfRenderWindow()->popGLStates();
}

void RenderWindow::resetGLStates() {
    GetSfRenderWindow()->resetGLStates();
}

} // namespace gr
HOBGOBLIN_NAMESPACE_END

#include <Hobgoblin/Private/Pmacro_undef.hpp>
