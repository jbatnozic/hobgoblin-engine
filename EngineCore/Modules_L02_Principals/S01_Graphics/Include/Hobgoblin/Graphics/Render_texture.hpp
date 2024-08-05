// Code in this file is adapted from SFML code and retains its original
// open source licence (provided below).
// See https://github.com/SFML/SFML

////////////////////////////////////////////////////////////
//
// SFML - Simple and Fast Multimedia Library
// Copyright (C) 2007-2023 Laurent Gomila (laurent@sfml-dev.org)
//
// This software is provided 'as-is', without any express or implied warranty.
// In no event will the authors be held liable for any damages arising from the use of this software.
//
// Permission is granted to anyone to use this software for any purpose,
// including commercial applications, and to alter it and redistribute it freely,
// subject to the following restrictions:
//
// 1. The origin of this software must not be misrepresented;
//    you must not claim that you wrote the original software.
//    If you use this software in a product, an acknowledgment
//    in the product documentation would be appreciated but is not required.
//
// 2. Altered source versions must be plainly marked as such,
//    and must not be misrepresented as being the original software.
//
// 3. This notice may not be removed or altered from any source distribution.
//
////////////////////////////////////////////////////////////

#ifndef UHOBGOBLIN_GRAPHICS_RENDER_TEXTURE_HPP
#define UHOBGOBLIN_GRAPHICS_RENDER_TEXTURE_HPP

#include <Hobgoblin/Graphics/Canvas.hpp>
#include <Hobgoblin/Graphics/Texture.hpp>
#include <Hobgoblin/Graphics/View_controller.hpp>
#include <Hobgoblin/Math/Vector.hpp>
#include <Hobgoblin/Window/Context_settings.hpp>

#include <memory>

#include <Hobgoblin/Private/Pmacro_define.hpp>

HOBGOBLIN_NAMESPACE_BEGIN
namespace gr {

namespace detail {
class GraphicsImplAccessor;
} // namespace detail

//! \brief Target for off-screen 2D rendering into a texture
class RenderTexture
    : public Canvas
    , public ViewController {
public:
    //! \brief Default constructor
    //!
    //! Constructs an empty, invalid render-texture. You must
    //! call create to have a valid render-texture.
    //!
    //! \see create
    RenderTexture();

    //! \brief Destructor
    ~RenderTexture() override;

    //! \brief Deleted copy constructor
    RenderTexture(const RenderTexture&) = delete;

    //! \brief Deleted copy assignment
    RenderTexture& operator=(const RenderTexture&) = delete;

    //! \brief Move constructor
    RenderTexture(RenderTexture&& aOther) noexcept = delete;

    //! \brief Move assignment operator
    RenderTexture& operator=(RenderTexture&& aOther) noexcept = delete;

    //! \brief Create the render-texture
    //!
    //! Before calling this function, the render-texture is in
    //! an invalid state, thus it is mandatory to call it before
    //! doing anything with the render-texture.
    //! The last parameter, \a settings, is useful if you want to enable
    //! multi-sampling or use the render-texture for OpenGL rendering that
    //! requires a depth or stencil buffer. Otherwise it is unnecessary, and
    //! you should leave this parameter at its default value.
    //!
    //! \param size     Width and height of the render-texture
    //! \param settings Additional settings for the underlying OpenGL texture and context
    //!
    //! \return True if creation has been successful
    void create(const math::Vector2pz&      aSize,
                const win::ContextSettings& aSettings = win::ContextSettings{});

    //! \brief Get the maximum anti-aliasing level supported by the system
    //!
    //! \return The maximum anti-aliasing level supported by the system
    static PZInteger getMaximumAntialiasingLevel();

    //! \brief Enable or disable texture smoothing
    //!
    //! This function is similar to Texture::setSmooth.
    //! This parameter is disabled by default.
    //!
    //! \param smooth True to enable smoothing, false to disable it
    //!
    //! \see isSmooth
    void setSmooth(bool aSmooth);

    //! \brief Tell whether the smooth filtering is enabled or not
    //!
    //! \return True if texture smoothing is enabled
    //!
    //! \see setSmooth
    bool isSmooth() const;

    //! \brief Enable or disable texture repeating
    //!
    //! This function is similar to Texture::setRepeated.
    //! This parameter is disabled by default.
    //!
    //! \param repeated True to enable repeating, false to disable it
    //!
    //! \see isRepeated
    void setRepeated(bool aRepeated);

    //! \brief Tell whether the texture is repeated or not
    //!
    //! \return True if texture is repeated
    //!
    //! \see setRepeated
    bool isRepeated() const;

    //! \brief Generate a mipmap using the current texture data
    //!
    //! This function is similar to Texture::generateMipmap and operates
    //! on the texture used as the target for drawing.
    //! Be aware that any draw operation may modify the base level image data.
    //! For this reason, calling this function only makes sense after all
    //! drawing is completed and display has been called. Not calling display
    //! after subsequent drawing will lead to undefined behavior if a mipmap
    //! had been previously generated.
    //!
    //! \return True if mipmap generation was successful, false if unsuccessful
    void generateMipmap();

    //! \brief Update the contents of the target texture
    //!
    //! This function updates the target texture with what
    //! has been drawn so far. Like for windows, calling this
    //! function is mandatory at the end of rendering. Not calling
    //! it may leave the texture in an undefined state.
    void display();

    //! \brief Get a read-only reference to the target texture
    //!
    //! After drawing to the render-texture and calling Display,
    //! you can retrieve the updated texture using this function,
    //! and draw it using a sprite (for example).
    //! The internal sf::Texture of a render-texture is always the
    //! same instance, so that it is possible to call this function
    //! once and keep a reference to the texture even after it is
    //! modified.
    //!
    //! \return Const reference to the texture
    const Texture& getTexture() const;

    ///////////////////////////////////////////////////////////////////////////
    // CANVAS - BASIC                                                        //
    ///////////////////////////////////////////////////////////////////////////

    math::Vector2pz getSize() const override;

    bool isSrgb() const override;

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

    [[nodiscard]] bool setActive(bool aActive = true) override;

    void pushGLStates() override;

    void popGLStates() override;

    void resetGLStates() override;

    ///////////////////////////////////////////////////////////////////////////
    // VIEW CONTROLLER                                                       //
    ///////////////////////////////////////////////////////////////////////////

    void setViewCount(PZInteger aViewCount) override;

    void setView(const View& aView) override;

    void setView(PZInteger aViewIdx, const View& aView) override;

    PZInteger getViewCount() const override;

    const View& getView(PZInteger aViewIdx = 0) const override;

    View& getView(PZInteger aViewIdx = 0) override;

    View getDefaultView() const override;

    math::Rectangle<PZInteger> getViewport(const View& aView) const override;

    math::Rectangle<PZInteger> getViewport(PZInteger aViewIdx) const override;

    math::Vector2f mapPixelToCoords(const math::Vector2i& aPoint, const View& aView) const override;

    math::Vector2f mapPixelToCoords(const math::Vector2i& aPoint, PZInteger aViewIdx) const override;

    math::Vector2i mapCoordsToPixel(const math::Vector2f& point, const View& view) const override;

    math::Vector2i mapCoordsToPixel(const math::Vector2f& point, PZInteger aViewIdx) const override;

private:
    friend class detail::GraphicsImplAccessor;

    void*       _getSFMLImpl();
    const void* _getSFMLImpl() const;

    // clang-format off
    // SFML RenderTexture:
    static constexpr std::size_t SFRT_STORAGE_SIZE  = 528;
    static constexpr std::size_t SFRT_STORAGE_ALIGN =   8;
    std::aligned_storage<SFRT_STORAGE_SIZE, SFRT_STORAGE_ALIGN>::type _storage;

    // SFML RenderTarget adapter:
    static constexpr std::size_t SRTA_STORAGE_SIZE  = 24;
    static constexpr std::size_t SRTA_STORAGE_ALIGN =  8;
    std::aligned_storage<SRTA_STORAGE_SIZE, SRTA_STORAGE_ALIGN>::type _srtaStorage;

    // Multiview adapter:
    static constexpr std::size_t MVA_STORAGE_SIZE  = 208;
    static constexpr std::size_t MVA_STORAGE_ALIGN =   8;
    std::aligned_storage<MVA_STORAGE_SIZE, MVA_STORAGE_ALIGN>::type _mvaStorage;
    //clang-format on

    Texture _texture;
};

} // namespace gr
HOBGOBLIN_NAMESPACE_END

