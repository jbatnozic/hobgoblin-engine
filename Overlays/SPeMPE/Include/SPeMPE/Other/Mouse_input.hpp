#ifndef SPEMPE_OTHER_MOUSE_INPUT_HPP
#define SPEMPE_OTHER_MOUSE_INPUT_HPP

#include <Hobgoblin/Common.hpp>

#include <SFML/System.hpp>

#include <functional>

namespace jbatnozic {
namespace spempe {

//! Mouse button enumeration
enum class MouseButton {
    Left,
    Right,
    Middle,
    Extra_1,
    Extra_2
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

private:
    friend class MouseInputTracker;

    explicit MouseInput(MouseInputTracker& aTracker);

    MouseInputTracker& _tracker;
};

class MouseInputTracker {
public:
    using GetMousePosFunc = std::function<sf::Vector2f(hobgoblin::PZInteger)>;

    explicit MouseInputTracker(GetMousePosFunc aGetMousePos);

    sf::Vector2f getMousePos(hobgoblin::PZInteger aViewIndex) const;

private:
    GetMousePosFunc _getMousePos;
};

} // namespace spempe
} // namespace jbatnozic

#endif // !SPEMPE_OTHER_MOUSE_INPUT_HPP
