
#include <Hobgoblin/Graphics/Views.hpp>

#include <Hobgoblin/Private/Pmacro_define.hpp>

HOBGOBLIN_NAMESPACE_BEGIN
namespace gr {

///////////////////////////////////////////////////////////////////////////
// VIEW                                                                  //
///////////////////////////////////////////////////////////////////////////

View::View(const sf::View& sfmlView) {
    static_cast<sf::View&>(SELF) = sfmlView;
}

bool View::isEnabled() const noexcept {
    return _enabled;
}

void View::setEnabled(bool enabled) noexcept {
    _enabled = enabled;
}

///////////////////////////////////////////////////////////////////////////
// MULTI VIEW RNEDER TARGET ADAPTER                                      //
///////////////////////////////////////////////////////////////////////////

MultiViewRenderTargetAdapter::MultiViewRenderTargetAdapter(sf::RenderTarget& renderTarget)
    : _renderTarget{renderTarget}
    , _views{_renderTarget.getDefaultView()}
    , _viewCount{1}
{
}

void MultiViewRenderTargetAdapter::setViewCount(PZInteger viewCount) {
    if (viewCount < 1) {
        viewCount = 1;
    }

    if (viewCount > 1) {
        if (_viewCount > 1) {
            std::get<Views>(_views).resize(pztos(viewCount));
        }
        else {
            View currentView = *addressOfFirstView();
            _views = Views{};
            std::get<Views>(_views).resize(pztos(viewCount));
            std::get<Views>(_views)[0] = currentView;
        }
    }
    else {
        _views = *addressOfFirstView();
    }

    _viewCount = viewCount;
}

PZInteger MultiViewRenderTargetAdapter::getViewCount() const noexcept {
    return _viewCount;
}

View& MultiViewRenderTargetAdapter::getView(PZInteger viewIndex) {
    assert(viewIndex < _viewCount);
    return *(addressOfFirstView() + viewIndex);
}

const View& MultiViewRenderTargetAdapter::getView(PZInteger viewIndex) const {
    assert(viewIndex < _viewCount);
    return *(addressOfFirstView() + viewIndex);
}

sf::RenderTarget& MultiViewRenderTargetAdapter::getUnderlyingRenderTarget() const {
    return _renderTarget;
}

void MultiViewRenderTargetAdapter::draw(const sf::Drawable& drawable,
                                        const sf::RenderStates& states) {
    for (PZInteger i = 0; i < _viewCount; i += 1) {
        _renderTarget.setView(getView(i));
        _renderTarget.draw(drawable, states);
    }
}

void MultiViewRenderTargetAdapter::draw(const Drawable& aDrawable,
                                        const sf::RenderStates& aStates) {
    detail::Drawable_DrawOntoCanvas(aDrawable, SELF, aStates);
}

void MultiViewRenderTargetAdapter::draw(const sf::Vertex* vertices, 
                                        std::size_t vertexCount, 
                                        sf::PrimitiveType type,
                                        const sf::RenderStates& states) {
    for (PZInteger i = 0; i < _viewCount; i += 1) {
        _renderTarget.setView(getView(i));
        _renderTarget.draw(vertices, vertexCount, type, states);
    }
}

void MultiViewRenderTargetAdapter::draw(const sf::VertexBuffer& vertexBuffer,
                                        const sf::RenderStates& states) {
    for (PZInteger i = 0; i < _viewCount; i += 1) {
        _renderTarget.setView(getView(i));
        _renderTarget.draw(vertexBuffer, states);
    }
}

void MultiViewRenderTargetAdapter::draw(const sf::VertexBuffer& vertexBuffer,
                                        std::size_t firstVertex,
                                        std::size_t vertexCount,
                                        const sf::RenderStates& states) {
    for (PZInteger i = 0; i < _viewCount; i += 1) {
        _renderTarget.setView(getView(i));
        _renderTarget.draw(vertexBuffer, firstVertex, vertexCount, states);
    }
}

void MultiViewRenderTargetAdapter::flush() {
}

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

