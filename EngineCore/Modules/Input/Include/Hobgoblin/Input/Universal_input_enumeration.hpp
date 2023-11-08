#ifndef UHOBGOBLIN_INPUT_UNIVERSAL_INPUT_ENUMERATION_HPP
#define UHOBGOBLIN_INPUT_UNIVERSAL_INPUT_ENUMERATION_HPP

#include <Hobgoblin/Common.hpp>

#include <string>
#include <optional>

#include <Hobgoblin/Private/Pmacro_define.hpp>

HOBGOBLIN_NAMESPACE_BEGIN
namespace in {

//! Universal enumeration of all possible user inputs.
enum UniversalInputEnum {

    ///////////////////////////////////////////////////////////////////////////
    // VIRTUAL KEYBOARD KEYS                                                 //
    ///////////////////////////////////////////////////////////////////////////

#define HG_INPUT_VIRTUALKEY_FIRST ::jbatnozic::hobgoblin::in::VK_A

    VK_UNKNOWN,      //!< Unhandled key
    VK_A,            //!< The A key
    VK_B,            //!< The B key
    VK_C,            //!< The C key
    VK_D,            //!< The D key
    VK_E,            //!< The E key
    VK_F,            //!< The F key
    VK_G,            //!< The G key
    VK_H,            //!< The H key
    VK_I,            //!< The I key
    VK_J,            //!< The J key
    VK_K,            //!< The K key
    VK_L,            //!< The L key
    VK_M,            //!< The M key
    VK_N,            //!< The N key
    VK_O,            //!< The O key
    VK_P,            //!< The P key
    VK_Q,            //!< The Q key
    VK_R,            //!< The R key
    VK_S,            //!< The S key
    VK_T,            //!< The T key
    VK_U,            //!< The U key
    VK_V,            //!< The V key
    VK_W,            //!< The W key
    VK_X,            //!< The X key
    VK_Y,            //!< The Y key
    VK_Z,            //!< The Z key
    VK_NUM_0,        //!< The 0 key
    VK_NUM_1,        //!< The 1 key
    VK_NUM_2,        //!< The 2 key
    VK_NUM_3,        //!< The 3 key
    VK_NUM_4,        //!< The 4 key
    VK_NUM_5,        //!< The 5 key
    VK_NUM_6,        //!< The 6 key
    VK_NUM_7,        //!< The 7 key
    VK_NUM_8,        //!< The 8 key
    VK_NUM_9,        //!< The 9 key
    VK_ESCAPE,       //!< The Escape key
    VK_LCONTROL,     //!< The left Control key
    VK_LSHIFT,       //!< The left Shift key
    VK_LALT,         //!< The left Alt key
    VK_LSYSTEM,      //!< The left OS specific key: window (Windows and Linux), apple (macOS), ...
    VK_RCONTROL,     //!< The right Control key
    VK_RSHIFT,       //!< The right Shift key
    VK_RALT,         //!< The right Alt key
    VK_RSYSTEM,      //!< The right OS specific key: window (Windows and Linux), apple (macOS), ...
    VK_MENU,         //!< The Menu key
    VK_LBRACKET,     //!< The [ key
    VK_RBRACKET,     //!< The ] key
    VK_SEMICOLON,    //!< The ; key
    VK_COMMA,        //!< The , key
    VK_PERIOD,       //!< The . key
    VK_APOSTROPHE,   //!< The ' key
    VK_SLASH,        //!< The / key
    VK_BACKSLASH,    //!< The \ key
    VK_GRAVE,        //!< The ` key (key where the tilde also sits)
    VK_EQUAL,        //!< The = key
    VK_HYPHEN,       //!< The - key (hyphen)
    VK_SPACE,        //!< The Space key
    VK_ENTER,        //!< The Enter/Return keys
    VK_BACKSPACE,    //!< The Backspace key
    VK_TAB,          //!< The Tabulation key
    VK_PAGE_UP,      //!< The Page up key
    VK_PAGE_DOWN,    //!< The Page down key
    VK_END,          //!< The End key
    VK_HOME,         //!< The Home key
    VK_INSERT,       //!< The Insert key
    VK_DELETE,       //!< The Delete key
    VK_ADD,          //!< The + key
    VK_SUBTRACT,     //!< The - key (minus, usually from numpad)
    VK_MULTIPLY,     //!< The * key
    VK_DIVIDE,       //!< The / key
    VK_LEFT,         //!< Left arrow
    VK_RIGHT,        //!< Right arrow
    VK_UP,           //!< Up arrow
    VK_DOWN,         //!< Down arrow
    VK_NUMPAD_0,     //!< The numpad 0 key
    VK_NUMPAD_1,     //!< The numpad 1 key
    VK_NUMPAD_2,     //!< The numpad 2 key
    VK_NUMPAD_3,     //!< The numpad 3 key
    VK_NUMPAD_4,     //!< The numpad 4 key
    VK_NUMPAD_5,     //!< The numpad 5 key
    VK_NUMPAD_6,     //!< The numpad 6 key
    VK_NUMPAD_7,     //!< The numpad 7 key
    VK_NUMPAD_8,     //!< The numpad 8 key
    VK_NUMPAD_9,     //!< The numpad 9 key
    VK_PAUSE,        //!< The Pause key
    VK_F1,           //!< The F1 key
    VK_F2,           //!< The F2 key
    VK_F3,           //!< The F3 key
    VK_F4,           //!< The F4 key
    VK_F5,           //!< The F5 key
    VK_F6,           //!< The F6 key
    VK_F7,           //!< The F7 key
    VK_F8,           //!< The F8 key
    VK_F9,           //!< The F9 key
    VK_F10,          //!< The F10 key
    VK_F11,          //!< The F11 key
    VK_F12,          //!< The F12 key
    VK_F13,          //!< The F13 key
    VK_F14,          //!< The F14 key
    VK_F15,          //!< The F15 key

#define HG_INPUT_VIRTUALKEY_LAST ::jbatnozic::hobgoblin::in::VK_F15

