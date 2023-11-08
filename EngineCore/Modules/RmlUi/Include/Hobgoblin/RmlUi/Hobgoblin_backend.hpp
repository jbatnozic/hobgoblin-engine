#ifndef UHOBGOBLIN_RMLUI_HOBGOBLIN_BACKEND_HPP
#define UHOBGOBLIN_RMLUI_HOBGOBLIN_BACKEND_HPP

#include <RmlUi/Core.h>
#include <Hobgoblin/Input/Virtual_keyboard_key.hpp>
#include <Hobgoblin/Window.hpp>

#include <memory>

#include <Hobgoblin/Private/Pmacro_define.hpp>
HOBGOBLIN_NAMESPACE_BEGIN
namespace rml {

namespace detail {
class RmlUiHobgoblinSystem;
class RmlUiHobgoblinRenderer;
} // namespace detail

class ContextDriver;

//! Hobgoblin backend for the RmlUI library.
class HobgoblinBackend {
public:
    struct BackendLifecycleGuard {
        ~BackendLifecycleGuard();
    };

    static std::unique_ptr<BackendLifecycleGuard> initialize();

    //! Convenience function to convert from a Hobgoblin.Input key to RmlUi key.
    static Rml::Input::KeyIdentifier translateKey(in::VirtualKeyboardKey aKey);

    //! Convenience function to convert from a Hobgoblin.Input mouse button to RmlUi button.
    static int translateButton(in::MouseButton aButton);

    //! Convenience function to get the current state of modifier keys (ctrl,
    //! alt, shift) in RmlUi format.
    static int getKeyModifiers();

private:
    friend class ContextDriver;

    static detail::RmlUiHobgoblinSystem* getSystem();
    static detail::RmlUiHobgoblinRenderer* getRenderer();
};

} // namespace rml
HOBGOBLIN_NAMESPACE_END
#include <Hobgoblin/Private/Pmacro_undef.hpp>
#include <Hobgoblin/Private/Short_namespace.hpp>

#endif // !UHOBGOBLIN_RMLUI_HOBGOBLIN_BACKEND_HPP