#include <Hobgoblin/Private/Pmacro_undef.hpp>
#include <Hobgoblin/Private/Short_namespace.hpp>

#endif // !UHOBGOBLIN_GRAPHICS_RENDER_TEXTURE_HPP

////////////////////////////////////////////////////////////
/// \class sf::RenderTexture
/// \ingroup graphics
///
/// sf::RenderTexture is the little brother of sf::RenderWindow.
/// It implements the same 2D drawing and OpenGL-related functions
/// (see their base class sf::RenderTarget for more details),
/// the difference is that the result is stored in an off-screen
/// texture rather than being show in a window.
///
/// Rendering to a texture can be useful in a variety of situations:
/// \li precomputing a complex static texture (like a level's background from multiple tiles)
/// \li applying post-effects to the whole scene with shaders
/// \li creating a sprite from a 3D object rendered with OpenGL
/// \li etc.
///
/// Usage example:
///
/// \code
/// // Create a new render-window
/// sf::RenderWindow window(sf::VideoMode({800, 600}), "SFML window");
///
/// // Create a new render-texture
/// sf::RenderTexture texture;
/// if (!texture.create({500, 500}))
///     return -1;
///
/// // The main loop
/// while (window.isOpen())
/// {
///    // Event processing
///    // ...
///
///    // Clear the whole texture with red color
///    texture.clear(sf::Color::Red);
///
///    // Draw stuff to the texture
///    texture.draw(sprite);  // sprite is a sf::Sprite
///    texture.draw(shape);   // shape is a sf::Shape
///    texture.draw(text);    // text is a sf::Text
///
///    // We're done drawing to the texture
///    texture.display();
///
///    // Now we start rendering to the window, clear it first
///    window.clear();
///
///    // Draw the texture
///    sf::Sprite sprite(texture.getTexture());
///    window.draw(sprite);
///
///    // End the current frame and display its contents on screen
///    window.display();
/// }
/// \endcode
///
/// Like sf::RenderWindow, sf::RenderTexture is still able to render direct
/// OpenGL stuff. It is even possible to mix together OpenGL calls
/// and regular SFML drawing commands. If you need a depth buffer for
/// 3D rendering, don't forget to request it when calling RenderTexture::create.
///
/// \see sf::RenderTarget, sf::RenderWindow, sf::View, sf::Texture
///
////////////////////////////////////////////////////////////
