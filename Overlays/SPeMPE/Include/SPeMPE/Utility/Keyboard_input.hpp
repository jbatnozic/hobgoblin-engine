#ifndef SPEMPE_OTHER_KEYBOARD_INPUT_HPP
#define SPEMPE_OTHER_KEYBOARD_INPUT_HPP

#include <Hobgoblin/Common.hpp>
#include <SFML/System/String.hpp>
#include <SFML/Window.hpp>

#include <cstdint>
#include <string>
#include <vector>

namespace jbatnozic {
namespace spempe {

//! Keyboard key enumeration
enum class KbKey {
    Unknown = -1, //! Unhandled key
    A,            //! The A key
    B,            //! The B key
    C,            //! The C key
    D,            //! The D key
    E,            //! The E key
    F,            //! The F key
    G,            //! The G key
    H,            //! The H key
    I,            //! The I key
    J,            //! The J key
    K,            //! The K key
    L,            //! The L key
    M,            //! The M key
    N,            //! The N key
    O,            //! The O key
    P,            //! The P key
    Q,            //! The Q key
    R,            //! The R key
    S,            //! The S key
    T,            //! The T key
    U,            //! The U key
    V,            //! The V key
    W,            //! The W key
    X,            //! The X key
    Y,            //! The Y key
    Z,            //! The Z key
    Num0,         //! The 0 key
    Num1,         //! The 1 key
    Num2,         //! The 2 key
    Num3,         //! The 3 key
    Num4,         //! The 4 key
    Num5,         //! The 5 key
    Num6,         //! The 6 key
    Num7,         //! The 7 key
    Num8,         //! The 8 key
    Num9,         //! The 9 key
    Escape,       //! The Escape key
    LControl,     //! The left Control key
    LShift,       //! The left Shift key
    LAlt,         //! The left Alt key
    LSystem,      //! The left OS specific key: window (Windows and Linux), apple (MacOS X), ...
    RControl,     //! The right Control key
    RShift,       //! The right Shift key
    RAlt,         //! The right Alt key
    RSystem,      //! The right OS specific key: window (Windows and Linux), apple (MacOS X), ...
    Menu,         //! The Menu key
    LBracket,     //! The [ key
    RBracket,     //! The ] key
    Semicolon,    //! The ; key
    Comma,        //! The , key
    Period,       //! The . key
    Quote,        //! The ' key
    Slash,        //! The / key
    Backslash,    //! The \ key
    Tilde,        //! The ~ key
    Equal,        //! The = key
    Hyphen,       //! The - key (hyphen)
    Space,        //! The Space key
    Enter,        //! The Enter/Return keys
    Backspace,    //! The Backspace key
    Tab,          //! The Tabulation key
    PageUp,       //! The Page up key
    PageDown,     //! The Page down key
    End,          //! The End key
    Home,         //! The Home key
    Insert,       //! The Insert key
    Delete,       //! The Delete key
    Add,          //! The + key
    Subtract,     //! The - key (minus, usually from numpad)
    Multiply,     //! The * key
    Divide,       //! The / key
    Left,         //! Left arrow
    Right,        //! Right arrow
    Up,           //! Up arrow
    Down,         //! Down arrow
    Numpad0,      //! The numpad 0 key
    Numpad1,      //! The numpad 1 key
    Numpad2,      //! The numpad 2 key
    Numpad3,      //! The numpad 3 key
    Numpad4,      //! The numpad 4 key
    Numpad5,      //! The numpad 5 key
    Numpad6,      //! The numpad 6 key
    Numpad7,      //! The numpad 7 key
    Numpad8,      //! The numpad 8 key
    Numpad9,      //! The numpad 9 key
    F1,           //! The F1 key
    F2,           //! The F2 key
    F3,           //! The F3 key
    F4,           //! The F4 key
    F5,           //! The F5 key
    F6,           //! The F6 key
    F7,           //! The F7 key
    F8,           //! The F8 key
    F9,           //! The F9 key
    F10,          //! The F10 key
    F11,          //! The F11 key
    F12,          //! The F12 key
    F13,          //! The F13 key
    F14,          //! The F14 key
    F15,          //! The F15 key
    Pause,        //! The Pause key

    KeyCount,     //! Keep last -- the total number of supported keyboard keys
};

//! Returns the string representation of a keyboard key. It will be the same as the name of
//! the corresponding enum field in 'KbKey' if it's a valid key, and "Unknown" otherwise.
std::string KbKeyToString(KbKey aKey);

//! Parses a string into KbKey value. The string has to contain exactly one of the names from
//! the 'KbKey' enum, otherwise KbKey::Unknown will be returned.
KbKey StringToKbKey(const std::string& aString);

class KbInputTracker;

//! Class to get keyboard inputs from a KbInputTracker.
//! Objects of this class are very lightweight.
class KbInput {
public:
    enum class Mode {
        //! Check if key is currently pressed/released; Takes window focus into consideration
        Default,

        //! Check if key was pressed/released this frame (and wasn't in the previous frame);
        //! Takes window focus into consideration
        Edge,

        //! Same as Edge, but also returns true every few frames afterwards so 
        //! long as the key remains pressed (with OS-defined frequency)
        Repeated,

        //! Poll the hardware directly to check if the key is pressed or not (disregards window focus)
        Direct
    };

    //! Checks if a key is/was pressed in accordance to the provided mode.
    bool checkPressed(KbKey aKey, Mode aMode = Mode::Default) const;

    //! Checks if a key is/was released in accordance to the provided mode.
    bool checkReleased(KbKey aKey, Mode aMode = Mode::Default) const;

    //! TODO
    const sf::String& getTypedText() const;

private:
    friend class KbInputTracker;

    explicit KbInput(const KbInputTracker& aTracker);

    const KbInputTracker& _tracker;
};

//! Class to change settings of a KbInputTracker and/or override recorded inputs.
//! Objects of this class are very lightweight.
class KbInputMutator {
public:
    void setTypedTextMaxLength(hobgoblin::PZInteger aMaxLength);

    void setTypedTextString(sf::String aString);

private:
    friend class KbInputTracker;

    explicit KbInputMutator(KbInputTracker& aTracker);

    KbInputTracker& _tracker;
};

//! Tracks inputs (key pressed and releases) from the keyboard.
class KbInputTracker {
public:
    KbInputTracker();

    KbInput getInput() const;

    KbInputMutator getMutator();

    void prepForEvents();
    void keyEventOccurred(const sf::Event& aEvent);
    void textEventOccurred(const sf::Event& aEvent);

private:
    friend class KbInput;
    friend class KbInputMutator;

    class KeyControlBlock {
    public:
        void advance();
        void recordPress();
        void recordRelease();

        bool isPressed() const;
        bool isPressedEdge() const;
        bool isPressedRepeated() const;
        bool isReleasedEdge() const;

    private:
        std::uint8_t _status = 0;
    };

    std::vector<KeyControlBlock> _controlBlocks;

    sf::String  _typedText;
    std::size_t _typedTextMaxLength = 256;

    void _limitTypedTextLength();
};

} // namespace spempe
} // namespace jbatnozic

#endif // !SPEMPE_OTHER_KEYBOARD_INPUT_HPP