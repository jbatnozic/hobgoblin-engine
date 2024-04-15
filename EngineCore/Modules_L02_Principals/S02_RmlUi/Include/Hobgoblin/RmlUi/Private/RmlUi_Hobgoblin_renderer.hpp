// Copyright 2024 Jovan Batnozic. Released under MS-PL licence in Serbia.
// See https://github.com/jbatnozic/Hobgoblin?tab=readme-ov-file#licence

// clang-format off

#ifndef UHOBGOBLIN_RMLUI_PRIVATE_RMLUI_HOBGOBLIN_RENDERER_HPP
#define UHOBGOBLIN_RMLUI_PRIVATE_RMLUI_HOBGOBLIN_RENDERER_HPP

#include <RmlUi/Core/RenderInterface.h>

#include <Hobgoblin/Graphics.hpp>
#include <Hobgoblin/RmlUi/Context_driver.hpp>

#include <Hobgoblin/Private/Pmacro_define.hpp>
HOBGOBLIN_NAMESPACE_BEGIN
namespace rml {
namespace detail {

class RmlUiHobgoblinRenderer : public Rml::RenderInterface {
public:
    RmlUiHobgoblinRenderer() = default;

    //! Sets the render target
    void setRenderTarget(gr::RenderTarget* aRenderTarget);

    //! Returns the currently render target
    gr::RenderTarget* getRenderTarget() const;

    ///////////////////////////////////////////////////////////////////////////
    // INHERITED FROM RML::RENDERINTERFACE                                   //
    ///////////////////////////////////////////////////////////////////////////

    //! Called by RmlUi when it wants to render geometry that it does not wish to optimise.
    void RenderGeometry(Rml::Vertex* aVertices,
                        int aVerticesCount,
                        int* aIndices,
                        int aIndicesCount,
                        Rml::TextureHandle aTexture,
                        const Rml::Vector2f& aTranslation) override;

    //! Called by RmlUi when it wants to compile geometry it believes will be static for the forseeable future.
    Rml::CompiledGeometryHandle CompileGeometry(Rml::Vertex*, int, int*, int, Rml::TextureHandle) override;

    //! Called by RmlUi when it wants to render application-compiled geometry.
    void RenderCompiledGeometry(Rml::CompiledGeometryHandle geometry, const Rml::Vector2f& translation) override;

    //! Called by RmlUi when it wants to release application-compiled geometry.
    void ReleaseCompiledGeometry(Rml::CompiledGeometryHandle geometry) override;

    //! Called by RmlUi when it wants to enable or disable scissoring to clip content.
    void EnableScissorRegion(bool aEnable) override;

    //! Called by RmlUi when it wants to change the scissor region.
    void SetScissorRegion(int x, int y, int width, int height) override;

    //! Called by RmlUi when a texture is required by the library.
    bool LoadTexture(Rml::TextureHandle& texture_handle, Rml::Vector2i& texture_dimensions, const Rml::String& source) override;

    //! Called by RmlUi when a texture is required to be built from an internally-generated sequence of pixels.
    bool GenerateTexture(Rml::TextureHandle& texture_handle, const Rml::byte* source, const Rml::Vector2i& source_dimensions) override;

    //! Called by RmlUi when a loaded texture is no longer required.
    void ReleaseTexture(Rml::TextureHandle texture_handle) override;

private:
    void _initViewport();

    gr::RenderTarget* _renderTarget = nullptr;
};

} // namespace detail
} // namespace rml
HOBGOBLIN_NAMESPACE_END
#include <Hobgoblin/Private/Pmacro_undef.hpp>
#include <Hobgoblin/Private/Short_namespace.hpp>

#endif // !UHOBGOBLIN_RMLUI_PRIVATE_RMLUI_HOBGOBLIN_RENDERER_HPP

// clang-format on
