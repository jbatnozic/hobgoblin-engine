// Copyright 2024 Jovan Batnozic. Released under MS-PL licence in Serbia.
// See https://github.com/jbatnozic/Hobgoblin?tab=readme-ov-file#licence

#include <Hobgoblin/Graphics/Render_window.hpp>

#include <SFML/Graphics/RenderWindow.hpp>

#include "Multiview_decorator.hpp"
#include "SFML_conversions.hpp"
#include "SFML_rendertarget_adapter.hpp"

#include <cstring>
#include <iostream>
#include <new>
#include <utility>

#include <Hobgoblin/Private/Pmacro_define.hpp>

HOBGOBLIN_NAMESPACE_BEGIN
namespace gr {

using ImplType            = MultiViewDecorator<SfmlRenderTargetAdapter>;
constexpr auto IMPL_SIZE  = sizeof(ImplType);
constexpr auto IMPL_ALIGN = alignof(ImplType);
#define IMPLOF(_obj_)  (reinterpret_cast<ImplType*>(&((_obj_)._storage)))
#define CIMPLOF(_obj_) (reinterpret_cast<const ImplType*>(&((_obj_)._storage)))
#define SELF_IMPL      (IMPLOF(SELF))
#define SELF_CIMPL     (CIMPLOF(SELF))

// clang-format off
#define GetSfRenderWindow() \
    (&detail::GraphicsImplAccessor::getImplOf<sf::RenderWindow>(static_cast<win::Window&>(SELF)))
// clang-format on

RenderWindow::RenderWindow() {
    // clang-format off
    static_assert(STORAGE_SIZE  == sizeof(ImplType),  "RenderWindow::STORAGE_SIZE is inadequate.");
    static_assert(STORAGE_ALIGN == alignof(ImplType), "RenderWindow::STORAGE_ALIGN is inadequate.");
    // clang-format on

    new (&_storage) ImplType(*GetSfRenderWindow());
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
    *SELF_IMPL = std::move(*IMPLOF(aOther));
    SELF_IMPL->setSFMLRenderTarget(*GetSfRenderWindow());
}

RenderWindow& RenderWindow::operator=(RenderWindow&& aOther) {
    if (this != &aOther) {
        // Replace Window
        static_cast<Window&>(SELF) = std::move(static_cast<Window&>(aOther));

        // Replace Adapter
        *SELF_IMPL = std::move(*IMPLOF(aOther));
        SELF_IMPL->setSFMLRenderTarget(*GetSfRenderWindow());
    }
    return SELF;
}

RenderWindow::~RenderWindow() {
    SELF_IMPL->~ImplType();
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
    return SELF_CIMPL->getSize();
}

RenderingBackendRef RenderWindow::getRenderingBackend() {
    return SELF_IMPL->getRenderingBackend();
}

bool RenderWindow::isSrgb() const {
    return getSettings().sRgbCapable;
}

///////////////////////////////////////////////////////////////////////////
// CANVAS - DRAWING                                                      //
///////////////////////////////////////////////////////////////////////////

void RenderWindow::clear(const Color& aColor) {
    SELF_IMPL->clear(aColor);
}

void RenderWindow::draw(const Drawable& aDrawable, const RenderStates& aStates) {
    SELF_IMPL->draw(aDrawable, aStates);
}

void RenderWindow::draw(const Vertex*       aVertices,
                        PZInteger           aVertexCount,
                        PrimitiveType       aPrimitiveType,
                        const RenderStates& aStates) {
    SELF_IMPL->draw(aVertices, aVertexCount, aPrimitiveType, aStates);
}

void RenderWindow::draw(const VertexBuffer& aVertexBuffer, const RenderStates& aStates) {
    SELF_IMPL->draw(aVertexBuffer, aStates);
}

void RenderWindow::draw(const VertexBuffer& aVertexBuffer,
                        PZInteger           aFirstVertex,
                        PZInteger           aVertexCount,
                        const RenderStates& aStates) {
    SELF_IMPL->draw(aVertexBuffer, aFirstVertex, aVertexCount, aStates);
}

void RenderWindow::flush() {
    SELF_IMPL->flush();
}

///////////////////////////////////////////////////////////////////////////
// CANVAS - OPEN GL                                                      //
///////////////////////////////////////////////////////////////////////////

bool RenderWindow::setActive(bool aActive) {
    return SELF_IMPL->setActive(aActive);
}

void RenderWindow::pushGLStates() {
    SELF_IMPL->pushGLStates();
}

void RenderWindow::popGLStates() {
    SELF_IMPL->popGLStates();
}

void RenderWindow::resetGLStates() {
    SELF_IMPL->resetGLStates();
}

///////////////////////////////////////////////////////////////////////////
// VIEW CONTROLLER                                                       //
///////////////////////////////////////////////////////////////////////////

void RenderWindow::setViewCount(PZInteger aViewCount) {
    SELF_IMPL->setViewCount(aViewCount);
}

void RenderWindow::setView(const View& aView) {
    SELF_IMPL->setView(aView);
}

void RenderWindow::setView(PZInteger aViewIdx, const View& aView) {
    SELF_IMPL->setView(aViewIdx, aView);
}

PZInteger RenderWindow::getViewCount() const {
    return SELF_CIMPL->getViewCount();
}

const View& RenderWindow::getView(PZInteger aViewIdx) const {
    return SELF_CIMPL->getView(aViewIdx);
}

View& RenderWindow::getView(PZInteger aViewIdx) {
    return SELF_IMPL->getView(aViewIdx);
}

View RenderWindow::getDefaultView() const {
    return SELF_CIMPL->getDefaultView();
}

math::Rectangle<PZInteger> RenderWindow::getViewport(const View& aView) const {
    return SELF_CIMPL->getViewport(aView);
}

math::Rectangle<PZInteger> RenderWindow::getViewport(PZInteger aViewIdx) const {
    return SELF_CIMPL->getViewport(aViewIdx);
}

math::Vector2f RenderWindow::mapPixelToCoords(const math::Vector2i& aPoint, const View& aView) const {
    return SELF_CIMPL->mapPixelToCoords(aPoint, aView);
}

math::Vector2f RenderWindow::mapPixelToCoords(const math::Vector2i& aPoint, PZInteger aViewIdx) const {
    return SELF_CIMPL->mapPixelToCoords(aPoint, aViewIdx);
}

math::Vector2i RenderWindow::mapCoordsToPixel(const math::Vector2f& aPoint, const View& aView) const {
    return SELF_CIMPL->mapCoordsToPixel(aPoint, aView);
}

math::Vector2i RenderWindow::mapCoordsToPixel(const math::Vector2f& aPoint, PZInteger aViewIdx) const {
    return SELF_CIMPL->mapCoordsToPixel(aPoint, aViewIdx);
}

} // namespace gr
HOBGOBLIN_NAMESPACE_END

#include <Hobgoblin/Private/Pmacro_undef.hpp>
