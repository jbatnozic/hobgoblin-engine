// Copyright 2024 Jovan Batnozic. Released under MS-PL licence in Serbia.
// See https://github.com/jbatnozic/Hobgoblin?tab=readme-ov-file#licence

// clang-format off

#ifndef UHOBGOBLIN_GRAPHICS_MULTIVIEW_RENDERTARGET_ADAPTER_HPP
#define UHOBGOBLIN_GRAPHICS_MULTIVIEW_RENDERTARGET_ADAPTER_HPP

#include <Hobgoblin/Common.hpp>
#include <Hobgoblin/Graphics/Canvas.hpp>
#include <Hobgoblin/Graphics/Render_target.hpp>
#include <Hobgoblin/Graphics/View.hpp>

#include <variant>
#include <vector>

#include <Hobgoblin/Private/Pmacro_define.hpp>

HOBGOBLIN_NAMESPACE_BEGIN
namespace gr {

//! TODO(add description)
class MultiViewRenderTargetAdapter final : public Canvas {
public:
    MultiViewRenderTargetAdapter(RenderTarget& aRenderTarget);

    MultiViewRenderTargetAdapter(MultiViewRenderTargetAdapter&& aOther) = default;

    void setRenderTarget(RenderTarget& aRenderTarget);

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

    void getCanvasDetails(CanvasType& aType, void*& aRenderingBackend) override {
        _renderTarget->getCanvasDetails(aType, aRenderingBackend);
    }

private:
    using Views = std::vector<View>;

    RenderTarget* _renderTarget;
    std::variant<View, Views> _views;
    PZInteger _viewCount;

    View* _addressOfFirstView();

    const View* _addressOfFirstView() const;
};

} // namespace gr
HOBGOBLIN_NAMESPACE_END

#include <Hobgoblin/Private/Pmacro_undef.hpp>
#include <Hobgoblin/Private/Short_namespace.hpp>

#endif // !UHOBGOBLIN_GRAPHICS_MULTIVIEW_RENDERTARGET_ADAPTER_HPP

// clang-format on
