// Copyright 2024 Jovan Batnozic. Released under MS-PL licence in Serbia.
// See https://github.com/jbatnozic/Hobgoblin?tab=readme-ov-file#licence

// clang-format off

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

//! Adapts a sf::RenderTarget to the hg::gr::RenderTarget interface.
//! NO multiview support!
class SfmlRenderTargetAdapter : public RenderTarget {
public:
    SfmlRenderTargetAdapter(sf::RenderTarget& aRenderTarget);

    ///////////////////////////////////////////////////////////////////////////
    // CANVAS - BASIC                                                        //
    ///////////////////////////////////////////////////////////////////////////

    math::Vector2pz getSize() const override final;

    bool isSrgb() const override final;

    RenderingBackendRef getRenderingBackend() override final;

    ///////////////////////////////////////////////////////////////////////////
    // CANVAS - DRAWING                                                      //
    ///////////////////////////////////////////////////////////////////////////

    void clear(const Color& aColor = COLOR_BLACK) override;

    void draw(const Drawable& aDrawable, const RenderStates& aStates = RenderStates::DEFAULT) override;

    void draw(const Vertex*       aVertices,
              PZInteger           aVertexCount,
              PrimitiveType       aPrimitiveType,
              const RenderStates& aStates = RenderStates::DEFAULT) override;

    void draw(const VertexBuffer& aVertexBuffer,
              const RenderStates& aStates = RenderStates::DEFAULT) override;

    void draw(const VertexBuffer& aVertexBuffer,
              PZInteger           aFirstVertex,
              PZInteger           aVertexCount,
              const RenderStates& aStates = RenderStates::DEFAULT) override;

    void flush() override;

    ///////////////////////////////////////////////////////////////////////////
    // CANVAS - OPEN GL                                                      //
    ///////////////////////////////////////////////////////////////////////////

    [[nodiscard]] bool setActive(bool aActive = true) override final;

    void pushGLStates() override final;

    void popGLStates() override final;

    void resetGLStates() override final;

    ///////////////////////////////////////////////////////////////////////////
    // VIEW CONTROLLER                                                       //
    ///////////////////////////////////////////////////////////////////////////

    void setViewCount(PZInteger aViewCount) override;

    void setView(const View& aView) override;

    void setView(PZInteger aViewIdx, const View& aView) override;

    PZInteger getViewCount() const override;

    const View& getView(PZInteger aViewIdx = 0) const override;

    View& getView(PZInteger aViewIdx = 0) override;

    View getDefaultView() const override final;

    math::Rectangle<PZInteger> getViewport(const View& aView) const override;

    math::Rectangle<PZInteger> getViewport(PZInteger aViewIdx = 0) const override;

    math::Vector2f mapPixelToCoords(const math::Vector2i& aPoint, const View& aView) const override;

    math::Vector2f mapPixelToCoords(const math::Vector2i& aPoint, PZInteger aViewIdx = 0) const override;

    math::Vector2i mapCoordsToPixel(const math::Vector2f& point, const View& view) const override;

    math::Vector2i mapCoordsToPixel(const math::Vector2f& point, PZInteger aViewIdx = 0) const override;

    ///////////////////////////////////////////////////////////////////////////
    // OTHER                                                                 //
    ///////////////////////////////////////////////////////////////////////////

    sf::RenderTarget& getSFMLRenderTarget() {
        return *_renderTarget;
    }

    void setSFMLRenderTarget(sf::RenderTarget& aRenderTarget) {
        _renderTarget = &aRenderTarget;
    }

private:
    NeverNull<sf::RenderTarget*> _renderTarget;
};

} // namespace gr
HOBGOBLIN_NAMESPACE_END

#include <Hobgoblin/Private/Pmacro_undef.hpp>
#include <Hobgoblin/Private/Short_namespace.hpp>

#endif // !UHOBGOBLIN_GRAPHICS_SFML_RENDERTARGET_ADAPTER_HPP

// clang-format on
