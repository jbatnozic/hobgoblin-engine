#pragma once

#include <Hobgoblin/Input/Private/Conversions.hpp>
#include <Hobgoblin/Window/Event.hpp>

#include <SFML/Window/Event.hpp>

#include <Hobgoblin/Private/Pmacro_define.hpp>

HOBGOBLIN_NAMESPACE_BEGIN
namespace win {

inline
Event ToHg(const sf::Event& aEvent) {
    switch (aEvent.type) {
        case sf::Event::Closed:
            return {Event::Closed{}};

        case sf::Event::Resized:
        {
            Event::Resized data;
            data.width  = static_cast<PZInteger>(aEvent.size.width);
            data.height = static_cast<PZInteger>(aEvent.size.height);
            return {data};
        }

        case sf::Event::LostFocus:
            return {Event::LostFocus{}};

        case sf::Event::GainedFocus:
            return {Event::GainedFocus{}};

        case sf::Event::TextEntered:
            return {Event::TextEntered{aEvent.text.unicode}};

        case sf::Event::KeyPressed:
        {
            Event::KeyPressed data;
            data.virtualKey = in::VirtualKeyboardKey{
                static_cast<in::UniversalInputEnum>(in::detail::SfKeyboardKeyToInputEnum(aEvent.key.code))
            };
            data.physicalKey = in::PhysicalKeyboardKey{
                static_cast<in::UniversalInputEnum>(in::detail::SfKeyboardScancodeToInputEnum(aEvent.key.scancode))
            };
            data.alt     = aEvent.key.alt;
            data.control = aEvent.key.control;
            data.shift   = aEvent.key.shift;
            data.system  = aEvent.key.system;
            return {data};
        }

        case sf::Event::KeyReleased:
        {
            Event::KeyReleased data;
            data.virtualKey = in::VirtualKeyboardKey{
                static_cast<in::UniversalInputEnum>(in::detail::SfKeyboardKeyToInputEnum(aEvent.key.code))
            };
            data.physicalKey = in::PhysicalKeyboardKey{
                static_cast<in::UniversalInputEnum>(in::detail::SfKeyboardScancodeToInputEnum(aEvent.key.scancode))
            };
            data.alt     = aEvent.key.alt;
            data.control = aEvent.key.control;
            data.shift   = aEvent.key.shift;
            data.system  = aEvent.key.system;
            return {data};
        }

        case sf::Event::MouseWheelMoved:
            // Deprecated
            break;

#if 0
        case sf::Event::MouseWheelScrolled:
#endif


        case sf::Event::MouseButtonPressed:
        {
            Event::MouseButtonPressed data;
            data.button = in::MouseButton{
                static_cast<in::UniversalInputEnum>(in::detail::SfMouseButtonToInputEnum(aEvent.mouseButton.button))
            };
            data.x = aEvent.mouseButton.x;
            data.y = aEvent.mouseButton.y;
            return {data};
        }

        case sf::Event::MouseButtonReleased:
        {
            Event::MouseButtonReleased data;
            data.button = in::MouseButton{
                static_cast<in::UniversalInputEnum>(in::detail::SfMouseButtonToInputEnum(aEvent.mouseButton.button))
            };
            data.x = aEvent.mouseButton.x;
            data.y = aEvent.mouseButton.y;
            return {data};
        }

        case sf::Event::MouseMoved:
        {
            Event::MouseMoved data;
            data.x = aEvent.mouseMove.x;
            data.y = aEvent.mouseMove.y;
            return {data};
        }

        case sf::Event::MouseEntered:
            return {Event::MouseEntered{}};

        case sf::Event::MouseLeft:
            return {Event::MouseLeft{}};

#if 0
        case sf::Event::JoystickButtonPressed:
        case sf::Event::JoystickButtonReleased:
        case sf::Event::JoystickMoved:
        case sf::Event::JoystickConnected:
        case sf::Event::JoystickDisconnected:

        case sf::Event::TouchBegan:
        case sf::Event::TouchMoved:
        case sf::Event::TouchEnded:

        case sf::Event::SensorChanged:
#endif

        default: NO_OP();
    }
}

} // namespace win
HOBGOBLIN_NAMESPACE_END

#include <Hobgoblin/Private/Pmacro_undef.hpp>