    ///////////////////////////////////////////////////////////////////////////
    // PHYSICAL KEYBOARD KEYS                                                //
    ///////////////////////////////////////////////////////////////////////////

#define HG_INPUT_PHYSICALKEY_FIRST ::jbatnozic::hobgoblin::in::PK_A

    PK_UNKNOWN,      //!< Unhandled key
    PK_A,            //!< The A key
    PK_B,            //!< The B key
    PK_C,            //!< The C key
    PK_D,            //!< The D key
    PK_E,            //!< The E key
    PK_F,            //!< The F key
    PK_G,            //!< The G key
    PK_H,            //!< The H key
    PK_I,            //!< The I key
    PK_J,            //!< The J key
    PK_K,            //!< The K key
    PK_L,            //!< The L key
    PK_M,            //!< The M key
    PK_N,            //!< The N key
    PK_O,            //!< The O key
    PK_P,            //!< The P key
    PK_Q,            //!< The Q key
    PK_R,            //!< The R key
    PK_S,            //!< The S key
    PK_T,            //!< The T key
    PK_U,            //!< The U key
    PK_V,            //!< The V key
    PK_W,            //!< The W key
    PK_X,            //!< The X key
    PK_Y,            //!< The Y key
    PK_Z,            //!< The Z key
    PK_NUM_0,        //!< The 0 key
    PK_NUM_1,        //!< The 1 key
    PK_NUM_2,        //!< The 2 key
    PK_NUM_3,        //!< The 3 key
    PK_NUM_4,        //!< The 4 key
    PK_NUM_5,        //!< The 5 key
    PK_NUM_6,        //!< The 6 key
    PK_NUM_7,        //!< The 7 key
    PK_NUM_8,        //!< The 8 key
    PK_NUM_9,        //!< The 9 key
    PK_ESCAPE,       //!< The Escape key
    PK_LCONTROL,     //!< The left Control key
    PK_LSHIFT,       //!< The left Shift key
    PK_LALT,         //!< The left Alt key
    PK_LSYSTEM,      //!< The left OS specific key: window (Windows and Linux), apple (macOS), ...
    PK_RCONTROL,     //!< The right Control key
    PK_RSHIFT,       //!< The right Shift key
    PK_RALT,         //!< The right Alt key
    PK_RSYSTEM,      //!< The right OS specific key: window (Windows and Linux), apple (macOS), ...
    PK_MENU,         //!< The Menu key
    PK_LBRACKET,     //!< The [ key
    PK_RBRACKET,     //!< The ] key
    PK_SEMICOLON,    //!< The ; key
    PK_COMMA,        //!< The , key
    PK_PERIOD,       //!< The . key
    PK_APOSTROPHE,   //!< The ' key
    PK_SLASH,        //!< The / key
    PK_BACKSLASH,    //!< The \ key
    PK_GRAVE,        //!< The ` key (key where the tilde also sits)
    PK_EQUAL,        //!< The = key
    PK_HYPHEN,       //!< The - key (hyphen)
    PK_SPACE,        //!< The Space key
    PK_ENTER,        //!< The Enter/Return keys
    PK_BACKSPACE,    //!< The Backspace key
    PK_CAPS_LOCK,
    PK_PRINT_SCREEN,
    PK_SCROLL_LOCK,
    PK_TAB,          //!< The Tabulation key
    PK_PAGE_UP,      //!< The Page up key
    PK_PAGE_DOWN,    //!< The Page down key
    PK_END,          //!< The End key
    PK_HOME,         //!< The Home key
    PK_INSERT,       //!< The Insert key
    PK_DELETE,       //!< The Delete key
    PK_NUMLOCK,
    PK_NUMPAD_PLUS,     //!< The + key
    PK_NUMPAD_MINUS,    //!< The - key (minus, usually from numpad)
    PK_NUMPAD_MULTIPLY, //!< The * key
    PK_NUMPAD_DIVIDE,   //!< The / key
    PK_NUMPAD_EQUAL,
    PK_NUMPAD_ENTER,
    PK_NUMPAD_DECIMAL,
    PK_LEFT,         //!< Left arrow
    PK_RIGHT,        //!< Right arrow
    PK_UP,           //!< Up arrow
    PK_DOWN,         //!< Down arrow
    PK_NUMPAD_0,     //!< The numpad 0 key
    PK_NUMPAD_1,     //!< The numpad 1 key
    PK_NUMPAD_2,     //!< The numpad 2 key
    PK_NUMPAD_3,     //!< The numpad 3 key
    PK_NUMPAD_4,     //!< The numpad 4 key
    PK_NUMPAD_5,     //!< The numpad 5 key
    PK_NUMPAD_6,     //!< The numpad 6 key
    PK_NUMPAD_7,     //!< The numpad 7 key
    PK_NUMPAD_8,     //!< The numpad 8 key
    PK_NUMPAD_9,     //!< The numpad 9 key
    PK_PAUSE,        //!< The Pause key
    PK_F1,           //!< The F1 key
    PK_F2,           //!< The F2 key
    PK_F3,           //!< The F3 key
    PK_F4,           //!< The F4 key
    PK_F5,           //!< The F5 key
    PK_F6,           //!< The F6 key
    PK_F7,           //!< The F7 key
    PK_F8,           //!< The F8 key
    PK_F9,           //!< The F9 key
    PK_F10,          //!< The F10 key
    PK_F11,          //!< The F11 key
    PK_F12,          //!< The F12 key
    PK_F13,          //!< The F13 key
    PK_F14,          //!< The F14 key
    PK_F15,          //!< The F15 key

