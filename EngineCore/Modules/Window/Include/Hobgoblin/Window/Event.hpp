#ifndef UHOBGOBLIN_WINDOW_EVENT_HPP
#define UHOBGOBLIN_WINDOW_EVENT_HPP

#include <Hobgoblin/Common.hpp>
#include <Hobgoblin/Utility/Visitor.hpp>

#include <cstdint>
#include <string>
#include <utility>
#include <variant>

#include <Hobgoblin/Private/Pmacro_define.hpp>

HOBGOBLIN_NAMESPACE_BEGIN
namespace win {

struct Event {

    //! TODO
    struct Closed {};

    //! TODO
    struct GainedFocus {};

    //! TODO
    struct LostFocus {};

    /*
        JoystickButtonPressed,  //!< A joystick button was pressed (data in event.joystickButton)
        JoystickButtonReleased, //!< A joystick button was released (data in event.joystickButton)
        JoystickConnected,      //!< A joystick was connected (data in event.joystickConnect)
        JoystickDisconnected,   //!< A joystick was disconnected (data in event.joystickConnect)
        JoystickMoved,          //!< The joystick moved along an axis (data in event.joystickMove)
    */

    //! TODO
    struct KeyPressed {
        Keyboard::Key code;          //!< Code of the key that has been pressed
        Keyboard::Scancode scancode; //!< Physical code of the key that has been pressed
        bool alt;                    //!< Is the Alt key pressed?
        bool control;                //!< Is the Control key pressed?
        bool shift;                  //!< Is the Shift key pressed?
        bool system;                 //!< Is the System key pressed?
    };

    //! TODO
    struct KeyReleased {
        Keyboard::Key code;          //!< Code of the key that has been pressed
        Keyboard::Scancode scancode; //!< Physical code of the key that has been pressed
        bool alt;                    //!< Is the Alt key pressed?
        bool control;                //!< Is the Control key pressed?
        bool shift;                  //!< Is the Shift key pressed?
        bool system;                 //!< Is the System key pressed?
    };

    //! TODO
    struct MouseButtonPressed {
        // TODO
    };

    //! TODO
    struct MouseButtonReleased {
        // TODO
    };

    //! TODO
    struct MouseEntered {};

    //! TODO
    struct MouseLeft {};

    //! TODO
    struct MouseMoved {
        // TODO
    };

    //! TODO
    struct MouseWheelScrolled {
        // TODO
    };

    //! TODO
    struct Resized {
        PZInteger width;  //!< New width, in pixels
        PZInteger height; //!< New height, in pixels
    };

#if 0
    //! TODO
    struct SensorChanged {
        Sensor::Type type; //!< Type of the sensor
        float x;           //!< Current value of the sensor on X axis
        float y;           //!< Current value of the sensor on Y axis
        float z;           //!< Current value of the sensor on Z axis
    };
#endif

    //! TODO
    struct TextEntered {
        std::uint32_t unicode; //!< UTF-32 Unicode value of the character
    };

    struct TouchBegan {
        // TODO
    };

    struct TouchEnded {

    };

    struct TouchMoved {

    };

    ////////////////////////////////////////////////////////////
    /// \brief Mouse move event parameters (MouseMoved)
    ///
    ////////////////////////////////////////////////////////////
    struct MouseMoveEvent
    {
        int x; //!< X position of the mouse pointer, relative to the left of the owner window
        int y; //!< Y position of the mouse pointer, relative to the top of the owner window
    };

    ////////////////////////////////////////////////////////////
    /// \brief Mouse buttons events parameters
    ///        (MouseButtonPressed, MouseButtonReleased)
    ///
    ////////////////////////////////////////////////////////////
    struct MouseButtonEvent
    {
        Mouse::Button button; //!< Code of the button that has been pressed
        int           x;      //!< X position of the mouse pointer, relative to the left of the owner window
        int           y;      //!< Y position of the mouse pointer, relative to the top of the owner window
    };

