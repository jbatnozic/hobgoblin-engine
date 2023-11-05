#ifndef SPEMPE_UTILITY_WINDOW_FRAME_INPUT_VIEW_HPP
#define SPEMPE_UTILITY_WINDOW_FRAME_INPUT_VIEW_HPP

#include <Hobgoblin/Common.hpp>
#include <Hobgoblin/Input/Universal_input_enumeration.hpp>
#include <Hobgoblin/Math/Vector.hpp>

#include <functional>
#include <vector>

namespace jbatnozic {
namespace spempe {

namespace hg = ::jbatnozic::hobgoblin;

namespace detail {
class WindowInputTracker;
} // namespace detail

// TODO(add description)
class WindowFrameInputView {
public:
    enum class Mode { // TODO(make standalone so we don't have to type WindowFrameInputView every time)
        //! Check if key or button is currently pressed/released.
        //! Takes window focus into consideration.
        Default,

        //! Check if key or button was pressed/released this frame (and wasn't in the previous frame);
        //! Takes window focus into consideration.
        Edge,

        //! For KEYBOARD keys:
        //!     Same as Edge, but also returns true every few frames afterwards so 
        //!     long as the key remains pressed (with OS-defined frequency).
        //! 
        //! For MOUSE buttons:
        //!     Same as Edge.
        Repeated,

        //! Poll the hardware directly to check if the key or button is pressed or not.
        //! Disregards window focus.
        Direct
    };

    ///////////////////////////////////////////////////////////////////////////
    // UNIVERSAL                                                             //
    ///////////////////////////////////////////////////////////////////////////

    //! Checks if a key or button is/was pressed in accordance to the provided mode.
    bool checkPressed(hg::in::UniversalInputEnum aKeyOrButton, Mode aMode = Mode::Default) const;

    //! Checks if a key or button is/was released in accordance to the provided mode.
    bool checkReleased(hg::in::UniversalInputEnum aKeyOrButton, Mode aMode = Mode::Default) const;

    ///////////////////////////////////////////////////////////////////////////
    // KEYBOARD-SPECIFIC                                                     //
    ///////////////////////////////////////////////////////////////////////////

    // typed txt..

    ///////////////////////////////////////////////////////////////////////////
    // MOUSE-SPECIFIC                                                        //
    ///////////////////////////////////////////////////////////////////////////

    //! Checks if the mouse cursor was moved since the last frame.
    bool checkMouseMoved() const;

    //! Returns the  position of the mouse cursor relative to the selected view (in game world coordinates).
    hg::math::Vector2f getViewRelativeMousePos(hobgoblin::PZInteger aViewIndex = 0) const;

    //! Returns the position of the mouse cursor relative to the window (in window coordinates).
    hg::math::Vector2i getWindowRelativeMousePos() const;

    //! Positive is up, negative is down, 0 = no change since last frame
    float getVerticalMouseWheelScroll() const;

    //! Positive is left, negative is right, 0 = no change since last frame
    float getHorizontalMouseWheelScroll() const;

    //! Checks if the mouse cursor entered the bounds of the window since the last frame.
    bool checkMouseEnteredWindow() const;

    //! Checks if the mouse cursor left the bounds of the window since the last frame.
    bool checkMouseLeftWindow() const;

    //! Checks if the mouse cursor is currently within the bounds of the window.
    bool checkMouseInWindow() const;

private:
    friend class detail::WindowInputTracker;

    explicit WindowFrameInputView(hg::not_null<const detail::WindowInputTracker*> aTracker);

    const detail::WindowInputTracker& _tracker;
};

} // namespace spempe
} // namespace jbatnozic

#endif // !SPEMPE_UTILITY_WINDOW_FRAME_INPUT_VIEW_HPP
