// Copyright 2024 Jovan Batnozic. Released under MS-PL licence in Serbia.
// See https://github.com/jbatnozic/Hobgoblin?tab=readme-ov-file#licence

#include <Hobgoblin/Graphics/Render_window.hpp>

#include <SFML/Graphics/RenderWindow.hpp>

#include "Multiview_rendertarget_adapter.hpp"
#include "SFML_conversions.hpp"
#include "SFML_rendertarget_adapter.hpp"

#include <cstring>
#include <iostream>
#include <new>
#include <utility>

#include <Hobgoblin/Private/Pmacro_define.hpp>

HOBGOBLIN_NAMESPACE_BEGIN
namespace gr {

// clang-format off
#define GetSfRenderWindow(_cv_) \
    (&detail::GraphicsImplAccessor::getImplOf<_cv_ sf::RenderWindow>(static_cast<_cv_ win::Window&>(SELF)))

#define GetSRTA(_cv_) \
    (reinterpret_cast<_cv_ SfmlRenderTargetAdapter*>(&_srtaStorage))

#define GetMVA(_cv_) \
    (reinterpret_cast<_cv_ MultiViewRenderTargetAdapter*>(&_mvaStorage))
// clang-format on

RenderWindow::RenderWindow() {
    // clang-format off
    static_assert(MVA_STORAGE_SIZE  == sizeof(MultiViewRenderTargetAdapter),  "RenderWindow::MVA_STORAGE_SIZE is inadequate.");
    static_assert(MVA_STORAGE_ALIGN == alignof(MultiViewRenderTargetAdapter), "RenderWindow::MVA_STORAGE_ALIGN is inadequate.");
    static_assert(SRTA_STORAGE_SIZE  == sizeof(SfmlRenderTargetAdapter),      "RenderWindow::SRTA_STORAGE_SIZE is inadequate.");
    static_assert(SRTA_STORAGE_ALIGN == alignof(SfmlRenderTargetAdapter),     "RenderWindow::SRTA_STORAGE_ALIGN is inadequate.");
    // clang-format on

    new (&_srtaStorage) SfmlRenderTargetAdapter(*GetSfRenderWindow());
    new (&_mvaStorage) MultiViewRenderTargetAdapter(*GetSRTA());
}

RenderWindow::RenderWindow(win::VideoMode              aMode,
                           const std::string&          aTitle,
                           win::WindowStyle            aStyle,
                           const win::ContextSettings& aSettings)
    : RenderWindow{} {
    Window::create(aMode, aTitle, aStyle, aSettings);

    auto& view = getView(0);
    view.setSize({static_cast<float>(aMode.width), static_cast<float>(aMode.height)});
    view.setCenter({static_cast<float>(aMode.width) / 2.f, static_cast<float>(aMode.height) / 2.f});
    view.setViewport({0.f, 0.f, 1.f, 1.f});
}

RenderWindow::RenderWindow(win::WindowHandle aHandle, const win::ContextSettings& aSettings)
    : RenderWindow{} {
    Window::create(aHandle, aSettings);

    auto&       view = getView(0);
    const auto& size = getSize();
    view.setSize({static_cast<float>(size.x), static_cast<float>(size.y)});
    view.setCenter({static_cast<float>(size.x) / 2.f, static_cast<float>(size.y) / 2.f});
    view.setViewport({0.f, 0.f, 1.f, 1.f});
}

RenderWindow::RenderWindow(RenderWindow&& aOther)
    : Window{std::move(aOther)} {
    // Replace SfmlRenderTargetAdapter
    new (&_srtaStorage) SfmlRenderTargetAdapter(*GetSfRenderWindow());

    // Replace MultiViewRenderTargetAdapter
    auto* otherMvaStoragePtr = &(aOther._mvaStorage);
    auto* otherMvaPtr        = reinterpret_cast<MultiViewRenderTargetAdapter*>(otherMvaStoragePtr);
    new (&_mvaStorage) MultiViewRenderTargetAdapter(std::move(*otherMvaPtr));
    GetMVA()->setRenderTarget(*GetSRTA());
}

RenderWindow& RenderWindow::operator=(RenderWindow&& aOther) {
    if (this != &aOther) {
        // Replace Window
        static_cast<Window&>(SELF) = std::move(static_cast<Window&>(aOther));

        // Replace SfmlRenderTargetAdapter
        GetSRTA()->~SfmlRenderTargetAdapter();
        new (&_srtaStorage) SfmlRenderTargetAdapter(*GetSfRenderWindow());

        // Replace MultiViewRenderTargetAdapter
        GetMVA()->~MultiViewRenderTargetAdapter();
        auto* otherMvaStoragePtr = &(aOther._mvaStorage);
        auto* otherMvaPtr        = reinterpret_cast<MultiViewRenderTargetAdapter*>(otherMvaStoragePtr);
        new (&_mvaStorage) MultiViewRenderTargetAdapter(std::move(*otherMvaPtr));
        GetMVA()->setRenderTarget(*GetSRTA());
    }
    return SELF;
}

RenderWindow::~RenderWindow() {
    GetSRTA()->~SfmlRenderTargetAdapter();
    GetMVA()->~MultiViewRenderTargetAdapter();
}

void RenderWindow::onCreate() {
    // TODO
}

void RenderWindow::onResize() {
    // TODO
}

///////////////////////////////////////////////////////////////////////////
// CANVAS - BASIC                                                        //
///////////////////////////////////////////////////////////////////////////

math::Vector2pz RenderWindow::getSize() const {
    return GetSRTA(const)->getSize();
}

void RenderWindow::getCanvasDetails(CanvasType& aType, void*& aRenderingBackend) {
    GetMVA()->getCanvasDetails(aType, aRenderingBackend);
}

bool RenderWindow::isSrgb() const {
    return getSettings().sRgbCapable;
}

///////////////////////////////////////////////////////////////////////////
// CANVAS - DRAWING                                                      //
///////////////////////////////////////////////////////////////////////////

void RenderWindow::clear(const Color& aColor) {
    GetSRTA()->clear(aColor);
}

void RenderWindow::draw(const Drawable& aDrawable, const RenderStates& aStates) {
    GetMVA()->draw(aDrawable, aStates);
}

void RenderWindow::draw(const Vertex*       aVertices,
                        PZInteger           aVertexCount,
                        PrimitiveType       aPrimitiveType,
                        const RenderStates& aStates) {
    GetMVA()->draw(aVertices, aVertexCount, aPrimitiveType, aStates);
}

void RenderWindow::draw(const VertexBuffer& aVertexBuffer, const RenderStates& aStates) {
    GetMVA()->draw(aVertexBuffer, aStates);
}

void RenderWindow::draw(const VertexBuffer& aVertexBuffer,
                        PZInteger           aFirstVertex,
                        PZInteger           aVertexCount,
                        const RenderStates& aStates) {
    GetMVA()->draw(aVertexBuffer, aFirstVertex, aVertexCount, aStates);
}

void RenderWindow::flush() {
    GetMVA()->flush();
}

///////////////////////////////////////////////////////////////////////////
// CANVAS - OPEN GL                                                      //
///////////////////////////////////////////////////////////////////////////

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

///////////////////////////////////////////////////////////////////////////
// VIEW CONTROLLER                                                       //
///////////////////////////////////////////////////////////////////////////

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

} // namespace gr
HOBGOBLIN_NAMESPACE_END

#include <Hobgoblin/Private/Pmacro_undef.hpp>
