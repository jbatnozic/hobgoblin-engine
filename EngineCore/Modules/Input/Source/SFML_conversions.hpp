#pragma once

#include <Hobgoblin/Input/Universal_input_enumeration.hpp>

#include <SFML/Window/Keyboard.hpp>
#include <SFML/Window/Mouse.hpp>

#include <Hobgoblin/Private/Pmacro_define.hpp>

HOBGOBLIN_NAMESPACE_BEGIN
namespace in {

// SFML -> Hobgoblin

inline
UniversalInputEnum ToHgVirtualKeyboardKey(sf::Keyboard::Key aKey) {
    switch (aKey) {
    case sf::Keyboard::Unknown: return VK_UNKNOWN;
    case sf::Keyboard::A: return VK_A;
    case sf::Keyboard::B: return VK_B;
    case sf::Keyboard::C: return VK_C;
    case sf::Keyboard::D: return VK_D;
    case sf::Keyboard::E: return VK_E;
    case sf::Keyboard::F: return VK_F;
    case sf::Keyboard::G: return VK_G;
    case sf::Keyboard::H: return VK_H;
    case sf::Keyboard::I: return VK_I;
    case sf::Keyboard::J: return VK_J;
    case sf::Keyboard::K: return VK_K;
    case sf::Keyboard::L: return VK_L;
    case sf::Keyboard::M: return VK_M;
    case sf::Keyboard::N: return VK_N;
    case sf::Keyboard::O: return VK_O;
    case sf::Keyboard::P: return VK_P;
    case sf::Keyboard::Q: return VK_Q;
    case sf::Keyboard::R: return VK_R;
    case sf::Keyboard::S: return VK_S;
    case sf::Keyboard::T: return VK_T;
    case sf::Keyboard::U: return VK_U;
    case sf::Keyboard::V: return VK_V;
    case sf::Keyboard::W: return VK_W;
    case sf::Keyboard::X: return VK_X;
    case sf::Keyboard::Y: return VK_Y;
    case sf::Keyboard::Z: return VK_Z;
    case sf::Keyboard::Num0: return VK_NUM_0;
    case sf::Keyboard::Num1: return VK_NUM_1;
    case sf::Keyboard::Num2: return VK_NUM_2;
    case sf::Keyboard::Num3: return VK_NUM_3;
    case sf::Keyboard::Num4: return VK_NUM_4;
    case sf::Keyboard::Num5: return VK_NUM_5;
    case sf::Keyboard::Num6: return VK_NUM_6;
    case sf::Keyboard::Num7: return VK_NUM_7;
    case sf::Keyboard::Num8: return VK_NUM_8;
    case sf::Keyboard::Num9: return VK_NUM_9;
    case sf::Keyboard::Escape: return VK_ESCAPE;
    case sf::Keyboard::LControl: return VK_LCONTROL;
    case sf::Keyboard::LShift: return VK_LSHIFT;
    case sf::Keyboard::LAlt: return VK_LALT;
    case sf::Keyboard::LSystem: return VK_LSYSTEM;
    case sf::Keyboard::RControl: return VK_RCONTROL;
    case sf::Keyboard::RShift: return VK_RSHIFT;
    case sf::Keyboard::RAlt: return VK_RALT;
    case sf::Keyboard::RSystem: return VK_RSYSTEM;
    case sf::Keyboard::Menu: return VK_MENU;
    case sf::Keyboard::LBracket: return VK_LBRACKET;
    case sf::Keyboard::RBracket: return VK_RBRACKET;
    case sf::Keyboard::Semicolon: return VK_SEMICOLON;
    case sf::Keyboard::Comma: return VK_COMMA;
    case sf::Keyboard::Period: return VK_PERIOD;
    case sf::Keyboard::Apostrophe: return VK_APOSTROPHE;
    case sf::Keyboard::Slash: return VK_SLASH;
    case sf::Keyboard::Backslash: return VK_BACKSLASH;
    case sf::Keyboard::Grave: return VK_GRAVE;
    case sf::Keyboard::Equal: return VK_EQUAL;
    case sf::Keyboard::Hyphen: return VK_HYPHEN;
    case sf::Keyboard::Space: return VK_SPACE;
    case sf::Keyboard::Enter: return VK_ENTER;
    case sf::Keyboard::Backspace: return VK_BACKSPACE;
    case sf::Keyboard::Tab: return VK_TAB;
    case sf::Keyboard::PageUp: return VK_PAGE_UP;
    case sf::Keyboard::PageDown: return VK_PAGE_DOWN;
    case sf::Keyboard::End: return VK_END;
    case sf::Keyboard::Home: return VK_HOME;
    case sf::Keyboard::Insert: return VK_INSERT;
    case sf::Keyboard::Delete: return VK_DELETE;
    case sf::Keyboard::Add: return VK_ADD;
    case sf::Keyboard::Subtract: return VK_SUBTRACT;
    case sf::Keyboard::Multiply: return VK_MULTIPLY;
    case sf::Keyboard::Divide: return VK_DIVIDE;
    case sf::Keyboard::Left: return VK_LEFT;
    case sf::Keyboard::Right: return VK_RIGHT;
    case sf::Keyboard::Up: return VK_UP;
    case sf::Keyboard::Down: return VK_DOWN;
    case sf::Keyboard::Numpad0: return VK_NUMPAD_0;
    case sf::Keyboard::Numpad1: return VK_NUMPAD_1;
    case sf::Keyboard::Numpad2: return VK_NUMPAD_2;
    case sf::Keyboard::Numpad3: return VK_NUMPAD_3;
    case sf::Keyboard::Numpad4: return VK_NUMPAD_4;
    case sf::Keyboard::Numpad5: return VK_NUMPAD_5;
    case sf::Keyboard::Numpad6: return VK_NUMPAD_6;
    case sf::Keyboard::Numpad7: return VK_NUMPAD_7;
    case sf::Keyboard::Numpad8: return VK_NUMPAD_8;
    case sf::Keyboard::Numpad9: return VK_NUMPAD_9;
    case sf::Keyboard::F1: return VK_F1;
    case sf::Keyboard::F2: return VK_F2;
    case sf::Keyboard::F3: return VK_F3;
    case sf::Keyboard::F4: return VK_F4;
    case sf::Keyboard::F5: return VK_F5;
    case sf::Keyboard::F6: return VK_F6;
    case sf::Keyboard::F7: return VK_F7;
    case sf::Keyboard::F8: return VK_F8;
    case sf::Keyboard::F9: return VK_F9;
    case sf::Keyboard::F10: return VK_F10;
    case sf::Keyboard::F11: return VK_F11;
    case sf::Keyboard::F12: return VK_F12;
    case sf::Keyboard::F13: return VK_F13;
    case sf::Keyboard::F14: return VK_F14;
    case sf::Keyboard::F15: return VK_F15;
    case sf::Keyboard::Pause: return VK_PAUSE;
    default:
        return VK_UNKNOWN;
    }
}

inline
UniversalInputEnum ToHgVirtualKeyboardKey(sf::Keyboard::Scan::Scancode aScancode) {
    switch (aScancode) {
    case sf::Keyboard::Scan::Unknown: return PK_UNKNOWN;
        case sf::Keyboard::Scan::A: return PK_A;
        case sf::Keyboard::Scan::B: return PK_B;
        case sf::Keyboard::Scan::C: return PK_C;
        case sf::Keyboard::Scan::D: return PK_D;
        case sf::Keyboard::Scan::E: return PK_E;
        case sf::Keyboard::Scan::F: return PK_F;
        case sf::Keyboard::Scan::G: return PK_G;
        case sf::Keyboard::Scan::H: return PK_H;
        case sf::Keyboard::Scan::I: return PK_I;
        case sf::Keyboard::Scan::J: return PK_J;
        case sf::Keyboard::Scan::K: return PK_K;
        case sf::Keyboard::Scan::L: return PK_L;
        case sf::Keyboard::Scan::M: return PK_M;
        case sf::Keyboard::Scan::N: return PK_N;
        case sf::Keyboard::Scan::O: return PK_O;
        case sf::Keyboard::Scan::P: return PK_P;
        case sf::Keyboard::Scan::Q: return PK_Q;
        case sf::Keyboard::Scan::R: return PK_R;
        case sf::Keyboard::Scan::S: return PK_S;
        case sf::Keyboard::Scan::T: return PK_T;
        case sf::Keyboard::Scan::U: return PK_U;
        case sf::Keyboard::Scan::V: return PK_V;
        case sf::Keyboard::Scan::W: return PK_W;
        case sf::Keyboard::Scan::X: return PK_X;
        case sf::Keyboard::Scan::Y: return PK_Y;
        case sf::Keyboard::Scan::Z: return PK_Z;
        case sf::Keyboard::Scan::Num1: return PK_NUM_1;
        case sf::Keyboard::Scan::Num2: return PK_NUM_2;
        case sf::Keyboard::Scan::Num3: return PK_NUM_3;
        case sf::Keyboard::Scan::Num4: return PK_NUM_4;
        case sf::Keyboard::Scan::Num5: return PK_NUM_5;
        case sf::Keyboard::Scan::Num6: return PK_NUM_6;
        case sf::Keyboard::Scan::Num7: return PK_NUM_7;
        case sf::Keyboard::Scan::Num8: return PK_NUM_8;
        case sf::Keyboard::Scan::Num9: return PK_NUM_9;
        case sf::Keyboard::Scan::Num0: return PK_NUM_0;
        case sf::Keyboard::Scan::Enter: return PK_ENTER;
        case sf::Keyboard::Scan::Escape: return PK_ESCAPE;
        case sf::Keyboard::Scan::Backspace: return PK_BACKSPACE;
        case sf::Keyboard::Scan::Tab: return PK_TAB;
        case sf::Keyboard::Scan::Space: return PK_SPACE;
        case sf::Keyboard::Scan::Hyphen: return PK_HYPHEN;
        case sf::Keyboard::Scan::Equal: return PK_EQUAL;
        case sf::Keyboard::Scan::LBracket: return PK_LBRACKET;
        case sf::Keyboard::Scan::RBracket: return PK_RBRACKET;
        case sf::Keyboard::Scan::Backslash: return PK_BACKSLASH;
        case sf::Keyboard::Scan::Semicolon: return PK_SEMICOLON;
        case sf::Keyboard::Scan::Apostrophe: return PK_APOSTROPHE;
        case sf::Keyboard::Scan::Grave: return PK_GRAVE;
        case sf::Keyboard::Scan::Comma: return PK_COMMA;
        case sf::Keyboard::Scan::Period: return PK_PERIOD;
        case sf::Keyboard::Scan::Slash: return PK_SLASH;
        case sf::Keyboard::Scan::F1: return PK_F1;
        case sf::Keyboard::Scan::F2: return PK_F2;
        case sf::Keyboard::Scan::F3: return PK_F3;
        case sf::Keyboard::Scan::F4: return PK_F4;
        case sf::Keyboard::Scan::F5: return PK_F5;
        case sf::Keyboard::Scan::F6: return PK_F6;
        case sf::Keyboard::Scan::F7: return PK_F7;
        case sf::Keyboard::Scan::F8: return PK_F8;
        case sf::Keyboard::Scan::F9: return PK_F9;
        case sf::Keyboard::Scan::F10: return PK_F10;
        case sf::Keyboard::Scan::F11: return PK_F11;
        case sf::Keyboard::Scan::F12: return PK_F12;
        case sf::Keyboard::Scan::F13: return PK_F13;
        case sf::Keyboard::Scan::F14: return PK_F14;
        case sf::Keyboard::Scan::F15: return PK_F15;
        case sf::Keyboard::Scan::F16: return PK_F16;
        case sf::Keyboard::Scan::F17: return PK_F17;
        case sf::Keyboard::Scan::F18: return PK_F18;
        case sf::Keyboard::Scan::F19: return PK_F19;
        case sf::Keyboard::Scan::F20: return PK_F20;
        case sf::Keyboard::Scan::F21: return PK_F21;
        case sf::Keyboard::Scan::F22: return PK_F22;
        case sf::Keyboard::Scan::F23: return PK_F23;
        case sf::Keyboard::Scan::F24: return PK_F24;
        case sf::Keyboard::Scan::CapsLock: return PK_CAPS_LOCK;
        case sf::Keyboard::Scan::PrintScreen: return PK_PRINT_SCREEN;
        case sf::Keyboard::Scan::ScrollLock: return PK_SCROLL_LOCK;
        case sf::Keyboard::Scan::Pause: return PK_PAUSE;
        case sf::Keyboard::Scan::Insert: return PK_INSERT;
        case sf::Keyboard::Scan::Home: return PK_HOME;
        case sf::Keyboard::Scan::PageUp: return PK_PAGE_UP;
        case sf::Keyboard::Scan::Delete: return PK_DELETE;
        case sf::Keyboard::Scan::End: return PK_END;
        case sf::Keyboard::Scan::PageDown: return PK_PAGE_DOWN;
        case sf::Keyboard::Scan::Right: return PK_RIGHT;
        case sf::Keyboard::Scan::Left: return PK_LEFT;
        case sf::Keyboard::Scan::Down: return PK_DOWN;
        case sf::Keyboard::Scan::Up: return PK_UP;
        case sf::Keyboard::Scan::NumLock: return PK_NUMLOCK;
        case sf::Keyboard::Scan::NumpadDivide: return PK_NUMPAD_DIVIDE;
        case sf::Keyboard::Scan::NumpadMultiply: return PK_NUMPAD_MULTIPLY;
        case sf::Keyboard::Scan::NumpadMinus: return PK_NUMPAD_MINUS;
        case sf::Keyboard::Scan::NumpadPlus: return PK_NUMPAD_PLUS;
        case sf::Keyboard::Scan::NumpadEqual: return PK_NUMPAD_EQUAL;
        case sf::Keyboard::Scan::NumpadEnter: return PK_NUMPAD_ENTER;
        case sf::Keyboard::Scan::NumpadDecimal: return PK_NUMPAD_DECIMAL;
        case sf::Keyboard::Scan::Numpad1: return PK_NUMPAD_1;
        case sf::Keyboard::Scan::Numpad2: return PK_NUMPAD_2;
        case sf::Keyboard::Scan::Numpad3: return PK_NUMPAD_3;
        case sf::Keyboard::Scan::Numpad4: return PK_NUMPAD_4;
        case sf::Keyboard::Scan::Numpad5: return PK_NUMPAD_5;
        case sf::Keyboard::Scan::Numpad6: return PK_NUMPAD_6;
        case sf::Keyboard::Scan::Numpad7: return PK_NUMPAD_7;
        case sf::Keyboard::Scan::Numpad8: return PK_NUMPAD_8;
        case sf::Keyboard::Scan::Numpad9: return PK_NUMPAD_9;
        case sf::Keyboard::Scan::Numpad0: return PK_NUMPAD_0;
        case sf::Keyboard::Scan::NonUsBackslash: return PK_NON_US_BACKSLASH;
        case sf::Keyboard::Scan::Application: return PK_APPLICATION;
        case sf::Keyboard::Scan::Execute: return PK_EXECUTE;
        case sf::Keyboard::Scan::ModeChange: return PK_MODE_CHANGE;
        case sf::Keyboard::Scan::Help: return PK_HELP;
        case sf::Keyboard::Scan::Menu: return PK_MENU;
        case sf::Keyboard::Scan::Select: return PK_SELECT;
        case sf::Keyboard::Scan::Redo: return PK_REDO;
        case sf::Keyboard::Scan::Undo: return PK_UNDO;
        case sf::Keyboard::Scan::Cut: return PK_CUT;
        case sf::Keyboard::Scan::Copy: return PK_COPY;
        case sf::Keyboard::Scan::Paste: return PK_PASTE;
        case sf::Keyboard::Scan::VolumeMute: return PK_VOLUME_MUTE;
        case sf::Keyboard::Scan::VolumeUp: return PK_VOLUME_UP;
        case sf::Keyboard::Scan::VolumeDown: return PK_VOLUME_DOWN;
        case sf::Keyboard::Scan::MediaPlayPause: return PK_MEDIA_PLAY_PAUSE;
        case sf::Keyboard::Scan::MediaStop: return PK_MEDIA_STOP;
        case sf::Keyboard::Scan::MediaNextTrack: return PK_MEDIA_NEXT_TRACK;
        case sf::Keyboard::Scan::MediaPreviousTrack: return PK_MEDIA_PREVIOUS_TRACK;
        case sf::Keyboard::Scan::LControl: return PK_LCONTROL;
        case sf::Keyboard::Scan::LShift: return PK_LSHIFT;
        case sf::Keyboard::Scan::LAlt: return PK_LALT;
        case sf::Keyboard::Scan::LSystem: return PK_LSYSTEM;
        case sf::Keyboard::Scan::RControl: return PK_RCONTROL;
        case sf::Keyboard::Scan::RShift: return PK_RSHIFT;
        case sf::Keyboard::Scan::RAlt: return PK_RALT;
        case sf::Keyboard::Scan::RSystem: return PK_RSYSTEM;
        case sf::Keyboard::Scan::Back: return PK_BACK;
        case sf::Keyboard::Scan::Forward: return PK_FORWARD;
        case sf::Keyboard::Scan::Refresh: return PK_REFRESH;
        case sf::Keyboard::Scan::Stop: return PK_STOP;
        case sf::Keyboard::Scan::Search: return PK_SEARCH;
        case sf::Keyboard::Scan::Favorites: return PK_FAVORITES;
        case sf::Keyboard::Scan::HomePage: return PK_HOMEPAGE;
        case sf::Keyboard::Scan::LaunchApplication1: return PK_LAUNCH_APP_1;
        case sf::Keyboard::Scan::LaunchApplication2: return PK_LAUNCH_APP_2;
        case sf::Keyboard::Scan::LaunchMail: return PK_LAUNCH_MAIL;
        case sf::Keyboard::Scan::LaunchMediaSelect: return PK_LAUNCH_MEDIA_SELECT;
        default:
            return PK_UNKNOWN;
    }
}

inline
UniversalInputEnum ToHgMouseButton(sf::Mouse::Button aValue) {
    switch (aValue) {
    case sf::Mouse::Left: return MB_LEFT;
    case sf::Mouse::Right: return MB_RIGHT;
    case sf::Mouse::Middle: return MB_MIDDLE;
    case sf::Mouse::XButton1: return MB_EXTRA_1;
    case sf::Mouse::XButton2: return MB_EXTRA_2;
    default: return MB_UNKNOWN;
    }
}

inline
UniversalInputEnum ToHgMouseWheel(sf::Mouse::Wheel aValue) {
}

// Hobgoblin -> SFML

inline
sf::Keyboard::Key ToSfKeyboardKey(UniversalInputEnum aValue) {
    switch (aValue) {
        case VK_UNKNOWN: return sf::Keyboard::Unknown;
        case VK_A: return sf::Keyboard::A;
        case VK_B: return sf::Keyboard::B;
        case VK_C: return sf::Keyboard::C;
        case VK_D: return sf::Keyboard::D;
        case VK_E: return sf::Keyboard::E;
        case VK_F: return sf::Keyboard::F;
        case VK_G: return sf::Keyboard::G;
        case VK_H: return sf::Keyboard::H;
        case VK_I: return sf::Keyboard::I;
        case VK_J: return sf::Keyboard::J;
        case VK_K: return sf::Keyboard::K;
        case VK_L: return sf::Keyboard::L;
        case VK_M: return sf::Keyboard::M;
        case VK_N: return sf::Keyboard::N;
        case VK_O: return sf::Keyboard::O;
        case VK_P: return sf::Keyboard::P;
        case VK_Q: return sf::Keyboard::Q;
        case VK_R: return sf::Keyboard::R;
        case VK_S: return sf::Keyboard::S;
        case VK_T: return sf::Keyboard::T;
        case VK_U: return sf::Keyboard::U;
        case VK_V: return sf::Keyboard::V;
        case VK_W: return sf::Keyboard::W;
        case VK_X: return sf::Keyboard::X;
        case VK_Y: return sf::Keyboard::Y;
        case VK_Z: return sf::Keyboard::Z;
        case VK_NUM_0: return sf::Keyboard::Num0;
        case VK_NUM_1: return sf::Keyboard::Num1;
        case VK_NUM_2: return sf::Keyboard::Num2;
        case VK_NUM_3: return sf::Keyboard::Num3;
        case VK_NUM_4: return sf::Keyboard::Num4;
        case VK_NUM_5: return sf::Keyboard::Num5;
        case VK_NUM_6: return sf::Keyboard::Num6;
        case VK_NUM_7: return sf::Keyboard::Num7;
        case VK_NUM_8: return sf::Keyboard::Num8;
        case VK_NUM_9: return sf::Keyboard::Num9;
        case VK_ESCAPE: return sf::Keyboard::Escape;
        case VK_LCONTROL: return sf::Keyboard::LControl;
        case VK_LSHIFT: return sf::Keyboard::LShift;
        case VK_LALT: return sf::Keyboard::LAlt;
        case VK_LSYSTEM: return sf::Keyboard::LSystem;
        case VK_RCONTROL: return sf::Keyboard::RControl;
        case VK_RSHIFT: return sf::Keyboard::RShift;
        case VK_RALT: return sf::Keyboard::RAlt;
        case VK_RSYSTEM: return sf::Keyboard::RSystem;
        case VK_MENU: return sf::Keyboard::Menu;
        case VK_LBRACKET: return sf::Keyboard::LBracket;
        case VK_RBRACKET: return sf::Keyboard::RBracket;
        case VK_SEMICOLON: return sf::Keyboard::SemiColon;
        case VK_COMMA: return sf::Keyboard::Comma;
        case VK_PERIOD: return sf::Keyboard::Period;
        case VK_APOSTROPHE: return sf::Keyboard::Apostrophe;
        case VK_SLASH: return sf::Keyboard::Slash;
        case VK_BACKSLASH: return sf::Keyboard::Backslash;
        case VK_GRAVE: return sf::Keyboard::Grave;
        case VK_EQUAL: return sf::Keyboard::Equal;
        case VK_HYPHEN: return sf::Keyboard::Hyphen;
        case VK_SPACE: return sf::Keyboard::Space;
        case VK_ENTER: return sf::Keyboard::Enter;
        case VK_BACKSPACE: return sf::Keyboard::Backspace;
        case VK_TAB: return sf::Keyboard::Tab;
        case VK_PAGE_UP: return sf::Keyboard::PageUp;
        case VK_PAGE_DOWN: return sf::Keyboard::PageDown;
        case VK_END: return sf::Keyboard::End;
        case VK_HOME: return sf::Keyboard::Home;
        case VK_INSERT: return sf::Keyboard::Insert;
        case VK_DELETE: return sf::Keyboard::Delete;
        case VK_ADD: return sf::Keyboard::Add;
        case VK_SUBTRACT: return sf::Keyboard::Subtract;
        case VK_MULTIPLY: return sf::Keyboard::Multiply;
        case VK_DIVIDE: return sf::Keyboard::Divide;
        case VK_LEFT: return sf::Keyboard::Left;
        case VK_RIGHT: return sf::Keyboard::Right;
        case VK_UP: return sf::Keyboard::Up;
        case VK_DOWN: return sf::Keyboard::Down;
        case VK_NUMPAD_0: return sf::Keyboard::Numpad0;
        case VK_NUMPAD_1: return sf::Keyboard::Numpad1;
        case VK_NUMPAD_2: return sf::Keyboard::Numpad2;
        case VK_NUMPAD_3: return sf::Keyboard::Numpad3;
        case VK_NUMPAD_4: return sf::Keyboard::Numpad4;
        case VK_NUMPAD_5: return sf::Keyboard::Numpad5;
        case VK_NUMPAD_6: return sf::Keyboard::Numpad6;
        case VK_NUMPAD_7: return sf::Keyboard::Numpad7;
        case VK_NUMPAD_8: return sf::Keyboard::Numpad8;
        case VK_NUMPAD_9: return sf::Keyboard::Numpad9;
        case VK_PAUSE: return sf::Keyboard::Pause;
        case VK_F1: return sf::Keyboard::F1;
        case VK_F2: return sf::Keyboard::F2;
        case VK_F3: return sf::Keyboard::F3;
        case VK_F4: return sf::Keyboard::F4;
        case VK_F5: return sf::Keyboard::F5;
        case VK_F6: return sf::Keyboard::F6;
        case VK_F7: return sf::Keyboard::F7;
        case VK_F8: return sf::Keyboard::F8;
        case VK_F9: return sf::Keyboard::F9;
        case VK_F10: return sf::Keyboard::F10;
        case VK_F11: return sf::Keyboard::F11;
        case VK_F12: return sf::Keyboard::F12;
        case VK_F13: return sf::Keyboard::F13;
        case VK_F14: return sf::Keyboard::F14;
        case VK_F15: return sf::Keyboard::F15;
        default:
            return sf::Keyboard::Unknown;
    }
}

inline
sf::Keyboard::Scan::Scancode ToSfScancode(UniversalInputEnum aValue) {
    switch (aValue) {
    case PK_UNKNOWN: return sf::Keyboard::Scan::Unknown;
    case PK_A: return sf::Keyboard::Scan::A;
    case PK_B: return sf::Keyboard::Scan::B;
    case PK_C: return sf::Keyboard::Scan::C;
    case PK_D: return sf::Keyboard::Scan::D;
    case PK_E: return sf::Keyboard::Scan::E;
    case PK_F: return sf::Keyboard::Scan::F;
    case PK_G: return sf::Keyboard::Scan::G;
    case PK_H: return sf::Keyboard::Scan::H;
    case PK_I: return sf::Keyboard::Scan::I;
    case PK_J: return sf::Keyboard::Scan::J;
    case PK_K: return sf::Keyboard::Scan::K;
    case PK_L: return sf::Keyboard::Scan::L;
    case PK_M: return sf::Keyboard::Scan::M;
    case PK_N: return sf::Keyboard::Scan::N;
    case PK_O: return sf::Keyboard::Scan::O;
    case PK_P: return sf::Keyboard::Scan::P;
    case PK_Q: return sf::Keyboard::Scan::Q;
    case PK_R: return sf::Keyboard::Scan::R;
    case PK_S: return sf::Keyboard::Scan::S;
    case PK_T: return sf::Keyboard::Scan::T;
    case PK_U: return sf::Keyboard::Scan::U;
    case PK_V: return sf::Keyboard::Scan::V;
    case PK_W: return sf::Keyboard::Scan::W;
    case PK_X: return sf::Keyboard::Scan::X;
    case PK_Y: return sf::Keyboard::Scan::Y;
    case PK_Z: return sf::Keyboard::Scan::Z;
    case PK_NUM_0: return sf::Keyboard::Scan::Num0;
    case PK_NUM_1: return sf::Keyboard::Scan::Num1;
    case PK_NUM_2: return sf::Keyboard::Scan::Num2;
    case PK_NUM_3: return sf::Keyboard::Scan::Num3;
    case PK_NUM_4: return sf::Keyboard::Scan::Num4;
    case PK_NUM_5: return sf::Keyboard::Scan::Num5;
    case PK_NUM_6: return sf::Keyboard::Scan::Num6;
    case PK_NUM_7: return sf::Keyboard::Scan::Num7;
    case PK_NUM_8: return sf::Keyboard::Scan::Num8;
    case PK_NUM_9: return sf::Keyboard::Scan::Num9;
    case PK_ESCAPE: return sf::Keyboard::Scan::Escape;
    case PK_LCONTROL: return sf::Keyboard::Scan::LControl;
    case PK_LSHIFT: return sf::Keyboard::Scan::LShift;
    case PK_LALT: return sf::Keyboard::Scan::LAlt;
    case PK_LSYSTEM: return sf::Keyboard::Scan::LSystem;
    case PK_RCONTROL: return sf::Keyboard::Scan::RControl;
    case PK_RSHIFT: return sf::Keyboard::Scan::RShift;
    case PK_RALT: return sf::Keyboard::Scan::RAlt;
    case PK_RSYSTEM: return sf::Keyboard::Scan::RSystem;
    case PK_MENU: return sf::Keyboard::Scan::Menu;
    case PK_LBRACKET: return sf::Keyboard::Scan::LBracket;
    case PK_RBRACKET: return sf::Keyboard::Scan::RBracket;
    case PK_SEMICOLON: return sf::Keyboard::Scan::Semicolon;
    case PK_COMMA: return sf::Keyboard::Scan::Comma;
    case PK_PERIOD: return sf::Keyboard::Scan::Period;
    case PK_APOSTROPHE: return sf::Keyboard::Scan::Apostrophe;
    case PK_SLASH: return sf::Keyboard::Scan::Slash;
    case PK_BACKSLASH: return sf::Keyboard::Scan::Backslash;
    case PK_GRAVE: return sf::Keyboard::Scan::Grave;
    case PK_EQUAL: return sf::Keyboard::Scan::Equal;
    case PK_HYPHEN: return sf::Keyboard::Scan::Hyphen;
    case PK_SPACE: return sf::Keyboard::Scan::Space;
    case PK_ENTER: return sf::Keyboard::Scan::Enter;
    case PK_BACKSPACE: return sf::Keyboard::Scan::Backspace;
    case PK_CAPS_LOCK: return sf::Keyboard::Scan::CapsLock;
    case PK_PRINT_SCREEN: return sf::Keyboard::Scan::PrintScreen;
    case PK_SCROLL_LOCK: return sf::Keyboard::Scan::ScrollLock;
    case PK_TAB: return sf::Keyboard::Scan::Tab;
    case PK_PAGE_UP: return sf::Keyboard::Scan::PageUp;
    case PK_PAGE_DOWN: return sf::Keyboard::Scan::PageDown;
    case PK_END: return sf::Keyboard::Scan::End;
    case PK_HOME: return sf::Keyboard::Scan::Home;
    case PK_INSERT: return sf::Keyboard::Scan::Insert;
    case PK_DELETE: return sf::Keyboard::Scan::Delete;
    case PK_NUMLOCK: return sf::Keyboard::Scan::NumLock;
    case PK_NUMPAD_PLUS: return sf::Keyboard::Scan::NumpadPlus;
    case PK_NUMPAD_MINUS: return sf::Keyboard::Scan::NumpadMinus;
    case PK_NUMPAD_MULTIPLY: return sf::Keyboard::Scan::NumpadMultiply;
    case PK_NUMPAD_DIVIDE: return sf::Keyboard::Scan::NumpadDivide;
    case PK_NUMPAD_EQUAL: return sf::Keyboard::Scan::NumpadEqual;
    case PK_NUMPAD_ENTER: return sf::Keyboard::Scan::NumpadEnter;
    case PK_NUMPAD_DECIMAL: return sf::Keyboard::Scan::NumpadDecimal;
    case PK_LEFT: return sf::Keyboard::Scan::Left;
    case PK_RIGHT: return sf::Keyboard::Scan::Right;
    case PK_UP: return sf::Keyboard::Scan::Up;
    case PK_DOWN: return sf::Keyboard::Scan::Down;
    case PK_NUMPAD_0: return sf::Keyboard::Scan::Numpad0;
    case PK_NUMPAD_1: return sf::Keyboard::Scan::Numpad1;
    case PK_NUMPAD_2: return sf::Keyboard::Scan::Numpad2;
    case PK_NUMPAD_3: return sf::Keyboard::Scan::Numpad3;
    case PK_NUMPAD_4: return sf::Keyboard::Scan::Numpad4;
    case PK_NUMPAD_5: return sf::Keyboard::Scan::Numpad5;
    case PK_NUMPAD_6: return sf::Keyboard::Scan::Numpad6;
    case PK_NUMPAD_7: return sf::Keyboard::Scan::Numpad7;
    case PK_NUMPAD_8: return sf::Keyboard::Scan::Numpad8;
    case PK_NUMPAD_9: return sf::Keyboard::Scan::Numpad9;
    case PK_PAUSE: return sf::Keyboard::Scan::Pause;
    case PK_F1: return sf::Keyboard::Scan::F1;
    case PK_F2: return sf::Keyboard::Scan::F2;
    case PK_F3: return sf::Keyboard::Scan::F3;
    case PK_F4: return sf::Keyboard::Scan::F4;
    case PK_F5: return sf::Keyboard::Scan::F5;
    case PK_F6: return sf::Keyboard::Scan::F6;
    case PK_F7: return sf::Keyboard::Scan::F7;
    case PK_F8: return sf::Keyboard::Scan::F8;
    case PK_F9: return sf::Keyboard::Scan::F9;
    case PK_F10: return sf::Keyboard::Scan::F10;
    case PK_F11: return sf::Keyboard::Scan::F11;
    case PK_F12: return sf::Keyboard::Scan::F12;
    case PK_F13: return sf::Keyboard::Scan::F13;
    case PK_F14: return sf::Keyboard::Scan::F14;
    case PK_F15: return sf::Keyboard::Scan::F15;
    case PK_F16: return sf::Keyboard::Scan::F16;
    case PK_F17: return sf::Keyboard::Scan::F17;
    case PK_F18: return sf::Keyboard::Scan::F18;
    case PK_F19: return sf::Keyboard::Scan::F19;
    case PK_F20: return sf::Keyboard::Scan::F20;
    case PK_F21: return sf::Keyboard::Scan::F21;
    case PK_F22: return sf::Keyboard::Scan::F22;
    case PK_F23: return sf::Keyboard::Scan::F23;
    case PK_F24: return sf::Keyboard::Scan::F24; 
    case PK_NON_US_BACKSLASH: return sf::Keyboard::Scan::NonUsBackslash;
    case PK_APPLICATION: return sf::Keyboard::Scan::Application;
    case PK_EXECUTE: return sf::Keyboard::Scan::Execute;
    case PK_MODE_CHANGE: return sf::Keyboard::Scan::ModeChange;
    case PK_HELP: return sf::Keyboard::Scan::Help;
    case PK_SELECT: return sf::Keyboard::Scan::Select;
    case PK_REDO: return sf::Keyboard::Scan::Redo;
    case PK_UNDO: return sf::Keyboard::Scan::Undo;
    case PK_CUT: return sf::Keyboard::Scan::Cut;
    case PK_COPY: return sf::Keyboard::Scan::Copy;
    case PK_PASTE: return sf::Keyboard::Scan::Paste;
    case PK_VOLUME_MUTE: return sf::Keyboard::Scan::VolumeMute;
    case PK_VOLUME_UP: return sf::Keyboard::Scan::VolumeUp;
    case PK_VOLUME_DOWN: return sf::Keyboard::Scan::VolumeDown;
    case PK_MEDIA_PLAY_PAUSE: return sf::Keyboard::Scan::MediaPlayPause;
    case PK_MEDIA_STOP: return sf::Keyboard::Scan::MediaStop;
    case PK_MEDIA_NEXT_TRACK: return sf::Keyboard::Scan::MediaNextTrack;
    case PK_MEDIA_PREVIOUS_TRACK: return sf::Keyboard::Scan::MediaPreviousTrack;
    case PK_BACK: return sf::Keyboard::Scan::Back;
    case PK_FORWARD: return sf::Keyboard::Scan::Forward;
    case PK_REFRESH: return sf::Keyboard::Scan::Refresh;
    case PK_STOP: return sf::Keyboard::Scan::Stop;
    case PK_SEARCH: return sf::Keyboard::Scan::Search;
    case PK_FAVORITES: return sf::Keyboard::Scan::Favorites;
    case PK_HOMEPAGE: return sf::Keyboard::Scan::HomePage;
    case PK_LAUNCH_APP_1: return sf::Keyboard::Scan::LaunchApplication1;
    case PK_LAUNCH_APP_2: return sf::Keyboard::Scan::LaunchApplication2;
    case PK_LAUNCH_MAIL: return sf::Keyboard::Scan::LaunchMail;
    case PK_LAUNCH_MEDIA_SELECT: return sf::Keyboard::Scan::LaunchMediaSelect;
    default:
        return sf::Keyboard::Scan::Unknown;
    }
}

inline
sf::Mouse::Button ToSfMouseButton(UniversalInputEnum aValue) {
}

inline
sf::Mouse::Wheel ToSfMouseWheel(UniversalInputEnum aValue) {
}

} // namespace in

HOBGOBLIN_NAMESPACE_END

#include <Hobgoblin/Private/Pmacro_undef.hpp>
