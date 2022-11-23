
#include <SPeMPE/Managers/Window_manager_default.hpp>

#include <Hobgoblin/Common.hpp>

#define HARD_ASSERT(_expr_) do { if (!(_expr_)) { \
        throw ::jbatnozic::hobgoblin::TracedLogicError{#_expr_}; \
    } } while (false)

namespace jbatnozic {
namespace spempe {

DefaultWindowManager::DefaultWindowManager(hg::QAO_RuntimeRef aRuntimeRef,
                                           int aExecutionPriority)
    : NonstateObject{aRuntimeRef, SPEMPE_TYPEID_SELF, aExecutionPriority, "::jbatnozic::spempe::DefaultWindowManager"}
    , _window{}
    , _windowToCanvasAdapter{}
    , _windowDrawBatcher{}
    , _mainRenderTexture{}
    , _mainRenderTextureViewAdapter{}
    , _mainRenderTextureDrawBatcher{}
    , _rmlUiContextDriver{}
    , _mouseInputTracker{
        [this](hg::PZInteger aViewIndex) -> sf::Vector2f {
            return _getViewRelativeMousePos(aViewIndex);
        },
        [this]() -> sf::Vector2i {
            return _getWindowRelativeMousePos();
        }}
{
}

///////////////////////////////////////////////////////////////////////////
// CONFIGURATION                                                         //
///////////////////////////////////////////////////////////////////////////

void DefaultWindowManager::setToHeadlessMode(const TimingConfig& aTimingConfig) {
    SPEMPE_VERIFY_GAME_CONTEXT_FLAGS(ctx(), headless==true);

    _headless = true;

    _mainRenderTextureDrawBatcher.reset();
    _mainRenderTextureViewAdapter.reset();
    _mainRenderTexture.reset();
    _windowDrawBatcher.reset();
    _windowToCanvasAdapter.reset();
    _window.reset();

    // Set timing parameters:
    _deltaTime = std::chrono::microseconds{1'000'000 / aTimingConfig.targetFramerate};
    _preciseTiming = aTimingConfig.preciseTiming;
}

void DefaultWindowManager::setToNormalMode(const WindowConfig& aWindowConfig,
                                           const MainRenderTextureConfig& aMainRenderTextureConfig,
                                           const TimingConfig& aTimingConfig) {
    SPEMPE_VERIFY_GAME_CONTEXT_FLAGS(ctx(), headless==false);

    _headless = false;

    // Create window:
    _window.emplace(aWindowConfig.videoMode, 
                    aWindowConfig.title, 
                    aWindowConfig.style, 
                    aWindowConfig.openGlContextSettings);

    // Create main render texture:
    _mainRenderTexture.emplace();
    _mainRenderTexture->create(aMainRenderTextureConfig.size.x,
                               aMainRenderTextureConfig.size.y,
                               aMainRenderTextureConfig.openGlContextSettings);
    _mainRenderTexture->setSmooth(aMainRenderTextureConfig.smooth);

    // Create adapters:
    _windowToCanvasAdapter.emplace(*_window);
    _windowDrawBatcher.emplace(*_windowToCanvasAdapter);
    _mainRenderTextureViewAdapter.emplace(*_mainRenderTexture);
    _mainRenderTextureDrawBatcher.emplace(*_mainRenderTextureViewAdapter);

    // Create GUI:
    _rmlUiBackendLifecycleGuard = hg::rml::HobgoblinBackend::initialize();
    _rmlUiContextDriver.emplace("DefaultWindowManager::RmlContext", *_window);

    // Create default view:
    const auto w = static_cast<float>(aMainRenderTextureConfig.size.x);
    const auto h = static_cast<float>(aMainRenderTextureConfig.size.y);

    _mainRenderTextureViewAdapter->setViewCount(1);
    _mainRenderTextureViewAdapter->getView(0).setSize({w, h});
    _mainRenderTextureViewAdapter->getView(0).setCenter({w * 0.5f, h * 0.5f});
    _mainRenderTextureViewAdapter->getView(0).setViewport({0.f, 0.f, 1.f, 1.f});

    // Set timing parameters:
    _deltaTime = std::chrono::microseconds{1'000'000 / aTimingConfig.targetFramerate};
    _window->setFramerateLimit(aTimingConfig.framerateLimiter ? aTimingConfig.targetFramerate : 0);
    _window->setVerticalSyncEnabled(aTimingConfig.verticalSync);
    _preciseTiming = aTimingConfig.preciseTiming;
}

///////////////////////////////////////////////////////////////////////////
// WINDOW MANAGEMENT                                                     //
///////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////
// GRAPHICS & DRAWING                                                    //
///////////////////////////////////////////////////////////////////////////

// sf::RenderTexture& getMainRenderTexture();

hg::gr::Canvas& DefaultWindowManager::getCanvas() {
    HARD_ASSERT(!_headless);
    if (getRuntime()->getCurrentEvent() == hg::QAO_Event::DrawGUI) {
        return *_windowDrawBatcher;
    }
    else {
        return *_mainRenderTextureDrawBatcher;
    }
}

void DefaultWindowManager::setMainRenderTextureDrawPosition(DrawPosition aDrawPosition) {
    _mainRenderTextureDrawPos = aDrawPosition;
}

///////////////////////////////////////////////////////////////////////////
// VIEWS                                                                 //
///////////////////////////////////////////////////////////////////////////

void DefaultWindowManager::setViewCount(hg::PZInteger aViewCount) {
    HARD_ASSERT(!_headless);
    _mainRenderTextureViewAdapter->setViewCount(aViewCount);
}

hg::PZInteger DefaultWindowManager::getViewCount() const {
    HARD_ASSERT(!_headless);
    return _mainRenderTextureViewAdapter->getViewCount();
}

sf::View& DefaultWindowManager::getView(hg::PZInteger aViewIndex) {
    HARD_ASSERT(!_headless);
    return _mainRenderTextureViewAdapter->getView(aViewIndex);
}

const sf::View& DefaultWindowManager::getView(hg::PZInteger aViewIndex) const {
    HARD_ASSERT(!_headless);
    return _mainRenderTextureViewAdapter->getView(aViewIndex);
}

///////////////////////////////////////////////////////////////////////////
// GUI                                                                   //
///////////////////////////////////////////////////////////////////////////

Rml::Context& DefaultWindowManager::getGUIContext() {
    HARD_ASSERT(!_headless);
    return *(*_rmlUiContextDriver);
}

///////////////////////////////////////////////////////////////////////////
// KEYBOARD & MOUSE INPUT                                                //
///////////////////////////////////////////////////////////////////////////

KbInput DefaultWindowManager::getKeyboardInput() const {
    return _kbInputTracker.getInput();
}

KbInputMutator DefaultWindowManager::getKeyboardInputMutator() {
    return _kbInputTracker.getMutator();
}

MouseInput DefaultWindowManager::getMouseInput() const {
    return _mouseInputTracker.getInput();
}

MouseInputMutator DefaultWindowManager::getMouseInputMutator() {
    return _mouseInputTracker.getMutator();
}

///////////////////////////////////////////////////////////////////////////
// PRIVATE METHODS                                                       //
///////////////////////////////////////////////////////////////////////////

void DefaultWindowManager::_eventPostUpdate() {
    if (!_headless) {
        _mainRenderTexture->clear(hg::gr::Color::Black); // TODO Parametrize colour
    }
}

void DefaultWindowManager::_eventDraw2() {
    if (!_headless) {
        _window->clear(sf::Color::Black); // TODO Parametrize colour
        _drawMainRenderTexture();
    }
}

void DefaultWindowManager::_eventFinalizeFrame() {
    if (!_headless) {
        _finalizeFrameByDisplayingWindow();
    }
    else {
        if (_preciseTiming) {
            _finalizeFrameBySleeping();
        }
    }
}

DefaultWindowManager::MainRenderTexturePositioningData DefaultWindowManager::_getMainRenderTexturePositioningData() const {
    MainRenderTexturePositioningData result;

    const sf::Vector2u mrtSize = _mainRenderTexture->getSize();
    const sf::Vector2u winSize = _window->getSize();

    switch (_mainRenderTextureDrawPos) {
    case DrawPosition::None:
        result.position = {0.f, 0.f};
        result.origin   = {0.f, 0.f};
        result.scale    = {0.f, 0.f};
        break;

    case DrawPosition::Fill:
    case DrawPosition::Fit:
    {
        float scale;
        if (_mainRenderTextureDrawPos == DrawPosition::Fill) {
            scale = std::max(static_cast<float>(winSize.x) / mrtSize.x,
                             static_cast<float>(winSize.y) / mrtSize.y);
        }
        else {
            scale = std::min(static_cast<float>(winSize.x) / mrtSize.x,
                             static_cast<float>(winSize.y) / mrtSize.y);
        }
        result.scale = {scale, scale};
    }
    // FALLTHROUGH

    case DrawPosition::Centre:
        if (_mainRenderTextureDrawPos == DrawPosition::Centre) {
            result.scale = {1.f, 1.f};
        }

        result.position = {static_cast<float>(winSize.x) * 0.5f,
                           static_cast<float>(winSize.y) * 0.5f};
        result.origin = {static_cast<float>(mrtSize.x) * 0.5f,
                         static_cast<float>(mrtSize.y) * 0.5f};
        break;

    case DrawPosition::Stretch:
        result.position = {0.f, 0.f};
        result.origin   = {0.f, 0.f};
        result.scale    = {static_cast<float>(winSize.x) / mrtSize.x,
                            static_cast<float>(winSize.y) / mrtSize.y};
        break;

    case DrawPosition::TopLeft:
        result.position = {0.f, 0.f};
        result.origin   = {0.f, 0.f};
        result.scale    = {1.f, 1.f};
        break;

    default:
        assert(false && "Unreachable");
    }

    return result;
}

void DefaultWindowManager::_drawMainRenderTexture() {
    _mainRenderTextureDrawBatcher->flush();
    _mainRenderTexture->display();
    sf::Sprite mrtSprite{_mainRenderTexture->getTexture()};

    const auto mrtPositioning = _getMainRenderTexturePositioningData();

    mrtSprite.setPosition(mrtPositioning.position);
    mrtSprite.setOrigin(mrtPositioning.origin);
    mrtSprite.setScale(mrtPositioning.scale);

    _window->draw(mrtSprite);
}

void DefaultWindowManager::_finalizeFrameByDisplayingWindow() {
    _windowDrawBatcher->flush();
    _rmlUiContextDriver->update();
    _rmlUiContextDriver->render();
    _window->display();

    _kbInputTracker.prepForEvents();
    _mouseInputTracker.prepForEvents();

    sf::Event ev;
    while (_window->pollEvent(ev)) {
        const auto rmlUiDidConsumeEvent = _rmlUiContextDriver->processEvent(ev);
        if (rmlUiDidConsumeEvent) {
            continue;
        }

        switch (ev.type) {
        case sf::Event::Closed:
            // TODO
            break;

        case sf::Event::Resized:
            {
                sf::FloatRect visibleArea(0.f,
                                          0.f,
                                          static_cast<float>(ev.size.width),
                                          static_cast<float>(ev.size.height));
                _window->setView(sf::View(visibleArea));
            }
            break;

        case sf::Event::TextEntered:
            _kbInputTracker.textEventOccurred(ev);
            break;

        case sf::Event::KeyPressed:
        case sf::Event::KeyReleased:
            _kbInputTracker.keyEventOccurred(ev);
            break;

        case sf::Event::MouseWheelScrolled:
        case sf::Event::MouseButtonPressed:
        case sf::Event::MouseButtonReleased:
        case sf::Event::MouseMoved:
        case sf::Event::MouseEntered:
        case sf::Event::MouseLeft:
            _mouseInputTracker.buttonEventOccurred(ev);
            break;

        default: (void)0;
        }
    }

    if (_preciseTiming) {
        // I don't know if this is a good idea, but with vSync on, it waits in random places, including
        // in the draw events, which screws up timing measurements. So with this call, we force it to
        // synchronize during the finalizeFrame event.
        _finalizeFrameBySleeping();
    }
}

void DefaultWindowManager::_finalizeFrameBySleeping() {
    using Time = std::chrono::microseconds;
    const auto deltaTime = std::chrono::duration_cast<Time>(ctx().getRuntimeConfig().deltaTime);
    const auto timePassed = _frameDurationStopwatch.getElapsedTime<Time>();

    if (timePassed < deltaTime) {
        hg::util::SuperPreciseSleep(deltaTime - timePassed);
    }

    _frameDurationStopwatch.restart();
}

sf::Vector2f DefaultWindowManager::_getViewRelativeMousePos(hobgoblin::PZInteger aViewIndex) const {
    if (_headless) {
        return {0.f, 0.f};
    }

    const auto mrtPositioning = _getMainRenderTexturePositioningData();
    const auto pixelPos = sf::Mouse::getPosition(*_window);

    auto windowPos = _window->mapPixelToCoords(pixelPos);
    windowPos.x = (windowPos.x - mrtPositioning.position.x) / mrtPositioning.scale.x + mrtPositioning.origin.x;
    windowPos.y = (windowPos.y - mrtPositioning.position.y) / mrtPositioning.scale.y + mrtPositioning.origin.y;

    const sf::Vector2i windowPosI = {static_cast<int>(windowPos.x), static_cast<int>(windowPos.y)};

    return _mainRenderTexture->mapPixelToCoords(windowPosI, _mainRenderTextureViewAdapter->getView(aViewIndex));
}

sf::Vector2i DefaultWindowManager::_getWindowRelativeMousePos() const {
    if (_headless) {
        return {0, 0};
    }

    return sf::Mouse::getPosition(*_window);
}

} // namespace spempe
} // namespace jbatnozic
