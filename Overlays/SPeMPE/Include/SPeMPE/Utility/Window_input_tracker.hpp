#ifndef SPEMPE_UTILITY_MOUSE_INPUT_HPP
#define SPEMPE_UTILITY_MOUSE_INPUT_HPP

#include <Hobgoblin/Common.hpp>
#include <Hobgoblin/Input.hpp>
#include <Hobgoblin/Window.hpp>
#include <Hobgoblin/Math/Vector.hpp>

#include <SPeMPE/Utility/Window_frame_input_view.hpp>

#include <functional>
#include <vector>

namespace jbatnozic {
namespace spempe {
namespace detail {

namespace hg = ::jbatnozic::hobgoblin;

class WindowInputTracker {
public:
    using GetViewRelativeMousePosFunc   = std::function<hg::math::Vector2f(hg::PZInteger)>;
    using GetWindowRelativeMousePosFunc = std::function<hg::math::Vector2i()>;

    WindowInputTracker(GetViewRelativeMousePosFunc   aGetViewRelativeMousePos,
                       GetWindowRelativeMousePosFunc aGetWindowRelativeMousePos);

    ///////////////////////////////////////////////////////////////////////////
    // GENERAL                                                               //
    ///////////////////////////////////////////////////////////////////////////

    WindowFrameInputView getInputView() const;

    void prepForEvents();

    void eventOccurred(const hg::win::Event& aEvent);

    ///////////////////////////////////////////////////////////////////////////
    // READING: UNIVERSAL                                                    //
    ///////////////////////////////////////////////////////////////////////////

    using Mode = WindowFrameInputView::Mode;

    bool checkPressed(hg::in::UniversalInputEnum aKeyOrButton, Mode aMode = Mode::Default) const;

    bool checkReleased(hg::in::UniversalInputEnum aKeyOrButton, Mode aMode = Mode::Default) const;

    ///////////////////////////////////////////////////////////////////////////
    // READING: MOUSE-SPECIFIC                                               //
    ///////////////////////////////////////////////////////////////////////////

    bool checkMouseMoved() const;

    hg::math::Vector2f getViewRelativeMousePos(hobgoblin::PZInteger aViewIndex = 0) const;

    hg::math::Vector2i getWindowRelativeMousePos() const;

    float getVerticalMouseWheelScroll() const;

    float getHorizontalMouseWheelScroll() const;

    bool checkMouseEnteredWindow() const;

    bool checkMouseLeftWindow() const;

    bool checkMouseInWindow() const;

private:
    friend class MouseInput;

    class InputControlBlock {
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

    // Universal:

    std::vector<InputControlBlock> _controlBlocks;

    // Mouse-specific:

    GetViewRelativeMousePosFunc   _getViewRelativeMousePos;
    GetWindowRelativeMousePosFunc _getWindowRelativeMousePos;

    bool _mouseDidMove = false;

    float _verticalScrollDelta = 0.f;
    float _horizontalScrollDelta = 0.f;

    // enter = "press", leave = "release"
    InputControlBlock _cursorWithinWindow;
};

} // namespace detail
} // namespace spempe
} // namespace jbatnozic

#endif // !SPEMPE_UTILITY_MOUSE_INPUT_HPP
