// Copyright 2024 Jovan Batnozic. Released under MS-PL licence in Serbia.
// See https://github.com/jbatnozic/Hobgoblin?tab=readme-ov-file#licence

// clang-format off


#include "Multiview_rendertarget_adapter.hpp"

#include <cassert>

#include "SFML_conversions.hpp"

#include <Hobgoblin/Private/Pmacro_define.hpp>

HOBGOBLIN_NAMESPACE_BEGIN
namespace gr {

MultiViewRenderTargetAdapter::MultiViewRenderTargetAdapter(RenderTarget& aRenderTarget)
    : _renderTarget{&aRenderTarget}
    , _views{_renderTarget->getDefaultView()}
    , _viewCount{1}
{
}

void MultiViewRenderTargetAdapter::setRenderTarget(RenderTarget& aRenderTarget) {
    _renderTarget = &aRenderTarget;
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
            View currentView = *_addressOfFirstView();
            _views = Views{};
            std::get<Views>(_views).resize(pztos(aViewCount));
            std::get<Views>(_views)[0] = currentView;
        }
    }
    else {
        _views = *_addressOfFirstView();
    }

    _viewCount = aViewCount;
}

PZInteger MultiViewRenderTargetAdapter::getViewCount() const noexcept {
    return _viewCount;
}

View& MultiViewRenderTargetAdapter::getView(PZInteger aViewIdx) {
    assert(aViewIdx < _viewCount);
    return *(_addressOfFirstView() + aViewIdx);
}

const View& MultiViewRenderTargetAdapter::getView(PZInteger aViewIdx) const {
    assert(aViewIdx < _viewCount);
    return *(_addressOfFirstView() + aViewIdx);
}

void MultiViewRenderTargetAdapter::draw(const Drawable& aDrawable,
                                        const RenderStates& aStates) {
    for (PZInteger i = 0; i < _viewCount; i += 1) {
        const auto& view = getView(i);
        if (view.isEnabled()) {
            _renderTarget->setView(view);
            _renderTarget->draw(aDrawable, aStates);
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
            _renderTarget->setView(view);
            _renderTarget->draw(
                aVertices,
                aVertexCount,
                aPrimitiveType,
                aStates
            );
        }
    }
}

void MultiViewRenderTargetAdapter::draw(const VertexBuffer& aVertexBuffer,
                                        const RenderStates& aStates) {
    for (PZInteger i = 0; i < _viewCount; i += 1) {
        const auto& view = getView(i);
        if (view.isEnabled()) {
            _renderTarget->setView(view);
            _renderTarget->draw(aVertexBuffer, aStates);
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
            _renderTarget->setView(view);
            _renderTarget->draw(aVertexBuffer, aFirstVertex, aVertexCount, aStates);
        }
    }
}

void MultiViewRenderTargetAdapter::flush() {
    _renderTarget->flush();
}

View* MultiViewRenderTargetAdapter::_addressOfFirstView() {
    if (_viewCount > 1) {
        return std::get<Views>(_views).data();
    }
    else {
        return std::addressof(std::get<View>(_views));
    }
}

const View* MultiViewRenderTargetAdapter::_addressOfFirstView() const {
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

// clang-format on
