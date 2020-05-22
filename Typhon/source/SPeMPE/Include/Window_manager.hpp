#ifndef SPEMPE_WINDOW_MANAGER
#define SPEMPE_WINDOW_MANAGER

#include <Hobgoblin/Graphics.hpp>
#include <Hobgoblin/Utility/Time_utils.hpp>
#include <SFML/Window.hpp>
#include <SFML/Graphics.hpp>

#include <SPeMPE/Include/Game_object_framework.hpp>
#include <SPeMPE/Include/Keyboard_input.hpp>

namespace spempe {

class WindowManager : public NonstateObject {
public:
    enum class DrawPosition {
        Fill,
        Fit,
        Stretch,
        Centre
    };

    WindowManager(hg::QAO_RuntimeRef runtimeRef);
    WindowManager(hg::QAO_RuntimeRef runtimeRef, sf::Vector2u windowSize, const sf::String& windowTitle,
                  sf::Vector2u mainRenderTargetSize);

    void create(); // TODO Temp.
    void initAsHeadless();

    sf::RenderWindow& getWindow();
    sf::RenderTexture& getMainRenderTexture();
    hg::gr::Canvas& getCanvas();

    void eventPostUpdate() override;
    void eventDraw2() override;
    void eventFinalizeFrame() override;

    void drawMainRenderTexture(DrawPosition drawPosition);

    KbInputTracker& getKeyboardInput();

    sf::Vector2f getMousePos(hg::PZInteger viewIndex = 0) const {
        auto pixelPos = sf::Mouse::getPosition(_window);
        return _window.mapPixelToCoords(pixelPos, getView(viewIndex));
    }

    // Views:
    void setViewCount(hg::PZInteger viewCount);
    hg::PZInteger getViewCount() const noexcept;
    sf::View& getView(hg::PZInteger viewIndex = 0);
    const sf::View& getView(hg::PZInteger viewIndex = 0) const;

private:
    sf::RenderWindow _window;
    sf::RenderTexture _mainRenderTexture;
    hg::gr::CanvasAdapter _windowAdapter;
    hg::gr::MultiViewRenderTargetAdapter _mainRenderTextureAdapter;
    KbInputTracker _kbi;
    hg::util::Stopwatch _frameDurationStopwatch;
    bool _isHeadless = false;

    void _finalizeFrameByDisplayingWindow();
    void _finalizeFrameBySleeping();
};

} // namespace spempe

#endif // !SPEMPE_WINDOW_MANAGER

