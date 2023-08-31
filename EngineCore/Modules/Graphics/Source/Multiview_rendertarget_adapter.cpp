
#include "Multiview_rendertarget_adapter.hpp"

#include <cassert>

#include "SFML_conversions.hpp"

#include <Hobgoblin/Private/Pmacro_define.hpp>

HOBGOBLIN_NAMESPACE_BEGIN
namespace gr {

MultiViewRenderTargetAdapter::MultiViewRenderTargetAdapter(sf::RenderTarget& aRenderTarget)
    : _renderTarget{aRenderTarget}
    , _views{ToHg(_renderTarget.getDefaultView())}
    , _viewCount{1}
{
}

void MultiViewRenderTargetAdapter::setViewCount(PZInteger aViewCount) {
    if (aViewCount < 1) {
        aViewCount = 1;
    }

    if (aViewCount > 1) {
        if (_viewCount > 1) {
            std::get<Views>(_views).resize(pztos(aViewCount));
        }
        else {
            View currentView = *addressOfFirstView();
            _views = Views{};
            std::get<Views>(_views).resize(pztos(aViewCount));
            std::get<Views>(_views)[0] = currentView;
        }
    }
    else {
        _views = *addressOfFirstView();
    }

    _viewCount = aViewCount;
}

PZInteger MultiViewRenderTargetAdapter::getViewCount() const noexcept {
    return _viewCount;
}

View& MultiViewRenderTargetAdapter::getView(PZInteger aViewIdx) {
    assert(aViewIdx < _viewCount);
    return *(addressOfFirstView() + aViewIdx);
}

const View& MultiViewRenderTargetAdapter::getView(PZInteger aViewIdx) const {
    assert(aViewIdx < _viewCount);
    return *(addressOfFirstView() + aViewIdx);
}

void MultiViewRenderTargetAdapter::draw(const Drawable& aDrawable,
                                        const RenderStates& aStates) {
    for (PZInteger i = 0; i < _viewCount; i += 1) {
        const auto& view = getView(i);
        if (view.isEnabled()) {
            _renderTarget.setView(ToSf(view));
            // _renderTarget.draw(ToSf(aDrawable), ToSf(aStates)); TODO
            aDrawable._draw(SELF, aStates);
        }
    }
}

void MultiViewRenderTargetAdapter::draw(const Vertex* aVertices,
                                        PZInteger aVertexCount,
                                        PrimitiveType aPrimitiveType,
                                        const RenderStates& aStates) {
    for (PZInteger i = 0; i < _viewCount; i += 1) {
        const auto& view = getView(i);
        if (view.isEnabled()) {
            _renderTarget.setView(ToSf(view));
            _renderTarget.draw(
                reinterpret_cast<const sf::Vertex*>(aVertices),
                pztos(aVertexCount),
                ToSf(aPrimitiveType),
                ToSf(aStates)
            );
        }
    }
}

void MultiViewRenderTargetAdapter::draw(const VertexBuffer& aVertexBuffer,
                                        const RenderStates& aStates) {
    for (PZInteger i = 0; i < _viewCount; i += 1) {
        const auto& view = getView(i);
        if (view.isEnabled()) {
            _renderTarget.setView(ToSf(view));
            // _renderTarget.draw(ToSf(aVertexBuffer), ToSf(aStates)); TODO
        }
    }
}

void MultiViewRenderTargetAdapter::draw(const VertexBuffer& aVertexBuffer,
                                        PZInteger aFirstVertex,
                                        PZInteger aVertexCount,
                                        const RenderStates& aStates) {
    for (PZInteger i = 0; i < _viewCount; i += 1) {
        const auto& view = getView(i);
        if (view.isEnabled()) {
            _renderTarget.setView(ToSf(view));
            // _renderTarget.draw(ToSf(aVertexBuffer), pztos(aFirstVertex), pztos(aVertexCount), ToSf(aStates)); TODO
        }
    }
}

void MultiViewRenderTargetAdapter::flush() { /* Do nothing; there is no batching in SFML. */ }

View* MultiViewRenderTargetAdapter::addressOfFirstView() {
    if (_viewCount > 1) {
        return std::get<Views>(_views).data();
    }
    else {
        return std::addressof(std::get<View>(_views));
    }
}

const View* MultiViewRenderTargetAdapter::addressOfFirstView() const {
    if (_viewCount > 1) {
        return std::get<Views>(_views).data();
    }
    else {
        return std::addressof(std::get<View>(_views));
    }
}

} // namespace gr
HOBGOBLIN_NAMESPACE_END

#include <Hobgoblin/Private/Pmacro_undef.hpp>

