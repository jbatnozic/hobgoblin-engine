
#include <Hobgoblin/Graphics/Render_target.hpp>

#include <SFML/Graphics/RenderTarget.hpp>

#include <new>

#include "Multiview_rendertarget_adapter.hpp"
#include "SFML_conversions.hpp"

#include <Hobgoblin/Private/Pmacro_define.hpp>

HOBGOBLIN_NAMESPACE_BEGIN
namespace gr {

namespace {
class SfRenderTargetAdapter : public sf::RenderTarget {
public:
    SfRenderTargetAdapter(detail::RenderTargetPolymorphismAdapter& aRenderTargetPolyAdapter)
        : _renderTargetPolyAdapter{aRenderTargetPolyAdapter}
    {
    }

    sf::Vector2u getSize() const override {
        const auto size = _renderTargetPolyAdapter.getSize();
        return {
            static_cast<unsigned>(size.x),
            static_cast<unsigned>(size.y)
        };
    }

    void initialize() {
        RenderTarget::initialize();
    }

private:
    detail::RenderTargetPolymorphismAdapter& _renderTargetPolyAdapter;
};
} // namespace

using ImplType = SfRenderTargetAdapter;
constexpr auto IMPL_SIZE  = sizeof(ImplType);
constexpr auto IMPL_ALIGN = alignof(ImplType);
#define  IMPLOF(_obj_) (reinterpret_cast<ImplType*>(&((_obj_)._storage)))
#define CIMPLOF(_obj_) (reinterpret_cast<const ImplType*>(&((_obj_)._storage)))
#define  SELF_IMPL (IMPLOF(SELF))
#define SELF_CIMPL (CIMPLOF(SELF))

#define GetMVA(_cv_) (reinterpret_cast<_cv_ MultiViewRenderTargetAdapter*>(&_mvaStorage))

RenderTarget::RenderTarget() {
    static_assert(STORAGE_SIZE == IMPL_SIZE,   "RenderTarget::STORAGE_SIZE is inadequate.");
    static_assert(STORAGE_ALIGN == IMPL_ALIGN, "RenderTarget::STORAGE_ALIGN is inadequate.");

    static_assert(MVA_STORAGE_SIZE == sizeof(MultiViewRenderTargetAdapter), "RenderTarget::MVA_STORAGE_SIZE is inadequate.");
    static_assert(MVA_STORAGE_ALIGN == alignof(MultiViewRenderTargetAdapter), "RenderTarget::MVA_STORAGE_ALIGN is inadequate.");

    new (&_storage)    ImplType(SELF);
    new (&_mvaStorage) MultiViewRenderTargetAdapter(*SELF_IMPL);
}

RenderTarget::~RenderTarget() {
    GetMVA()->~MultiViewRenderTargetAdapter();
    SELF_IMPL->~ImplType();
}

void RenderTarget::clear(const Color& aColor) {
    SELF_IMPL->clear(ToSf(aColor));
}

void RenderTarget::setViewCount(PZInteger aViewCount) {
    GetMVA()->setViewCount(aViewCount);
}

void RenderTarget::setView(const View& aView, PZInteger aViewIdx) {
    getView(aViewIdx) = aView; // TODO is this correct?
}

PZInteger RenderTarget::getViewCount() const {
    return GetMVA(const)->getViewCount();
}

const View& RenderTarget::getView(PZInteger aViewIdx) const {
    return GetMVA(const)->getView(aViewIdx);
}

View& RenderTarget::getView(PZInteger aViewIdx) {
    return GetMVA()->getView(aViewIdx);
}

View RenderTarget::getDefaultView() const {
    return ToHg(SELF_CIMPL->getDefaultView());
}

math::Rectangle<PZInteger> RenderTarget::getViewport(const View& aView) const {
    const auto viewport = SELF_CIMPL->getViewport(ToSf(aView));
    return {
        static_cast<PZInteger>(viewport.left),
        static_cast<PZInteger>(viewport.top),
        static_cast<PZInteger>(viewport.width),
        static_cast<PZInteger>(viewport.height),
    };
}

math::Rectangle<PZInteger> RenderTarget::getViewport(PZInteger aViewIdx) const {
    return getViewport(getView(aViewIdx));
}

math::Vector2f RenderTarget::mapPixelToCoords(const math::Vector2i& aPoint) const {
    const auto pos = SELF_CIMPL->mapPixelToCoords(ToSf(aPoint));
    return ToHg(pos);
}

math::Vector2f RenderTarget::mapPixelToCoords(const math::Vector2i& aPoint, const View& aView) const {
    const auto pos = SELF_CIMPL->mapPixelToCoords(ToSf(aPoint), ToSf(aView));
    return ToHg(pos);
}

math::Vector2f RenderTarget::mapPixelToCoords(const math::Vector2i& aPoint, PZInteger aViewIdx) const {
    return mapPixelToCoords(aPoint, getView(aViewIdx));
}

math::Vector2i RenderTarget::mapCoordsToPixel(const math::Vector2f& aPoint) const {
    const auto pos = SELF_CIMPL->mapCoordsToPixel(ToSf(aPoint));
    return ToHg(pos);
}

math::Vector2i RenderTarget::mapCoordsToPixel(const math::Vector2f& aPoint, const View& aView) const {
    const auto pos = SELF_CIMPL->mapCoordsToPixel(ToSf(aPoint), ToSf(aView));
    return ToHg(pos);
}

math::Vector2i RenderTarget::mapCoordsToPixel(const math::Vector2f& aPoint, PZInteger aViewIdx) const {
    return mapCoordsToPixel(aPoint, getView(aViewIdx));
}

void RenderTarget::draw(const Drawable& aDrawable,
                        const RenderStates& aStates) {
    GetMVA()->draw(aDrawable, aStates);
}

void RenderTarget::draw(const Vertex* aVertices,
                        PZInteger aVertexCount,
                        PrimitiveType aPrimitiveType,
                        const RenderStates& aStates) {
    GetMVA()->draw(aVertices, aVertexCount, aPrimitiveType, aStates);
}

void RenderTarget::draw(const VertexBuffer& aVertexBuffer,
                        const RenderStates& aStates) {
    GetMVA()->draw(aVertexBuffer, aStates);
}

void RenderTarget::draw(const VertexBuffer& aVertexBuffer,
                        PZInteger aFirstVertex,
                        PZInteger aVertexCount,
                        const RenderStates& aStates) {
    GetMVA()->draw(aVertexBuffer, aFirstVertex, aVertexCount, aStates);
}

void RenderTarget::flush() {
    GetMVA()->flush();
}

math::Vector2pz RenderTarget::getSize() const {
    const auto size = SELF_CIMPL->getSize();
    return {
        static_cast<PZInteger>(size.x),
        static_cast<PZInteger>(size.y)
    };
}

bool RenderTarget::setActive(bool aActive) {
    return SELF_IMPL->setActive(aActive);
}

void RenderTarget::pushGLStates() {
    SELF_IMPL->pushGLStates();
}

void RenderTarget::popGLStates() {
    SELF_IMPL->popGLStates();
}

void RenderTarget::resetGLStates() {
    SELF_IMPL->resetGLStates();
}

void RenderTarget::initialize() {
    SELF_IMPL->initialize();
}

} // namespace gr
HOBGOBLIN_NAMESPACE_END

#include <Hobgoblin/Private/Pmacro_undef.hpp>