    ////////////////////////////////////////////////////////////
    /// \brief Mouse wheel events parameters (MouseWheelMoved)
    ///
    /// \deprecated This event is deprecated and potentially inaccurate.
    ///             Use MouseWheelScrollEvent instead.
    ///
    ////////////////////////////////////////////////////////////
    struct MouseWheelEvent
    {
        int delta; //!< Number of ticks the wheel has moved (positive is up, negative is down)
        int x;     //!< X position of the mouse pointer, relative to the left of the owner window
        int y;     //!< Y position of the mouse pointer, relative to the top of the owner window
    };

    ////////////////////////////////////////////////////////////
    /// \brief Mouse wheel events parameters (MouseWheelScrolled)
    ///
    ////////////////////////////////////////////////////////////
    struct MouseWheelScrollEvent
    {
        Mouse::Wheel wheel; //!< Which wheel (for mice with multiple ones)
        float        delta; //!< Wheel offset (positive is up/left, negative is down/right). High-precision mice may use non-integral offsets.
        int          x;     //!< X position of the mouse pointer, relative to the left of the owner window
        int          y;     //!< Y position of the mouse pointer, relative to the top of the owner window
    };

    ////////////////////////////////////////////////////////////
    /// \brief Joystick connection events parameters
    ///        (JoystickConnected, JoystickDisconnected)
    ///
    ////////////////////////////////////////////////////////////
    struct JoystickConnectEvent
    {
        unsigned int joystickId; //!< Index of the joystick (in range [0 .. Joystick::Count - 1])
    };

    ////////////////////////////////////////////////////////////
    /// \brief Joystick axis move event parameters (JoystickMoved)
    ///
    ////////////////////////////////////////////////////////////
    struct JoystickMoveEvent
    {
        unsigned int   joystickId; //!< Index of the joystick (in range [0 .. Joystick::Count - 1])
        Joystick::Axis axis;       //!< Axis on which the joystick moved
        float          position;   //!< New position on the axis (in range [-100 .. 100])
    };

    ////////////////////////////////////////////////////////////
    /// \brief Joystick buttons events parameters
    ///        (JoystickButtonPressed, JoystickButtonReleased)
    ///
    ////////////////////////////////////////////////////////////
    struct JoystickButtonEvent
    {
        unsigned int joystickId; //!< Index of the joystick (in range [0 .. Joystick::Count - 1])
        unsigned int button;     //!< Index of the button that has been pressed (in range [0 .. Joystick::ButtonCount - 1])
    };

    //! \brief Touch events parameters (TouchBegan, TouchMoved, TouchEnded).
    struct TouchEvent {
        PZInteger finger; //!< Index of the finger in case of multi-touch events
        int x;            //!< X position of the touch, relative to the left of the owner window
        int y;            //!< Y position of the touch, relative to the top of the owner window
    };


    using EventVariant = std::variant<
        BadPassphrase,
        ConnectAttemptFailed,
        Connected,
        Disconnected>;

    RN_Event() = default;

    template <class T>
    RN_Event(T&& arg)
        : eventVariant{std::forward<T>(arg)}
    {
    }

    std::optional<EventVariant> eventVariant;

    template <class ...Callables>
    void visit(Callables&&... callables) {
        std::visit(
            util::MakeVisitor([](const auto&) {}, std::forward<Callables>(callables)...),
            eventVariant.value()
        );
    }

    template <class ...Callables>
    void visit(Callables&&... callables) const {
        std::visit(
            util::MakeVisitor([](const auto&) {}, std::forward<Callables>(callables)...),
            eventVariant.value()
        );
    }

    //! Unlike visit, a call to strictVisit will not compile unless a
    //! matching callable is provided for each event type.
    template <class ...Callables>
    void strictVisit(Callables&&... callables) {
        std::visit(util::MakeVisitor(std::forward<Callables>(callables)...), eventVariant.value());
    }

    //! Unlike visit, a call to strictVisit will not compile unless a
    //! matching callable is provided for each event type.
    template <class ...Callables>
    void strictVisit(Callables&&... callables) const {
        std::visit(util::MakeVisitor(std::forward<Callables>(callables)...), eventVariant.value());
    }
};

} // namespace win
HOBGOBLIN_NAMESPACE_END

#include <Hobgoblin/Private/Pmacro_undef.hpp>
#include <Hobgoblin/Private/Short_namespace.hpp>

#endif // !UHOBGOBLIN_WINDOW_EVENT_HPP