    // Uncommon physical keys:

    PK_F16,                  //!< The F16 key
    PK_F17,                  //!< The F17 key
    PK_F18,                  //!< The F18 key
    PK_F19,                  //!< The F19 key
    PK_F20,                  //!< The F20 key
    PK_F21,                  //!< The F21 key
    PK_F22,                  //!< The F22 key
    PK_F23,                  //!< The F23 key
    PK_F24,                  //!< The F24 key 
    PK_NON_US_BACKSLASH,     //!< Keyboard Non-US \ and | key
    PK_APPLICATION,          //!< Keyboard Application key
    PK_EXECUTE,              //!< Keyboard Execute key
    PK_MODE_CHANGE,          //!< Keyboard Mode Change key
    PK_HELP,                 //!< Keyboard Help key
    PK_SELECT,               //!< Keyboard Select key
    PK_REDO,                 //!< Keyboard Redo key
    PK_UNDO,                 //!< Keyboard Undo key
    PK_CUT,                  //!< Keyboard Cut key
    PK_COPY,                 //!< Keyboard Copy key
    PK_PASTE,                //!< Keyboard Paste key
    PK_VOLUME_MUTE,          //!< Keyboard Volume Mute key
    PK_VOLUME_UP,            //!< Keyboard Volume Up key
    PK_VOLUME_DOWN,          //!< Keyboard Volume Down key
    PK_MEDIA_PLAY_PAUSE,     //!< Keyboard Media Play Pause key
    PK_MEDIA_STOP,           //!< Keyboard Media Stop key
    PK_MEDIA_NEXT_TRACK,     //!< Keyboard Media Next Track key
    PK_MEDIA_PREVIOUS_TRACK, //!< Keyboard Media Previous Track key
    PK_BACK,                 //!< Keyboard Back key
    PK_FORWARD,              //!< Keyboard Forward key
    PK_REFRESH,              //!< Keyboard Refresh key
    PK_STOP,                 //!< Keyboard Stop key
    PK_SEARCH,               //!< Keyboard Search key
    PK_FAVORITES,            //!< Keyboard Favorites key
    PK_HOMEPAGE,             //!< Keyboard Home Page key
    PK_LAUNCH_APP_1,         //!< Keyboard Launch Application 1 key
    PK_LAUNCH_APP_2,         //!< Keyboard Launch Application 2 key
    PK_LAUNCH_MAIL,          //!< Keyboard Launch Mail key
    PK_LAUNCH_MEDIA_SELECT,  //!< Keyboard Launch Media Select key

#define HG_INPUT_PHYSICALKEY_LAST ::jbatnozic::hobgoblin::in::PK_LAUNCH_MEDIA_SELECT

