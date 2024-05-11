// Copyright 2024 Jovan Batnozic. Released under MS-PL licence in Serbia.
// See https://github.com/jbatnozic/Hobgoblin?tab=readme-ov-file#licence

// clang-format off


#include <SPeMPE/Managers/Window_manager_default.hpp>

#include <Hobgoblin/HGExcept.hpp>
#include <Hobgoblin/Common.hpp>
#include <Hobgoblin/Window.hpp>
#include <Hobgoblin/Math/Vector.hpp>

#include <iostream>

namespace jbatnozic {
namespace spempe {

DefaultWindowManager::DefaultWindowManager(hg::QAO_RuntimeRef aRuntimeRef,
                                           int aExecutionPriority)
    : NonstateObject{aRuntimeRef, SPEMPE_TYPEID_SELF, aExecutionPriority, "::jbatnozic::spempe::DefaultWindowManager"}
    , _window{}
    , _windowDrawBatcher{}
    , _mainRenderTexture{}
    , _mainRenderTextureDrawBatcher{}
    , _rmlUiContextDriver{}
    , _inputTracker{
        [this](hg::PZInteger aViewIndex) -> hg::math::Vector2f {
            return _getViewRelativeMousePos(aViewIndex);
        },
        [this]() -> hg::math::Vector2i {
            return _getWindowRelativeMousePos();
        }}
{
}

///////////////////////////////////////////////////////////////////////////
// CONFIGURATION                                                         //
///////////////////////////////////////////////////////////////////////////

void DefaultWindowManager::setToHeadlessMode(const TimingConfig& aTimingConfig) {
    SPEMPE_VALIDATE_GAME_CONTEXT_FLAGS(ctx(), headless==true);

    _headless = true;

    _mainRenderTextureDrawBatcher.reset();
    _mainRenderTexture.reset();
    _windowDrawBatcher.reset();
    _window.reset();

    // Set timing parameters:
    _deltaTime = std::chrono::microseconds{1'000'000 / aTimingConfig.targetFramerate};
    _preciseTiming = aTimingConfig.preciseTiming;
}

void DefaultWindowManager::setToNormalMode(const WindowConfig& aWindowConfig,
                                           const MainRenderTextureConfig& aMainRenderTextureConfig,
                                           const TimingConfig& aTimingConfig) {
    SPEMPE_VALIDATE_GAME_CONTEXT_FLAGS(ctx(), headless==false);

    _headless = false;

    // Create window:
    _window.emplace(aWindowConfig.videoMode, 
                    aWindowConfig.title, 
                    aWindowConfig.style, 
                    aWindowConfig.openGlContextSettings);

    _window->setView(hg::gr::View{{
        0.f,
        0.f,
        static_cast<float>(aWindowConfig.videoMode.width),
        static_cast<float>(aWindowConfig.videoMode.height)
    }});

    // Create main render texture:
    _mainRenderTexture.emplace();
    _mainRenderTexture->create(aMainRenderTextureConfig.size,
                               aMainRenderTextureConfig.openGlContextSettings);
    _mainRenderTexture->setSmooth(aMainRenderTextureConfig.smooth);

    {
        const auto w = static_cast<float>(aMainRenderTextureConfig.size.x);
        const auto h = static_cast<float>(aMainRenderTextureConfig.size.y);

        _mainRenderTexture->setViewCount(1);
        _mainRenderTexture->getView(0).setSize({w, h});
        _mainRenderTexture->getView(0).setCenter({w * 0.5f, h * 0.5f});
        _mainRenderTexture->getView(0).setViewport({0.f, 0.f, 1.f, 1.f});
    }

    // Create adapters:
    _windowDrawBatcher.emplace(*_window);
    _mainRenderTextureDrawBatcher.emplace(*_mainRenderTexture);

    // Create GUI:
    _rmlUiBackendLifecycleGuard = hg::rml::HobgoblinBackend::initialize();
    _rmlUiContextDriver.emplace("DefaultWindowManager::RmlContext", *_window);

    // Set timing parameters:
    _deltaTime = std::chrono::microseconds{1'000'000 / aTimingConfig.targetFramerate};
    _window->setFramerateLimit(aTimingConfig.framerateLimiter ? aTimingConfig.targetFramerate : 0);
    _window->setVerticalSyncEnabled(aTimingConfig.verticalSync);
    _preciseTiming = aTimingConfig.preciseTiming;
}

///////////////////////////////////////////////////////////////////////////
// WINDOW MANAGEMENT                                                     //
///////////////////////////////////////////////////////////////////////////

hobgoblin::math::Vector2pz DefaultWindowManager::getWindowSize() const {
    return _window->getSize();
}

///////////////////////////////////////////////////////////////////////////
// GRAPHICS & DRAWING                                                    //
///////////////////////////////////////////////////////////////////////////

// sf::RenderTexture& getMainRenderTexture();

hg::gr::Canvas& DefaultWindowManager::getCanvas() {
    HG_HARD_ASSERT(!_headless && "Method not available in Headless mode.");
    if (getRuntime()->getCurrentEvent() == hg::QAO_Event::DRAW_GUI) {
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
    HG_HARD_ASSERT(!_headless && "Method not available in Headless mode.");
    _mainRenderTexture->setViewCount(aViewCount);
}

hg::PZInteger DefaultWindowManager::getViewCount() const {
    HG_HARD_ASSERT(!_headless && "Method not available in Headless mode.");
    return _mainRenderTexture->getViewCount();
}

hg::gr::View& DefaultWindowManager::getView(hg::PZInteger aViewIndex) {
    HG_HARD_ASSERT(!_headless && "Method not available in Headless mode.");
    return _mainRenderTexture->getView(aViewIndex);
}

const hg::gr::View& DefaultWindowManager::getView(hg::PZInteger aViewIndex) const {
    HG_HARD_ASSERT(!_headless && "Method not available in Headless mode.");
    return _mainRenderTexture->getView(aViewIndex);
}

hg::math::Vector2f DefaultWindowManager::mapPixelToCoords(const hg::math::Vector2i& aPoint,
                                                          const hg::gr::View& aView) const {
    HG_HARD_ASSERT(!_headless && "Method not available in Headless mode.");

    const auto mrtPositioning = _getMainRenderTexturePositioningData();

    auto windowPos = _window->mapPixelToCoords(aPoint, _window->getView());
    windowPos.x = (windowPos.x - mrtPositioning.position.x) / mrtPositioning.scale.x + mrtPositioning.origin.x;
    windowPos.y = (windowPos.y - mrtPositioning.position.y) / mrtPositioning.scale.y + mrtPositioning.origin.y;

    const sf::Vector2i windowPosI = {static_cast<int>(windowPos.x), static_cast<int>(windowPos.y)};

    return _mainRenderTexture->mapPixelToCoords(windowPosI, aView);
}

hg::math::Vector2f DefaultWindowManager::mapPixelToCoords(const hg::math::Vector2i& aPoint,
                                                          hg::PZInteger aViewIdx) const {
    HG_HARD_ASSERT(!_headless && "Method not available in Headless mode.");

    const auto mrtPositioning = _getMainRenderTexturePositioningData();

    auto windowPos = _window->mapPixelToCoords(aPoint, _window->getView());
    windowPos.x = (windowPos.x - mrtPositioning.position.x) / mrtPositioning.scale.x + mrtPositioning.origin.x;
    windowPos.y = (windowPos.y - mrtPositioning.position.y) / mrtPositioning.scale.y + mrtPositioning.origin.y;

    const sf::Vector2i windowPosI = {static_cast<int>(windowPos.x), static_cast<int>(windowPos.y)};

    return _mainRenderTexture->mapPixelToCoords(windowPosI, aViewIdx);
}

hg::math::Vector2i DefaultWindowManager::mapCoordsToPixel(const hg::math::Vector2f& aPoint,
                                                          const hg::gr::View& aView) const {
    HG_HARD_ASSERT(!_headless && "Method not available in Headless mode.");

    const auto mrtPositioning = _getMainRenderTexturePositioningData();

    auto mrtPos = _mainRenderTexture->mapCoordsToPixel(aPoint, aView);
    const auto xx = mrtPositioning.position.x + (mrtPos.x - mrtPositioning.origin.x) * mrtPositioning.scale.x;
    const auto yy = mrtPositioning.position.y + (mrtPos.y - mrtPositioning.origin.y) * mrtPositioning.scale.y;

    return _window->mapCoordsToPixel({xx, yy}, _window->getView());
}

hg::math::Vector2i DefaultWindowManager::mapCoordsToPixel(const hg::math::Vector2f& aPoint,
                                                          hg::PZInteger aViewIdx) const {
    HG_HARD_ASSERT(!_headless && "Method not available in Headless mode.");

    const auto mrtPositioning = _getMainRenderTexturePositioningData();

    auto mrtPos = _mainRenderTexture->mapCoordsToPixel(aPoint, aViewIdx);
    const auto xx = mrtPositioning.position.x + (mrtPos.x - mrtPositioning.origin.x) * mrtPositioning.scale.x;
    const auto yy = mrtPositioning.position.y + (mrtPos.y - mrtPositioning.origin.y) * mrtPositioning.scale.y;

    return {(int)xx, (int)yy};
    //return _window->mapCoordsToPixel({xx, yy}, _window->getView());
}

///////////////////////////////////////////////////////////////////////////
// GUI                                                                   //
///////////////////////////////////////////////////////////////////////////

Rml::Context& DefaultWindowManager::getGUIContext() {
    HG_HARD_ASSERT(!_headless && "Method not available in Headless mode.");
    return *(*_rmlUiContextDriver);
}

///////////////////////////////////////////////////////////////////////////
// KEYBOARD & MOUSE INPUT                                                //
///////////////////////////////////////////////////////////////////////////

WindowFrameInputView DefaultWindowManager::getInput() const {
    return _inputTracker.getInputView();
}

///////////////////////////////////////////////////////////////////////////
// PRIVATE METHODS                                                       //
///////////////////////////////////////////////////////////////////////////

void DefaultWindowManager::_eventPreDraw() {
    if (!_headless) {
        _mainRenderTexture->clear(hg::gr::COLOR_DARK_GRAY); // TODO Parametrize colour
    }
}

void DefaultWindowManager::_eventDraw2() {
    if (!_headless) {
        _window->clear(hg::gr::COLOR_BLACK); // TODO Parametrize colour
        _drawMainRenderTexture();
    }
}

void DefaultWindowManager::_eventDisplay() {
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

    const auto mrtSize = _mainRenderTexture->getSize();
    const auto winSize = _window->getSize();

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
    hg::gr::Sprite mrtSprite{&_mainRenderTexture->getTexture()};

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

    _inputTracker.prepForEvents();

    hg::win::Event ev;
    while (_window->pollEvent(ev)) {
        const auto rmlUiDidConsumeEvent = _rmlUiContextDriver->processEvent(ev);
        if (rmlUiDidConsumeEvent) {
            continue;
        }

        _inputTracker.eventOccurred(ev);

        ev.visit(
            [this](const hg::win::Event::Closed& aEventData) {
                // TODO
            },
            [this](const hg::win::Event::Resized& aEventData) {
                hg::math::Rectangle<float> visibleArea{
                    0.f,
                    0.f,
                    static_cast<float>(aEventData.width),
                    static_cast<float>(aEventData.height)
                };
                _window->setView(hg::gr::View(visibleArea));
            }
        );
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
    const auto pixelPos = hg::win::GetMousePositionRelativeToWindow(*_window);

    auto windowPos = _window->mapPixelToCoords(pixelPos, _window->getView());
    windowPos.x = (windowPos.x - mrtPositioning.position.x) / mrtPositioning.scale.x + mrtPositioning.origin.x;
    windowPos.y = (windowPos.y - mrtPositioning.position.y) / mrtPositioning.scale.y + mrtPositioning.origin.y;

    const sf::Vector2i windowPosI = {static_cast<int>(windowPos.x), static_cast<int>(windowPos.y)};

    return _mainRenderTexture->mapPixelToCoords(windowPosI, aViewIndex);
}

sf::Vector2i DefaultWindowManager::_getWindowRelativeMousePos() const {
    if (_headless) {
        return {0, 0};
    }

    return hg::win::GetMousePositionRelativeToWindow(*_window);
}

} // namespace spempe
} // namespace jbatnozic

// clang-format on
