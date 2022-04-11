#ifndef SPEMPE_MANAGERS_WINDOW_MANAGER_ONE_HPP
#define SPEMPE_MANAGERS_WINDOW_MANAGER_ONE_HPP

#include <Hobgoblin/Graphics.hpp>
#include <Hobgoblin/Utility/Time_utils.hpp>
#include <SFML/Graphics.hpp>
#include <SFML/Window.hpp>

#include <SPeMPE/GameObjectFramework/Game_object_bases.hpp>
#include <SPeMPE/Managers/Window_manager_interface.hpp>
#include <SPeMPE/Other/Keyboard_input.hpp>

#include <chrono>
#include <optional>

namespace jbatnozic {
namespace spempe {

class WindowManagerOne 
    : public WindowManagerInterface
    , public NonstateObject {
public:
    WindowManagerOne(hg::QAO_RuntimeRef aRuntimeRef,
                     int aExecutionPriority);

    ///////////////////////////////////////////////////////////////////////////
    // CONFIGURATION                                                         //
    ///////////////////////////////////////////////////////////////////////////

    void setToHeadlessMode(const TimingConfig& aTimingConfig) override;

    void setToNormalMode(const WindowConfig& aWindowConfig,
                         const MainRenderTextureConfig& aMainRenderTextureConfig,
                         const TimingConfig& aTimingConfig) override;

    ///////////////////////////////////////////////////////////////////////////
    // WINDOW MANAGEMENT                                                     //
    ///////////////////////////////////////////////////////////////////////////

    ///////////////////////////////////////////////////////////////////////////
    // GRAPHICS & DRAWING                                                    //
    ///////////////////////////////////////////////////////////////////////////

    // sf::RenderTexture& getMainRenderTexture();

    virtual hg::gr::Canvas& getCanvas() override;

    void setMainRenderTextureDrawPosition(DrawPosition aDrawPosition) override;

    ///////////////////////////////////////////////////////////////////////////
    // VIEWS                                                                 //
    ///////////////////////////////////////////////////////////////////////////

    void setViewCount(hg::PZInteger aViewCount) override;

    hg::PZInteger getViewCount() const override;

    sf::View& getView(hg::PZInteger aViewIndex = 0) override;

    const sf::View& getView(hg::PZInteger aViewIndex = 0) const override;

    ///////////////////////////////////////////////////////////////////////////
    // GUI                                                                   //
    ///////////////////////////////////////////////////////////////////////////

    ///////////////////////////////////////////////////////////////////////////
    // KEYBOARD & MOUSE INPUT                                                //
    ///////////////////////////////////////////////////////////////////////////

    KbInput getKeyboardInput() override;

    const KbInput getKeyboardInput() const override;

    // getMouseInput();

private:
    // Configuration:
    bool _headless;

    std::chrono::microseconds _deltaTime;
    bool _preciseTiming = true;
    hg::util::Stopwatch _frameDurationStopwatch;

    // Window management:
    std::optional<sf::RenderWindow> _window;

    // Graphics & drawing:
    std::optional<sf::RenderTexture> _mainRenderTexture;
    std::optional<hg::gr::CanvasAdapter> _windowToCanvasAdapter;

    DrawPosition _mainRenderTextureDrawPos = DrawPosition::Fit;

    // Views:
    std::optional<hg::gr::MultiViewRenderTargetAdapter> _mainRenderTextureViewAdapter;

    // GUI:

    // Keyboard & mouse input:
    KbInputTracker _kbInputTracker;

    void _eventPostUpdate() override;
    void _eventDraw2() override;
    void _eventFinalizeFrame() override;

    void _drawMainRenderTexture();
    void _finalizeFrameByDisplayingWindow();
    void _finalizeFrameBySleeping();
};

} // namespace spempe
} // namespace jbatnozic

#endif // !SPEMPE_MANAGERS_WINDOW_MANAGER_ONE_HPP