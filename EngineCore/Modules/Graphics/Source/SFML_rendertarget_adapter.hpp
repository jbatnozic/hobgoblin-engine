#ifndef UHOBGOBLIN_GRAPHICS_SFML_RENDERTARGET_ADAPTER_HPP
#define UHOBGOBLIN_GRAPHICS_SFML_RENDERTARGET_ADAPTER_HPP

#include <Hobgoblin/Common.hpp>
#include <Hobgoblin/Graphics/Render_target.hpp>
#include <Hobgoblin/Graphics/View.hpp>

#include <SFML/Graphics/RenderTarget.hpp>

#include <variant>
#include <vector>

#include <Hobgoblin/Private/Pmacro_define.hpp>

HOBGOBLIN_NAMESPACE_BEGIN
namespace gr {

//! Adapts a sf::RenderTarget to the Canvas interface.
//! NO multiview support!
class SfmlRenderTargetAdapter final : public RenderTarget {
public:
    SfmlRenderTargetAdapter(sf::RenderTarget& aRenderTarget);

    ///////////////////////////////////////////////////////////////////////////
    // RENDERTARGET                                                          //
    ///////////////////////////////////////////////////////////////////////////

    void clear(const Color& aColor = COLOR_BLACK) override;

    void setViewCount(PZInteger aViewCount) override;

    void setView(const View& aView) override;

    void setView(PZInteger aViewIdx, const View& aView) override;

    PZInteger getViewCount() const override;

    const View& getView(PZInteger aViewIdx = 0) const override;

    View& getView(PZInteger aViewIdx = 0) override;

    View getDefaultView() const override;

    math::Rectangle<PZInteger> getViewport(const View& aView) const override;

    math::Rectangle<PZInteger> getViewport(PZInteger aViewIdx) const override;

    math::Vector2f mapPixelToCoords(const math::Vector2i& aPoint, PZInteger aViewIdx = 0) const override;

    math::Vector2f mapPixelToCoords(const math::Vector2i& aPoint, const View& aView) const override;

    math::Vector2i mapCoordsToPixel(const math::Vector2f& point, const View& view) const override;

    math::Vector2i mapCoordsToPixel(const math::Vector2f& point, PZInteger aViewIdx = 0) const override;

    math::Vector2pz getSize() const override;

    bool setActive(bool aActive = true) override;

    void pushGLStates() override;

    void popGLStates() override;

    void resetGLStates() override;

    ///////////////////////////////////////////////////////////////////////////
    // CANVAS                                                                //
    ///////////////////////////////////////////////////////////////////////////

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

    ///////////////////////////////////////////////////////////////////////////
    // OTHER                                                                 //
    ///////////////////////////////////////////////////////////////////////////

    sf::RenderTarget& getSFMLRenderTarget() {
        return _renderTarget;
    }

private:
    sf::RenderTarget& _renderTarget;

    void getCanvasDetails(CanvasType& aType, void*& aRenderingBackend) override {
        aType = CanvasType::SFML;
        aRenderingBackend = &_renderTarget;
    }
};

} // namespace gr
HOBGOBLIN_NAMESPACE_END

#include <Hobgoblin/Private/Pmacro_undef.hpp>
#include <Hobgoblin/Private/Short_namespace.hpp>

#endif // !UHOBGOBLIN_GRAPHICS_SFML_RENDERTARGET_ADAPTER_HPP
