#ifndef UHOBGOBLIN_GR_VIEWS_HPP
#define UHOBGOBLIN_GR_VIEWS_HPP

#include <Hobgoblin/Graphics/Canvas.hpp>
#include <SFML/Graphics.hpp>

#include <cassert>
#include <variant>
#include <vector>

#include <Hobgoblin/Private/Pmacro_define.hpp>

HOBGOBLIN_NAMESPACE_START
namespace gr {

class View : public sf::View {
public:
    // TODO: Implementation to .cpp file

    using sf::View::View;

    View(const sf::View& sfmlView) {
        static_cast<sf::View&>(SELF) = sfmlView;
    }

    bool isEnabled() const noexcept {
        return _enabled;
    }

    void setEnabled(bool enabled) noexcept {
        _enabled = enabled;
    }
 
private:
    bool _enabled = true;
};

using Views = std::vector<View>;

class MultiViewRenderTargetAdapter : public Canvas {
public:
    // TODO: Implementation to .cpp file

    MultiViewRenderTargetAdapter(sf::RenderTarget& renderTarget)
        : _renderTarget{renderTarget}
        , _views{_renderTarget.getDefaultView()}
        , _viewCount{1}
    {
    }

    void setViewCount(PZInteger viewCount) {
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

    PZInteger getViewCount() const noexcept {
        return _viewCount;
    }

    View& getView(PZInteger viewIndex = 0) {
        assert(viewIndex < _viewCount);
        return *(addressOfFirstView() + viewIndex);
    }

    const View& getView(PZInteger viewIndex = 0) const {
        assert(viewIndex < _viewCount);
        return *(addressOfFirstView() + viewIndex);
    }

    sf::RenderTarget& getUnderlyingRenderTarget() const {
        return _renderTarget;
    }

    void draw(const sf::Drawable& drawable, 
              const sf::RenderStates& states = sf::RenderStates::Default) override {
        for (PZInteger i = 0; i < _viewCount; i += 1) {
            _renderTarget.setView(getView(i));
            _renderTarget.draw(drawable, states);
        }
    }

    void draw(const sf::Vertex* vertices, std::size_t vertexCount, sf::PrimitiveType type,
              const sf::RenderStates& states = sf::RenderStates::Default) override {
        for (PZInteger i = 0; i < _viewCount; i += 1) {
            _renderTarget.setView(getView(i));
            _renderTarget.draw(vertices, vertexCount, type, states);
        }
    }

    void draw(const sf::VertexBuffer& vertexBuffer, 
              const sf::RenderStates& states = sf::RenderStates::Default) override {
        for (PZInteger i = 0; i < _viewCount; i += 1) {
            _renderTarget.setView(getView(i));
            _renderTarget.draw(vertexBuffer, states);
        }
    }

    void draw(const sf::VertexBuffer& vertexBuffer, std::size_t firstVertex, std::size_t vertexCount,
              const sf::RenderStates& states = sf::RenderStates::Default) override {
        for (PZInteger i = 0; i < _viewCount; i += 1) {
            _renderTarget.setView(getView(i));
            _renderTarget.draw(vertexBuffer, firstVertex, vertexCount, states);
        }
    }

private:
    sf::RenderTarget& _renderTarget;
    std::variant<View, Views> _views;
    PZInteger _viewCount;

    View* addressOfFirstView() {
        if (_viewCount > 1) {
            return std::get<Views>(_views).data();
        }
        else {
            return std::addressof(std::get<View>(_views));
        }
    }

    const View* addressOfFirstView() const {
        if (_viewCount > 1) {
            return std::get<Views>(_views).data();
        }
        else {
            return std::addressof(std::get<View>(_views));
        }
    }
};

} // namespace gr
HOBGOBLIN_NAMESPACE_END

#include <Hobgoblin/Private/Pmacro_undef.hpp>
#include <Hobgoblin/Private/Short_namespace.hpp>

#endif // !UHOBGOBLIN_GR_VIEWS_HPP

