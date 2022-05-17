#ifndef SPEMPE_MANAGERS_WINDOW_MANAGER_INTERFACE_HPP
#define SPEMPE_MANAGERS_WINDOW_MANAGER_INTERFACE_HPP

#include <Hobgoblin/Graphics.hpp>
#include <SFML/Graphics.hpp>
#include <SFML/Window.hpp>

#include <SPeMPE/GameContext/Context_components.hpp>
#include <SPeMPE/Other/Keyboard_input.hpp>
#include <SPeMPE/Other/Mouse_input.hpp>

namespace jbatnozic {
namespace spempe {

class WindowManagerInterface : public ContextComponent {
public:
    virtual ~WindowManagerInterface() = default;

    ///////////////////////////////////////////////////////////////////////////
    // CONFIGURATION                                                         //
    ///////////////////////////////////////////////////////////////////////////

    enum class Mode {
        Headless,
        Normal
    };

    struct WindowConfig {
        WindowConfig(const sf::VideoMode& aVideoMode,
                     const sf::String& aTitle,
                     sf::Uint32 aStyle = sf::Style::Default,
                     const sf::ContextSettings& aOpenGlContextSettings = sf::ContextSettings{});

        sf::VideoMode videoMode;
        sf::String title;
        sf::Uint32 style;
        sf::ContextSettings openGlContextSettings;
    };

    struct MainRenderTextureConfig {
        MainRenderTextureConfig(const sf::Vector2u& aSize,
                                const bool aSmooth = true,
                                const sf::ContextSettings& aOpenGlContextSettings = sf::ContextSettings{});

        sf::Vector2u size;
        bool smooth;
        sf::ContextSettings openGlContextSettings;
    };

    struct TimingConfig {
        TimingConfig(hg::PZInteger aTargetFramerate = 60,
                     bool aFramerateLimiter = false,
                     bool aVerticalSync = true,
                     bool aPreciseTiming = true);

        hg::PZInteger targetFramerate;
        bool framerateLimiter;
        bool verticalSync;
        bool preciseTiming;
    };

    virtual void setToHeadlessMode(const TimingConfig& aTimingConfig) = 0;

    virtual void setToNormalMode(const WindowConfig& aWindowConfig,
                                 const MainRenderTextureConfig& aMainRenderTextureConfig,
                                 const TimingConfig& aTimingConfig) = 0;

    ///////////////////////////////////////////////////////////////////////////
    // WINDOW MANAGEMENT                                                     //
    ///////////////////////////////////////////////////////////////////////////

    //_window.getSettings();
    //_window.getPosition();
    //_window.getSize();
    //_window.hasFocus();
    //_window.mapCoordsToPixel();
    //_window.mapPixelToCoords();
    //_window.setFramerateLimit();
    //_window.setIcon();
    //_window.setPosition();
    //_window.setVerticalSyncEnabled();
    //_window.setTitle();
    //_window.setSize();
    //_window.setMouseCursor();
    //_window.setMouseCursorGrabbed();
    //_window.setMouseCursorVisible();
    //framerate limiter

    ///////////////////////////////////////////////////////////////////////////
    // GRAPHICS & DRAWING                                                    //
    ///////////////////////////////////////////////////////////////////////////

    // sf::RenderTexture& getMainRenderTexture();

    virtual hg::gr::Canvas& getCanvas() = 0;

    enum class DrawPosition {
        None,    //! Not drawn at all.
        Fill,    //! Scaled (keeping aspect ratio) and centred so that the whole window is filled.
        Fit,     //! Scaled (keeping aspect ratio) and centred so that the whole texture is visible.        
        Centre,  //! Kept at original size and just centred in the window.
        Stretch, //! Stretched (or compressed) to the window's exact size. 
        TopLeft, //! Kept at original size and top left corner aligned with window's top left corner.
    };

    //! Default draw position is Fit.
    virtual void setMainRenderTextureDrawPosition(DrawPosition aDrawPosition) = 0;

    ///////////////////////////////////////////////////////////////////////////
    // VIEWS                                                                 //
    ///////////////////////////////////////////////////////////////////////////

    virtual void setViewCount(hg::PZInteger aViewCount) = 0;

    virtual hg::PZInteger getViewCount() const = 0;

    virtual sf::View& getView(hg::PZInteger aViewIndex = 0) = 0;

    virtual const sf::View& getView(hg::PZInteger aViewIndex = 0) const = 0;

    ///////////////////////////////////////////////////////////////////////////
    // GUI                                                                   //
    ///////////////////////////////////////////////////////////////////////////

    ///////////////////////////////////////////////////////////////////////////
    // KEYBOARD & MOUSE INPUT                                                //
    ///////////////////////////////////////////////////////////////////////////

    virtual KbInput getKeyboardInput() = 0;

    virtual const KbInput getKeyboardInput() const = 0;

    //virtual MouseInput getMouseInput() = 0;

    //virtual const MouseInput getMouseInput() const = 0;

private:
    SPEMPE_CTXCOMP_TAG("jbatnozic::spempe::WindowManagerInterface");
};

inline
WindowManagerInterface::WindowConfig::WindowConfig(
    const sf::VideoMode& aVideoMode,
    const sf::String& aTitle,
    sf::Uint32 aStyle,
    const sf::ContextSettings& aOpenGlContextSettings) 
    : videoMode{aVideoMode}
    , title{aTitle}
    , style{aStyle}
    , openGlContextSettings{aOpenGlContextSettings}
{
}

inline
WindowManagerInterface::MainRenderTextureConfig::MainRenderTextureConfig(
    const sf::Vector2u& aSize,
    const bool aSmooth,
    const sf::ContextSettings& aOpenGlContextSettings)
    : size{aSize}
    , smooth{aSmooth}
    , openGlContextSettings{aOpenGlContextSettings}
{
}

inline
WindowManagerInterface::TimingConfig::TimingConfig(
    hg::PZInteger aTargetFramerate,
    bool aFramerateLimiter,
    bool aVerticalSync,
    bool aPreciseTiming)
    : targetFramerate{aTargetFramerate}
    , framerateLimiter{aFramerateLimiter}
    , verticalSync{aVerticalSync}
    , preciseTiming{aPreciseTiming}
{
}

} // namespace spempe
} // namespace jbatnozic

#endif // !SPEMPE_MANAGERS_WINDOW_MANAGER_INTERFACE_HPP
