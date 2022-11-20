
#include <Hobgoblin/RmlUi/Context_driver.hpp>

#include <Hobgoblin/RmlUi/Hobgoblin_backend.hpp>
#include <Hobgoblin/RmlUi/Private/RmlUi_SFML_renderer.hpp>

#include <RmlUi/Core.h>

#include <cassert>
#include <stdexcept>
#include <utility>

#include <Hobgoblin/Private/Pmacro_define.hpp>
HOBGOBLIN_NAMESPACE_BEGIN
namespace rml {

ContextDriver::ContextDriver(const std::string& aContextName,
                             std::unique_ptr<SizedRenderTarget> aRenderTarget)
    : _renderTarget{std::move(aRenderTarget)}
{
    assert(_renderTarget && "Render target must not be null!");

    _context = Rml::CreateContext(aContextName,
                                  Rml::Vector2i{
                                      static_cast<int>(_renderTarget->getSize().x),
                                      static_cast<int>(_renderTarget->getSize().y)
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
    _context->SetDimensions({
        static_cast<int>(_renderTarget->getSize().x),
        static_cast<int>(_renderTarget->getSize().y)
                            });

    renderer->setRenderTarget(_renderTarget.get());
    _context->Render();
    renderer->setRenderTarget(nullptr);
}

bool ContextDriver::processEvent(sf::Event aEvent) {
    const auto modifiers = HobgoblinBackend::getKeyModifiers();

    switch (aEvent.type) {
    case sf::Event::MouseMoved:
        return !_context->ProcessMouseMove(aEvent.mouseMove.x,
                                           aEvent.mouseMove.y,
                                           modifiers);

    case sf::Event::MouseButtonPressed:
        return !_context->ProcessMouseButtonDown(aEvent.mouseButton.button,
                                                 modifiers);

    case sf::Event::MouseButtonReleased:
        return !_context->ProcessMouseButtonUp(aEvent.mouseButton.button,
                                               modifiers);

    case sf::Event::MouseWheelMoved:
        return !_context->ProcessMouseWheel(static_cast<float>(-aEvent.mouseWheel.delta),
                                            modifiers);

    case sf::Event::TextEntered:
        // note: 0..31 are control characters
        if (aEvent.text.unicode > 31) {
            return !_context->ProcessTextInput(Rml::Character(aEvent.text.unicode));
        }
        return false;

    case sf::Event::KeyPressed:
        return !_context->ProcessKeyDown(HobgoblinBackend::translateKey(aEvent.key.code),
                                         modifiers);

    case sf::Event::KeyReleased:
        return !_context->ProcessKeyUp(HobgoblinBackend::translateKey(aEvent.key.code),
                                       modifiers);

    default:
        return false;
    }
}

void ContextDriver::update() {
    _context->Update();
}

void ContextDriver::setRenderTarget(std::unique_ptr<SizedRenderTarget> aRenderTarget) {
    _renderTarget = std::move(aRenderTarget);
}

} // namespace rml
HOBGOBLIN_NAMESPACE_END
#include <Hobgoblin/Private/Pmacro_undef.hpp>
