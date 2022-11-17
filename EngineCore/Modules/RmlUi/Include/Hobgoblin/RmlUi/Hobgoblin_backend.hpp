#ifndef UHOBGOBLIN_RMLUI_HOBGOBLIN_BACKEND_HPP
#define UHOBGOBLIN_RMLUI_HOBGOBLIN_BACKEND_HPP

#include <RmlUi/Core.h>
#include <SFML/Window.hpp>

#include <memory>

#include <Hobgoblin/Private/Pmacro_define.hpp>
HOBGOBLIN_NAMESPACE_BEGIN
namespace rml {

namespace detail {
class RmlUiSFMLSystem;
class RmlUiSFMLRenderer;
} // namespace detail

class ContextDriver;

//! Hobgoblin backend for the RmlUI library.
class HobgoblinBackend {
public:
    struct BackendLifecycleGuard {
        ~BackendLifecycleGuard();
    };

    static std::unique_ptr<BackendLifecycleGuard> initialize();

    //! Convenience function to convert from a SFML key to RmlUi key.
    static Rml::Input::KeyIdentifier translateKey(sf::Keyboard::Key aKey);

    //! Convenience function to get the current state of modifier keys (ctrl,
    //! alt, shift) in RmlUi format.
    static int getKeyModifiers();

private:
    friend class ContextDriver;

    static detail::RmlUiSFMLSystem* getSystem();
    static detail::RmlUiSFMLRenderer* getRenderer();
};

} // namespace rml
HOBGOBLIN_NAMESPACE_END
#include <Hobgoblin/Private/Pmacro_undef.hpp>
#include <Hobgoblin/Private/Short_namespace.hpp>

#endif // !UHOBGOBLIN_RMLUI_HOBGOBLIN_BACKEND_HPP
