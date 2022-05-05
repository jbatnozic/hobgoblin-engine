#ifndef UHOBGOBLIN_GR_VIEWS_HPP
#define UHOBGOBLIN_GR_VIEWS_HPP

#include <Hobgoblin/Common.hpp>
#include <Hobgoblin/Graphics/Canvas.hpp>
#include <SFML/Graphics.hpp>

#include <cassert>
#include <variant>
#include <vector>

#include <Hobgoblin/Private/Pmacro_define.hpp>

HOBGOBLIN_NAMESPACE_BEGIN
namespace gr {

class View : public sf::View {
public:
    using sf::View::View;

    View(const sf::View& sfmlView);

    bool isEnabled() const noexcept;

    void setEnabled(bool enabled) noexcept;
 
private:
    bool _enabled = true;
};

using Views = std::vector<View>;

//! Adapts a sf::RenderTarget to the Canvas interface while also
//! providing support for using multiple Views at the same time.
class MultiViewRenderTargetAdapter : public Canvas {
public:
    MultiViewRenderTargetAdapter(sf::RenderTarget& renderTarget);

    void setViewCount(PZInteger viewCount);

    PZInteger getViewCount() const noexcept;

    View& getView(PZInteger viewIndex = 0);

    const View& getView(PZInteger viewIndex = 0) const;

    sf::RenderTarget& getUnderlyingRenderTarget() const;

    void draw(const Drawable& aDrawable,
              const sf::RenderStates& aStates = sf::RenderStates::Default) override;

    void draw(const sf::Drawable& drawable,
              const sf::RenderStates& states = sf::RenderStates::Default) override;

    void draw(const sf::Vertex* vertices, std::size_t vertexCount, sf::PrimitiveType type,
              const sf::RenderStates& states = sf::RenderStates::Default) override;

    void draw(const sf::VertexBuffer& vertexBuffer,
              const sf::RenderStates& states = sf::RenderStates::Default) override;

    void draw(const sf::VertexBuffer& vertexBuffer, 
              std::size_t firstVertex, 
              std::size_t vertexCount,
              const sf::RenderStates& states = sf::RenderStates::Default) override;

    void flush() override;

private:
    sf::RenderTarget& _renderTarget;
    std::variant<View, Views> _views;
    PZInteger _viewCount;

    View* addressOfFirstView();

    const View* addressOfFirstView() const;
};

} // namespace gr
HOBGOBLIN_NAMESPACE_END

#include <Hobgoblin/Private/Pmacro_undef.hpp>
#include <Hobgoblin/Private/Short_namespace.hpp>

#endif // !UHOBGOBLIN_GR_VIEWS_HPP

