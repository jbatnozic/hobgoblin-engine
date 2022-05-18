#ifndef SPEMPE_OTHER_MOUSE_INPUT_HPP
#define SPEMPE_OTHER_MOUSE_INPUT_HPP

#include <Hobgoblin/Common.hpp>

#include <SFML/Window.hpp>

#include <functional>
#include <vector>

namespace jbatnozic {
namespace spempe {

//! Mouse button enumeration
enum class MouseButton {
    Unknown = -1,

    Left,
    Right,
    Middle,
    Extra_1,
    Extra_2,

    ButtonCount
};

class MouseInputTracker;

class MouseInput {
public:
    enum class Mode {
        //! Check if button is currently pressed/released; Takes window focus into consideration
        Default,

        //! Check if button was pressed/released this frame (and wasn't in the previous frame);
        //! Takes window focus into consideration
        Edge,

        //! Poll the hardware directly to check if the key is pressed or not (disregards window focus)
        Direct
    };

    //! Checks if a button is/was pressed in accordance to the provided mode.
    bool checkPressed(MouseButton aKey, Mode aMode = Mode::Default) const;

    //! Checks if a button is/was released in accordance to the provided mode.
    bool checkReleased(MouseButton aKey, Mode aMode = Mode::Default) const;

    //! Checks if the mouse cursor was moved since the last frame.
    bool checkMoved() const;

    //! Returns the  position of the mouse cursor relative to the selected view (in game world coordinates).
    sf::Vector2f getViewRelativePos(hobgoblin::PZInteger aViewIndex = 0) const;

    //! Returns the position of the mouse cursor relative to the window (in window coordinates).
    sf::Vector2i getWindowRelativePos() const;

    //! Positive is up, negative is down, 0 = no change since last frame
    float getVerticalWheelScroll() const;

    //! Positive is left, negative is right, 0 = no change since last frame
    float getHorizontalWheelScroll() const;

    //! Checks if the mouse cursor entered the bounds of the window since the last frame.
    bool checkEnteredWindow() const;

    //! Checks if the mouse cursor left the bounds of the window since the last frame.
    bool checkLeftWindow() const;

    //! Checks if the mouse cursor is currently within the bounds of the window.
    bool checkInWindow() const;

private:
    friend class MouseInputTracker;

    explicit MouseInput(const MouseInputTracker& aTracker);

    const MouseInputTracker& _tracker;
};

class MouseInputMutator {
    // TODO
};

class MouseInputTracker {
public:
    using GetViewRelativeMousePosFunc   = std::function<sf::Vector2f(hobgoblin::PZInteger)>;
    using GetWindowRelativeMousePosFunc = std::function<sf::Vector2i()>;

    explicit MouseInputTracker(GetViewRelativeMousePosFunc   aGetViewRelativeMousePos,
                               GetWindowRelativeMousePosFunc aGetWindowRelativeMousePos);

    MouseInput getInput() const;

    MouseInputMutator getMutator();

    void prepForEvents();
    void buttonEventOccurred(const sf::Event& aEvent);

private:
    friend class MouseInput;

    GetViewRelativeMousePosFunc   _getViewRelativeMousePos;
    GetWindowRelativeMousePosFunc _getWindowRelativeMousePos;

    class ButtonControlBlock {
    public:
        void advance();
        void recordPress();
        void recordRelease();

        bool isPressed() const;
        bool isPressedEdge() const;
        bool isReleasedEdge() const;

    private:
        std::uint8_t _status = 0;
    };

    std::vector<ButtonControlBlock> _controlBlocks;

    bool _didMove = false;

    float _verticalScrollDelta = 0.f;
    float _horizontalScrollDelta = 0.f;

    // enter = "press", leave = "release"
    ButtonControlBlock _cursorWithinWindow;
};

} // namespace spempe
} // namespace jbatnozic

#endif // !SPEMPE_OTHER_MOUSE_INPUT_HPP