    ///////////////////////////////////////////////////////////////////////////
    // MOUSE BUTTONS                                                         //
    ///////////////////////////////////////////////////////////////////////////

#define HG_INPUT_MOUSEBUTTON_FIRST ::jbatnozic::hobgoblin::in::MB_LEFT

    MB_UNKNOWN, //!< Unhandled mouse button
    MB_LEFT,    //!< The left mouse button
    MB_RIGHT,   //!< The right mouse button
    MB_MIDDLE,  //!< The middle (wheel) mouse button
    MB_EXTRA_1, //!< The first extra mouse button
    MB_EXTRA_2, //!< The second extra mouse button

#define HG_INPUT_MOUSEBUTTON_LAST ::jbatnozic::hobgoblin::in::MB_EXTRA_2

    ///////////////////////////////////////////////////////////////////////////
    // MOUSE WHEELS                                                          //
    ///////////////////////////////////////////////////////////////////////////

#define HG_INPUT_MOUSEWHEEL_FIRST ::jbatnozic::hobgoblin::in::MW_VERTICAL

    MW_UNKNOWN,    //!< Unhandled mouse wheel
    MW_VERTICAL,   //!< The vertical mouse wheel
    MW_HORIZONTAL, //!< The vertical mouse wheel

#define HG_INPUT_MOUSEWHEEL_LAST ::jbatnozic::hobgoblin::in::MW_HORIZONTAL

    ///////////////////////////////////////////////////////////////////////////
    // TO DO                                                                 //
    ///////////////////////////////////////////////////////////////////////////

        // - touch
        // - joystick/controller

    ///////////////////////////////////////////////////////////////////////////
    // COUNTER                                                               //
    ///////////////////////////////////////////////////////////////////////////

    UNIVERSAL_INPUT_ENUM_COUNT //!< Number of enum elements. Always keep last.
};

//! Checks whether a value can be mapped to a virtual keyboard key.
//! Returns `true` for all VK_* values of the `UniversalInputEnum` EXCEPT
//! for `VK_UNKNOWN`.
constexpr bool IsVirtualKeyboardKey(int aValue) {
    return aValue >= HG_INPUT_VIRTUALKEY_FIRST && aValue <= HG_INPUT_VIRTUALKEY_LAST;
}

//! Checks whether a value can be mapped to a physical keyboard key.
//! Returns `true` for all PK_* values of the `UniversalInputEnum` EXCEPT
//! for `PK_UNKNOWN`.
constexpr bool IsPhysicalKeyboardKey(int aValue) {
    return aValue >= HG_INPUT_PHYSICALKEY_FIRST && aValue <= HG_INPUT_PHYSICALKEY_LAST;
}

//! Checks whether a value can be mapped to a mouse button.
//! Returns `true` for all MB_* values of the `UniversalInputEnum` EXCEPT
//! for `MB_UNKNOWN`.
constexpr bool IsMouseButton(int aValue) {
    return aValue >= HG_INPUT_MOUSEBUTTON_FIRST && aValue <= HG_INPUT_MOUSEBUTTON_LAST;
}

//! Checks whether a value can be mapped to a mouse wheel.
//! Returns `true` for all MW_* values of the `UniversalInputEnum` EXCEPT
//! for `MW_UNKNOWN`.
constexpr bool IsMouseWheel(int aValue) {
    return aValue >= HG_INPUT_MOUSEWHEEL_FIRST && aValue <= HG_INPUT_MOUSEWHEEL_LAST;
}

//! TODO(add description)
std::optional<UniversalInputEnum> StringToInput(const std::string& aString);

//! TODO(add description)
std::optional<std::string> InputToString(UniversalInputEnum aValue);

} // namespace in
HOBGOBLIN_NAMESPACE_END

#include <Hobgoblin/Private/Pmacro_undef.hpp>
#include <Hobgoblin/Private/Short_namespace.hpp>

#endif // !UHOBGOBLIN_INPUT_UNIVERSAL_INPUT_ENUMERATION_HPP
