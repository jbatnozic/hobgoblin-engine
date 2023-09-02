#ifndef UHOBGOBLIN_GRAPHICS_MULTIVIEW_RENDERTARGET_ADAPTER_HPP
#define UHOBGOBLIN_GRAPHICS_MULTIVIEW_RENDERTARGET_ADAPTER_HPP

#include <Hobgoblin/Common.hpp>
#include <Hobgoblin/Graphics/Canvas.hpp>
#include <Hobgoblin/Graphics/View.hpp>

#include <SFML/Graphics/RenderTarget.hpp>

#include <variant>
#include <vector>

#include <Hobgoblin/Private/Pmacro_define.hpp>

HOBGOBLIN_NAMESPACE_BEGIN
namespace gr {

//! Adapts a sf::RenderTarget to the Canvas interface while also
//! providing support for using multiple Views at the same time.
class MultiViewRenderTargetAdapter final : public Canvas {
public:
    MultiViewRenderTargetAdapter(sf::RenderTarget& aRenderTarget);

    void setViewCount(PZInteger aViewCount);

    PZInteger getViewCount() const noexcept;

    View& getView(PZInteger aViewIdx);

    const View& getView(PZInteger aViewIdx) const;

    void draw(const Drawable& aDrawable,
              const RenderStates& aStates = RenderStates::DEFAULT) override;

    void draw(const Vertex* aVertices,
              PZInteger aVertexCount,
              PrimitiveType aPrimitiveType,
              const RenderStates& aStates = RenderStates::DEFAULT) override;

    void draw(const VertexBuffer& aVertexBuffer,
              const RenderStates& aStates = RenderStates::DEFAULT) override;

    void draw(const VertexBuffer& aVertexBuffer,
              PZInteger aFirstVertex,
              PZInteger aVertexCount,
              const RenderStates& aStates = RenderStates::DEFAULT) override;

    void flush() override;

    sf::RenderTarget& getSFMLRenderTarget() {
        return _renderTarget;
    }

private:
    using Views = std::vector<View>;

    sf::RenderTarget& _renderTarget;
    std::variant<View, Views> _views;
    PZInteger _viewCount;

    View* addressOfFirstView();

    const View* addressOfFirstView() const;

    void getCanvasDetails(CanvasType& aType, void*& aRenderingBackend) override {
        aType = CanvasType::SFML;
        aRenderingBackend = &_renderTarget;
    }
};

} // namespace gr
HOBGOBLIN_NAMESPACE_END

#include <Hobgoblin/Private/Pmacro_undef.hpp>
#include <Hobgoblin/Private/Short_namespace.hpp>

#endif // !UHOBGOBLIN_GRAPHICS_MULTIVIEW_RENDERTARGET_ADAPTER_HPP
