#ifndef UHOBGOBLIN_WINDOW_EVENT_HPP
#define UHOBGOBLIN_WINDOW_EVENT_HPP

#include <Hobgoblin/Common.hpp>
#include <Hobgoblin/Input.hpp>
#include <Hobgoblin/Utility/Visitor.hpp>

#include <cstdint>
#include <string>
#include <utility>
#include <variant>

#include <Hobgoblin/Private/Pmacro_define.hpp>

HOBGOBLIN_NAMESPACE_BEGIN
namespace win {

struct Event {
    //! An unknown/unhandled event.
    using Unknown = std::monostate;

    //! The window was closed.
    struct Closed {};

    //! The window gained focus.
    struct GainedFocus {};

    //! The window lost focus.
    struct LostFocus {};

#if 0
    // Note: Maybe call these classes Controller-something?

    //! A joystick button was pressed.
    struct JoystickButtonPressed {
        // TODO
    };

    //! A joystick button was released.
    struct JoystickButtonReleased {
        // TODO
    };

    //! A joystick was connected.
    struct JoystickConnected {
        // TODO
    };

    //! A joystick was disconnected.
    struct JoystickDisconnected {
        // TODO
    };

    //! A joystick moved along an axis.
    struct JoystickMoved {
        // TODO
    };
#endif

    //! A key was pressed on the keyboard.
    struct KeyPressed {
        in::VirtualKeyboardKey  virtualKey;  //!< Virtual code of the key that was pressed
        in::PhysicalKeyboardKey physicalKey; //!< Physical code of the key that was pressed
        bool alt;                            //!< Is the Alt key pressed?
        bool control;                        //!< Is the Control key pressed?
        bool shift;                          //!< Is the Shift key pressed?
        bool system;                         //!< Is the System key pressed?
    };

    //! A key was released on the keyboard.
    struct KeyReleased {
        in::VirtualKeyboardKey  virtualKey;  //!< Virtual code of the key that was released
        in::PhysicalKeyboardKey physicalKey; //!< Physical code of the key that was released
        bool alt;                            //!< Is the Alt key pressed?
        bool control;                        //!< Is the Control key pressed?
        bool shift;                          //!< Is the Shift key pressed?
        bool system;                         //!< Is the System key pressed?
    };

    //! A mouse button was pressed.
    struct MouseButtonPressed {
        in::MouseButton button; //!< Code of the button that has been pressed
        int x;                  //!< X position of the mouse pointer, relative to the left of the owner window
        int y;                  //!< Y position of the mouse pointer, relative to the top of the owner window
    };

    //! A mouse button was released.
    struct MouseButtonReleased {
        in::MouseButton button; //!< Code of the button that has been pressed
        int x;                  //!< X position of the mouse pointer, relative to the left of the owner window
        int y;                  //!< Y position of the mouse pointer, relative to the top of the owner window
    };

    //! The mouse cursor entered the area of the window.
    struct MouseEntered {};

    //! The mouse cursor left the area of the window.
    struct MouseLeft {};

    //! The mouse cursor moved.
    struct MouseMoved {
        int x; //!< X position of the mouse pointer, relative to the left of the owner window
        int y; //!< Y position of the mouse pointer, relative to the top of the owner window
    };

    //! The mouse wheel was scrolled.
    struct MouseWheelScrolled {
        in::MouseWheel wheel; //!< Which wheel (for mice with multiple ones).
        float delta;          //!< Wheel offset (positive is up/left, negative is down/right). High-precision mice may use non-integral offsets.
        int x;                //!< X position of the mouse pointer, relative to the left of the owner window.
        int y;                //!< Y position of the mouse pointer, relative to the top of the owner window.
    };

    //! The window was resized.
    struct Resized {
        PZInteger width;  //!< New width, in pixels
        PZInteger height; //!< New height, in pixels
    };

#if 0
    //! A sensor value changed.
    struct SensorChanged {
        // TODO
    };
#endif

    //! A character was entered.
    struct TextEntered {
        std::uint32_t unicode; //!< UTF-32 Unicode value of the character
    };

#if 0
    //! A touch event began.
    struct TouchBegan {
        // TODO
    };

    //! A touch event ended.
    struct TouchEnded {
        // TODO
    };

    //! A touch moved.
    struct TouchMoved {
        // TODO
    };
#endif

    using EventVariant = std::variant<
        Unknown,
        Closed,
        GainedFocus,
        LostFocus,
        KeyPressed,
        KeyReleased,
        MouseButtonPressed,
        MouseButtonReleased,
        MouseEntered,
        MouseLeft,
        MouseMoved,
        MouseWheelScrolled,
        Resized,
        TextEntered
    >;

    Event() = default;

    template <class T>
    Event(T&& arg)
        : eventVariant{std::forward<T>(arg)}
    {
    }

    std::optional<EventVariant> eventVariant;

    template <class ...taCallables>
    void visit(taCallables&&... aCallables) {
        std::visit(
            util::MakeVisitor([](const auto&) {}, std::forward<taCallables>(aCallables)...),
            eventVariant.value()
        );
    }

    template <class ...taCallables>
    void visit(taCallables&&... aCallables) const {
        std::visit(
            util::MakeVisitor([](const auto&) {}, std::forward<taCallables>(aCallables)...),
            eventVariant.value()
        );
    }

    //! Unlike visit, a call to strictVisit will not compile unless a
    //! matching callable is provided for each event type.
    template <class ...taCallables>
    void strictVisit(taCallables&&... aCallables) {
        std::visit(util::MakeVisitor(std::forward<taCallables>(aCallables)...), eventVariant.value());
    }

    //! Unlike visit, a call to strictVisit will not compile unless a
    //! matching callable is provided for each event type.
    template <class ...taCallables>
    void strictVisit(taCallables&&... aCallables) const {
        std::visit(util::MakeVisitor(std::forward<taCallables>(aCallables)...), eventVariant.value());
    }
};

} // namespace win
HOBGOBLIN_NAMESPACE_END

#include <Hobgoblin/Private/Pmacro_undef.hpp>
#include <Hobgoblin/Private/Short_namespace.hpp>

#endif // !UHOBGOBLIN_WINDOW_EVENT_HPP
