#ifndef UHOBGOBLIN_RMLUI_PRIVATE_RMLUI_CONTEXT_DRIVER_HPP
#define UHOBGOBLIN_RMLUI_PRIVATE_RMLUI_CONTEXT_DRIVER_HPP

#include <RmlUi/Core.h>

#include <SFML/Graphics.hpp>

#include <memory>
#include <string>

#include <Hobgoblin/Private/Pmacro_define.hpp>
HOBGOBLIN_NAMESPACE_BEGIN
namespace rml {

//! TODO(description)
class SizedRenderTarget {
public:
    virtual ~SizedRenderTarget() = default;

    virtual sf::RenderTarget& operator*() = 0;

    virtual sf::RenderTarget* operator->() = 0;

    virtual sf::Vector2u getSize() const = 0;
};

//! TODO(description)
template <class taRenderTarget>
class ConcreteSizedRenderTarget : public SizedRenderTarget {
public:
    explicit ConcreteSizedRenderTarget(taRenderTarget& aRenderTarget)
        : _renderTarget{aRenderTarget} {}

    sf::RenderTarget& operator*() override {
        return _renderTarget;
    }

    sf::RenderTarget* operator->() override {
        return std::addressof(_renderTarget);
    }

    sf::Vector2u getSize() const override {
        return _renderTarget.getSize();
    }

private:
    taRenderTarget& _renderTarget;
};

//! TODO(add description)
class ContextDriver {
public:
    ContextDriver(const std::string& aContextName,
                  std::unique_ptr<SizedRenderTarget> aRenderTarget);

    template <class taRenderTarget>
    ContextDriver(const std::string& aContextName,
                  taRenderTarget& aRenderTarget);

    ~ContextDriver();

    Rml::Context& operator*();

    const Rml::Context& operator*() const;

    Rml::Context* operator->();

    const Rml::Context* operator->() const;

    void render();

    //! Returns: Whether the event was consumed by an element (true) in the context or not (false).
    bool processEvent(sf::Event aEvent);

    void update();

    //! Sets a new render target for this context driver.
    //! It's recommended to use the templated verion below.
    void setRenderTarget(std::unique_ptr<SizedRenderTarget> aRenderTarget);

    template <class taRenderTarget>
    void setRenderTarget(taRenderTarget& aRenderTarget);

private:
    std::unique_ptr<SizedRenderTarget> _renderTarget;
    Rml::Context* _context = nullptr;
};

template <class taRenderTarget>
ContextDriver::ContextDriver(const std::string& aContextName,
                             taRenderTarget& aRenderTarget)
    : ContextDriver{
        aContextName,
        std::make_unique<ConcreteSizedRenderTarget<taRenderTarget>>(aRenderTarget)
}
{
}

template <class taRenderTarget>
void ContextDriver::setRenderTarget(taRenderTarget& aRenderTarget) {
    setRenderTarget(
        std::make_unique<ConcreteSizedRenderTarget<taRenderTarget>>(aRenderTarget)
    );
}

} // namespace rml
HOBGOBLIN_NAMESPACE_END
#include <Hobgoblin/Private/Pmacro_undef.hpp>
#include <Hobgoblin/Private/Short_namespace.hpp>

#endif // !UHOBGOBLIN_RMLUI_PRIVATE_RMLUI_CONTEXT_DRIVER_HPP
