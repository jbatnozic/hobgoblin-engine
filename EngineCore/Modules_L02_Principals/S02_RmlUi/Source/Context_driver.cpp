// Copyright 2024 Jovan Batnozic. Released under MS-PL licence in Serbia.
// See https://github.com/jbatnozic/Hobgoblin?tab=readme-ov-file#licence

// clang-format off


#include <Hobgoblin/RmlUi/Context_driver.hpp>

#include <Hobgoblin/RmlUi/Hobgoblin_backend.hpp>
#include <Hobgoblin/RmlUi/Private/RmlUi_Hobgoblin_renderer.hpp>

#include <RmlUi/Core.h>

#include <cassert>
#include <stdexcept>
#include <utility>

#include <Hobgoblin/Private/Pmacro_define.hpp>
HOBGOBLIN_NAMESPACE_BEGIN
namespace rml {

ContextDriver::ContextDriver(const std::string& aContextName,
                             gr::RenderTarget& aRenderTarget)
    : _renderTarget{&aRenderTarget}
{
    _context = Rml::CreateContext(aContextName,
                                  Rml::Vector2i{
                                      _renderTarget->getSize().x,
                                      _renderTarget->getSize().y
                                  },
                                  nullptr);
    if (!_context) {
        std::runtime_error{"ContextDriver - Could not create context " + aContextName};
    }
}

ContextDriver::~ContextDriver() {
    const auto success = Rml::RemoveContext(_context->GetName());
    assert(success && "Removing context failed!");
}

Rml::Context& ContextDriver::operator*() {
    return *_context;
}

const Rml::Context& ContextDriver::operator*() const {
    return *_context;
}

Rml::Context* ContextDriver:: operator->() {
    return _context;
}

const Rml::Context* ContextDriver::operator->() const {
    return _context;
}

void ContextDriver::render() {
    auto* renderer = HobgoblinBackend::getRenderer();
    assert(renderer);

    // TODO Temp.
    _context->SetDimensions({_renderTarget->getSize().x, _renderTarget->getSize().y});

    renderer->setRenderTarget(_renderTarget.get());
    _context->Render();
    renderer->setRenderTarget(nullptr);
}

bool ContextDriver::processEvent(const win::Event& aEvent) {
    const auto modifiers = HobgoblinBackend::getKeyModifiers();

    bool eventConsumed = false;

    aEvent.visit(
        [&](const win::Event::KeyPressed& aEventData) {
            eventConsumed = !_context->ProcessKeyDown(HobgoblinBackend::translateKey(aEventData.virtualKey), modifiers);
        },
        [&](const win::Event::KeyReleased& aEventData) {
            eventConsumed = !_context->ProcessKeyUp(HobgoblinBackend::translateKey(aEventData.virtualKey), modifiers);
        },
        [&](const win::Event::MouseButtonPressed& aEventData) {
            eventConsumed = !_context->ProcessMouseButtonDown(HobgoblinBackend::translateButton(aEventData.button), modifiers);
        },
        [&](const win::Event::MouseButtonReleased& aEventData) {
            eventConsumed = !_context->ProcessMouseButtonUp(HobgoblinBackend::translateButton(aEventData.button), modifiers);
        },
        [&](const win::Event::MouseMoved& aEventData) {
            eventConsumed = !_context->ProcessMouseMove(aEventData.x, aEventData.y, modifiers);
        },
        // TODO(mouse wheel)
        [&](const win::Event::TextEntered& aEventData) {
            // note: 0..31 are control characters
            if (aEventData.unicode > 31) {
                eventConsumed = !_context->ProcessTextInput(Rml::Character(aEventData.unicode));
                return;
            }
            eventConsumed = false;
        }
    );

#if 0
    switch (aEvent.type) {
    case sf::Event::MouseWheelMoved:
        return !_context->ProcessMouseWheel(static_cast<float>(-aEvent.mouseWheel.delta),
                                            modifiers);
    }
#endif

    return eventConsumed;
}

void ContextDriver::update() {
    _context->Update();
}

void ContextDriver::setRenderTarget(gr::RenderTarget& aRenderTarget) {
    _renderTarget = &aRenderTarget;
}

} // namespace rml
HOBGOBLIN_NAMESPACE_END
#include <Hobgoblin/Private/Pmacro_undef.hpp>

// clang-format on
