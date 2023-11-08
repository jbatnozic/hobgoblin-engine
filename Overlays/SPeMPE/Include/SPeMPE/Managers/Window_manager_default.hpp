#ifndef SPEMPE_MANAGERS_WINDOW_MANAGER_DEFAULT_HPP
#define SPEMPE_MANAGERS_WINDOW_MANAGER_DEFAULT_HPP

#include <Hobgoblin/Graphics.hpp>
#include <Hobgoblin/RmlUi.hpp>
#include <Hobgoblin/Utility/Time_utils.hpp>
#include <Hobgoblin/Graphics.hpp>
#include <Hobgoblin/Window.hpp>

#include <SPeMPE/GameObjectFramework/Game_object_bases.hpp>
#include <SPeMPE/Managers/Window_manager_interface.hpp>
#include <SPeMPE/Utility/Window_frame_input_view.hpp>
#include <SPeMPE/Utility/Window_input_tracker.hpp>

#include <chrono>
#include <optional>

namespace jbatnozic {
namespace spempe {

class DefaultWindowManager 
    : public WindowManagerInterface
    , public NonstateObject {
public:
    DefaultWindowManager(hg::QAO_RuntimeRef aRuntimeRef,
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

    hg::gr::Canvas& getCanvas() override;

    void setMainRenderTextureDrawPosition(DrawPosition aDrawPosition) override;

    ///////////////////////////////////////////////////////////////////////////
    // VIEWS                                                                 //
    ///////////////////////////////////////////////////////////////////////////

    void setViewCount(hg::PZInteger aViewCount) override;

    hg::PZInteger getViewCount() const override;

    hg::gr::View& getView(hg::PZInteger aViewIndex = 0) override;

    const hg::gr::View& getView(hg::PZInteger aViewIndex = 0) const override;

    ///////////////////////////////////////////////////////////////////////////
    // GUI                                                                   //
    ///////////////////////////////////////////////////////////////////////////

    Rml::Context& getGUIContext() override;

    ///////////////////////////////////////////////////////////////////////////
    // INPUT                                                                 //
    ///////////////////////////////////////////////////////////////////////////
    
    WindowFrameInputView getInput() const override;

private:
    struct MainRenderTexturePositioningData {
        sf::Vector2f position;
        sf::Vector2f origin;
        sf::Vector2f scale;
    };

    // Configuration:
    bool _headless;

    std::chrono::microseconds _deltaTime;
    bool _preciseTiming = true;
    hg::util::Stopwatch _frameDurationStopwatch;

    // Window management:
    std::optional<hg::gr::RenderWindow> _window;
    std::optional<hg::gr::DrawBatcher> _windowDrawBatcher;

    // Main render texture:
    std::optional<hg::gr::RenderTexture> _mainRenderTexture;
    std::optional<hg::gr::DrawBatcher> _mainRenderTextureDrawBatcher;

    DrawPosition _mainRenderTextureDrawPos = DrawPosition::Fit;

    // GUI:
    std::unique_ptr<hg::rml::HobgoblinBackend::BackendLifecycleGuard> _rmlUiBackendLifecycleGuard;
    std::optional<hg::rml::ContextDriver> _rmlUiContextDriver;

    // Input:
    detail::WindowInputTracker _inputTracker;

    void _eventPostUpdate() override;
    void _eventDraw2() override;
    void _eventFinalizeFrame() override;

    MainRenderTexturePositioningData _getMainRenderTexturePositioningData() const;

    void _drawMainRenderTexture();
    void _finalizeFrameByDisplayingWindow();
    void _finalizeFrameBySleeping();

    sf::Vector2f _getViewRelativeMousePos(hobgoblin::PZInteger aViewIndex) const;
    sf::Vector2i _getWindowRelativeMousePos() const;
};

} // namespace spempe
} // namespace jbatnozic

#endif // !SPEMPE_MANAGERS_WINDOW_MANAGER_DEFAULT_HPP