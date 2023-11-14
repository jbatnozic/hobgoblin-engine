
#include "SFML_rendertarget_adapter.hpp"

#include <Hobgoblin/HGExcept.hpp>

#include "SFML_conversions.hpp"
#include "SFML_vertices.hpp"

#include <Hobgoblin/Private/Pmacro_define.hpp>

HOBGOBLIN_NAMESPACE_BEGIN
namespace gr {

SfmlRenderTargetAdapter::SfmlRenderTargetAdapter(sf::RenderTarget& aRenderTarget)
    : _renderTarget{aRenderTarget}
{
}

///////////////////////////////////////////////////////////////////////////
// RENDERTARGET                                                          //
///////////////////////////////////////////////////////////////////////////

void SfmlRenderTargetAdapter::draw(const Drawable& aDrawable,
                                   const RenderStates& aStates) {
    aDrawable._draw(SELF, aStates);
}

void SfmlRenderTargetAdapter::draw(const Vertex* aVertices,
                                   PZInteger aVertexCount,
                                   PrimitiveType aPrimitiveType,
                                   const RenderStates& aStates) {
    SFMLVertices sfVertices{aVertices, pztos(aVertexCount)};

    _renderTarget.draw(
        sfVertices.getVertices(),
        sfVertices.getVertexCount(),
        ToSf(aPrimitiveType),
        ToSf(aStates)
    );
}

void SfmlRenderTargetAdapter::draw(const VertexBuffer& aVertexBuffer,
                                   const RenderStates& aStates) {
    // TODO
}

void SfmlRenderTargetAdapter::draw(const VertexBuffer& aVertexBuffer,
                                   PZInteger aFirstVertex,
                                   PZInteger aVertexCount,
                                   const RenderStates& aStates) {
    // TODO
}

void SfmlRenderTargetAdapter::flush() {
    /* Nothing to do (there is no batching in SFML). */
}

void SfmlRenderTargetAdapter::getCanvasDetails(CanvasType& aType, void*& aRenderingBackend) {
    aType = CanvasType::SFML;
    aRenderingBackend = &_renderTarget;
}

void SfmlRenderTargetAdapter::clear(const Color& aColor) {
    _renderTarget.clear(ToSf(aColor));
}

void SfmlRenderTargetAdapter::setViewCount(PZInteger aViewCount) {
    HG_UNREACHABLE("Forbidden to call method.");
}

void SfmlRenderTargetAdapter::setView(const View& aView) {
    _renderTarget.setView(ToSf(aView));
}

void SfmlRenderTargetAdapter::setView(PZInteger aViewIdx, const View& aView) {
    HG_UNREACHABLE("Forbidden to call method.");
}

PZInteger SfmlRenderTargetAdapter::getViewCount() const {
    HG_UNREACHABLE("Forbidden to call method.");
}

const View& SfmlRenderTargetAdapter::getView(PZInteger aViewIdx) const {
    HG_UNREACHABLE("Forbidden to call method.");
}

View& SfmlRenderTargetAdapter::getView(PZInteger aViewIdx) {
    HG_UNREACHABLE("Forbidden to call method.");
}

View SfmlRenderTargetAdapter::getDefaultView() const {
    return ToHg(_renderTarget.getDefaultView());
}

math::Rectangle<PZInteger> SfmlRenderTargetAdapter::getViewport(const View& aView) const {
    const auto& sfView = ToSf(aView);
    const auto viewport = _renderTarget.getViewport(sfView);
    return {
        static_cast<PZInteger>(viewport.left),
        static_cast<PZInteger>(viewport.top),
        static_cast<PZInteger>(viewport.width),
        static_cast<PZInteger>(viewport.height)
    };
}

math::Rectangle<PZInteger> SfmlRenderTargetAdapter::getViewport(PZInteger aViewIdx) const {
    HG_UNREACHABLE("Forbidden to call method.");
}

math::Vector2f SfmlRenderTargetAdapter::mapPixelToCoords(const math::Vector2i& aPoint, PZInteger aViewIdx) const {
    HG_UNREACHABLE("Forbidden to call method.");
}

math::Vector2f SfmlRenderTargetAdapter::mapPixelToCoords(const math::Vector2i& aPoint, const View& aView) const {
    const auto pt = _renderTarget.mapPixelToCoords(ToSf(aPoint), ToSf(aView));
    return ToHg(pt);
}

math::Vector2i SfmlRenderTargetAdapter::mapCoordsToPixel(const math::Vector2f& aPoint, const View& aView) const {
    const auto pt = _renderTarget.mapCoordsToPixel(ToSf(aPoint), ToSf(aView));
    return ToHg(pt);
}

math::Vector2i SfmlRenderTargetAdapter::mapCoordsToPixel(const math::Vector2f& aPoint, PZInteger aViewIdx) const {
    HG_UNREACHABLE("Forbidden to call method.");
}

math::Vector2pz SfmlRenderTargetAdapter::getSize() const {
    const auto size = _renderTarget.getSize();
    return {
        static_cast<PZInteger>(size.x),
        static_cast<PZInteger>(size.y)
    };
}

bool SfmlRenderTargetAdapter::setActive(bool aActive) {
    return _renderTarget.setActive();
}

void SfmlRenderTargetAdapter::pushGLStates() {
    _renderTarget.pushGLStates();
}

void SfmlRenderTargetAdapter::popGLStates() {
    _renderTarget.popGLStates();
}

void SfmlRenderTargetAdapter::resetGLStates() {
    _renderTarget.resetGLStates();
}

bool SfmlRenderTargetAdapter::isSrgb() const {
    return _renderTarget.isSrgb();
}

} // namespace gr
HOBGOBLIN_NAMESPACE_END

#include <Hobgoblin/Private/Pmacro_undef.hpp>
