////////////////////////////////////////////////////////////
//
// SFML - Simple and Fast Multimedia Library
// Copyright (C) 2007-2018 Laurent Gomila (laurent@sfml-dev.org)
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

#ifndef UHOBGOBLIN_GRAPHICS_RENDER_WINDOW_HPP
#define UHOBGOBLIN_GRAPHICS_RENDER_WINDOW_HPP

#include <Hobgoblin/Graphics/Render_target.hpp>
#include <Hobgoblin/Graphics/Window.hpp>
#include <Hobgoblin/Math/Vector.hpp>

#include <string>

#include <Hobgoblin/Private/Pmacro_define.hpp>

HOBGOBLIN_NAMESPACE_BEGIN
namespace gr {
  
//! \brief Window that can serve as a target for 2D drawing.
class RenderWindow : public Window, public RenderTarget {
public:
    //! \brief Default constructor
    //!
    //! This constructor doesn't actually create the window,
    //! use the other constructors or call create() to do so.
    RenderWindow();

    //! \brief Construct a new window
    //!
    //! This constructor creates the window with the size and pixel
    //! depth defined in \a mode. An optional style can be passed to
    //! customize the look and behavior of the window (borders,
    //! title bar, resizable, closable, ...).
    //!
    //! The fourth parameter is an optional structure specifying
    //! advanced OpenGL context settings such as antialiasing,
    //! depth-buffer bits, etc. You shouldn't care about these
    //! parameters for a regular usage of the graphics module.
    //!
    //! \param mode     Video mode to use (defines the width, height and depth of the rendering area of the window)
    //! \param title    Title of the window
    //! \param style    %Window style, a bitwise OR combination of sf::Style enumerators
    //! \param settings Additional settings for the underlying OpenGL context
    RenderWindow(VideoMode aMode, const std::string& aTitle, WindowStyle aStyle = WindowStyle::Default, const ContextSettings& aSettings = ContextSettings{});

    //! \brief Construct the window from an existing control
    //!
    //! Use this constructor if you want to create an SFML
    //! rendering area into an already existing control.
    //!
    //! The second parameter is an optional structure specifying
    //! advanced OpenGL context settings such as antialiasing,
    //! depth-buffer bits, etc. You shouldn't care about these
    //! parameters for a regular usage of the graphics module.
    //!
    //! \param handle   Platform-specific handle of the control (\a HWND on
    //!                 Windows, \a %Window on Linux/FreeBSD, \a NSWindow on OS X)
    //! \param settings Additional settings for the underlying OpenGL context
    explicit RenderWindow(WindowHandle aHandle, const ContextSettings& aSettings = ContextSettings{});

    //! \brief Destructor
    //!
    //! Closes the window and frees all the resources attached to it.
    virtual ~RenderWindow();

    //! \brief Tell if the window will use sRGB encoding when drawing on it
    //!
    //! You can request sRGB encoding for a window by having the sRgbCapable flag set in the ContextSettings
    //!
    //! \return True if the window use sRGB encoding, false otherwise
    virtual bool isSrgb() const;

    ///////////////////////////////////////////////////////////////////////////
    // RENDER TARGET                                                         //
    ///////////////////////////////////////////////////////////////////////////

    // TODO: see which overriden methods should be final

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

    void clear(const Color& aColor = COLOR_BLACK) override;

    void setViewCount(PZInteger aViewCount) override;

    void setView(const View& aView, PZInteger aViewIdx = 0) override;

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

    math::Vector2pz getSize() const override;

    bool setActive(bool aActive = true) override;

    void pushGLStates() override;

    void popGLStates() override;

    void resetGLStates() override;

protected:
    //! \brief Function called after the window has been created
    //!
    //! This function is called so that derived classes can
    //! perform their own specific initialization as soon as
    //! the window is created.
    virtual void onCreate();

    //! \brief Function called after the window has been resized
    //!
    //! This function is called so that derived classes can
    //! perform custom actions when the size of the window changes.
    virtual void onResize();

private:
    // Multiview adapter:

    static constexpr std::size_t MVA_STORAGE_SIZE  = 208;
    static constexpr std::size_t MVA_STORAGE_ALIGN = 8;
    std::aligned_storage<MVA_STORAGE_SIZE, MVA_STORAGE_ALIGN>::type _mvaStorage;

    void getCanvasDetails(CanvasType& aType, void*& aRenderingBackend) override;
};

} // namespace gr
HOBGOBLIN_NAMESPACE_END

#include <Hobgoblin/Private/Pmacro_undef.hpp>
#include <Hobgoblin/Private/Short_namespace.hpp>

#endif // !UHOBGOBLIN_GRAPHICS_RENDER_WINDOW_HPP

////////////////////////////////////////////////////////////
/// \class sf::RenderTarget
/// \ingroup graphics
///
/// sf::RenderTarget defines the common behavior of all the
/// 2D render targets usable in the graphics module. It makes
/// it possible to draw 2D entities like sprites, shapes, text
/// without using any OpenGL command directly.
///
/// A sf::RenderTarget is also able to use views (sf::View),
/// which are a kind of 2D cameras. With views you can globally
/// scroll, rotate or zoom everything that is drawn,
/// without having to transform every single entity. See the
/// documentation of sf::View for more details and sample pieces of
/// code about this class.
///
/// On top of that, render targets are still able to render direct
/// OpenGL stuff. It is even possible to mix together OpenGL calls
/// and regular SFML drawing commands. When doing so, make sure that
/// OpenGL states are not messed up by calling the
/// pushGLStates/popGLStates functions.
///
/// \see sf::RenderWindow, sf::RenderTexture, sf::View
///
////////////////////////////////////////////////////////////
