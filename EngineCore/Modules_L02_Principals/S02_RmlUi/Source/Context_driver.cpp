// Copyright 2024 Jovan Batnozic. Released under MS-PL licence in Serbia.
// See https://github.com/jbatnozic/Hobgoblin?tab=readme-ov-file#licence

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

ContextDriver::ContextDriver(const std::string& aContextName, gr::Canvas& aCanvas)
    : _canvas{&aCanvas} {
    _context = Rml::CreateContext(aContextName,
                                  Rml::Vector2i{_canvas->getSize().x, _canvas->getSize().y},
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

Rml::Context* ContextDriver::operator->() {
    return _context;
}

const Rml::Context* ContextDriver::operator->() const {
    return _context;
}

void ContextDriver::render() {
    auto* renderer = HobgoblinBackend::getRenderer();
    assert(renderer);

    // TODO Temp.
    _context->SetDimensions({_canvas->getSize().x, _canvas->getSize().y});

    renderer->setCanvas(_canvas.get());
    _context->Render();
    renderer->setCanvas(nullptr);
}

bool ContextDriver::processEvent(const win::Event& aEvent) {
    const auto modifiers = HobgoblinBackend::getKeyModifiers();

    bool eventConsumed = false;

    aEvent.strictVisit(
        [](const win::Event::Unknown&) {},
        [](const win::Event::Closed&) {},
        [](const win::Event::GainedFocus&) {},
        [](const win::Event::LostFocus&) {},
        [&](const win::Event::KeyPressed& aEventData) {
            eventConsumed =
                !_context->ProcessKeyDown(HobgoblinBackend::translateKey(aEventData.virtualKey),
                                          modifiers);
        },
        [&](const win::Event::KeyReleased& aEventData) {
            eventConsumed =
                !_context->ProcessKeyUp(HobgoblinBackend::translateKey(aEventData.virtualKey),
                                        modifiers);
        },
        [&](const win::Event::MouseButtonPressed& aEventData) {
            eventConsumed =
                !_context->ProcessMouseButtonDown(HobgoblinBackend::translateButton(aEventData.button),
                                                  modifiers);
        },
        [&](const win::Event::MouseButtonReleased& aEventData) {
            eventConsumed =
                !_context->ProcessMouseButtonUp(HobgoblinBackend::translateButton(aEventData.button),
                                                modifiers);
        },
        [](const win::Event::MouseEntered&) {},
        [](const win::Event::MouseLeft&) {},
        [&](const win::Event::MouseMoved& aEventData) {
            eventConsumed = !_context->ProcessMouseMove(aEventData.x, aEventData.y, modifiers);
        },
        [&](const win::Event::MouseWheelScrolled& aEventData) {
            switch (aEventData.wheel) {
            case in::MW_HORIZONTAL:
                eventConsumed =
                    !_context->ProcessMouseWheel(Rml::Vector2f{-aEventData.delta, 0.f}, modifiers);
                break;

            case in::MW_VERTICAL:
                eventConsumed =
                    !_context->ProcessMouseWheel(Rml::Vector2f{0.f, -aEventData.delta}, modifiers);
                break;

            default:
                break;
            }
        },
        [](const win::Event::Resized&) {},
        [&](const win::Event::TextEntered& aEventData) {
            // note: 0..31 are control characters
            if (aEventData.unicode > 31) {
                eventConsumed = !_context->ProcessTextInput(Rml::Character(aEventData.unicode));
                return;
            }
            eventConsumed = false;
        });

    return eventConsumed;
}

void ContextDriver::update() {
    _context->Update();
}

void ContextDriver::setCanvas(gr::Canvas& aCanvas) {
    _canvas = &aCanvas;
}

} // namespace rml
HOBGOBLIN_NAMESPACE_END
#include <Hobgoblin/Private/Pmacro_undef.hpp>
