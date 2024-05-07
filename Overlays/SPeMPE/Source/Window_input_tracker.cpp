// Copyright 2024 Jovan Batnozic. Released under MS-PL licence in Serbia.
// See https://github.com/jbatnozic/Hobgoblin?tab=readme-ov-file#licence

// clang-format off


#include <SPeMPE/Utility/Window_input_tracker.hpp>

#include <Hobgoblin/HGExcept.hpp>

#include <utility>

namespace jbatnozic {
namespace spempe {
namespace detail {

WindowInputTracker::WindowInputTracker(GetViewRelativeMousePosFunc   aGetViewRelativeMousePos,
                                       GetWindowRelativeMousePosFunc aGetWindowRelativeMousePos)
    : _getViewRelativeMousePos{std::move(aGetViewRelativeMousePos)}
    , _getWindowRelativeMousePos{std::move(aGetWindowRelativeMousePos)}
{
    _controlBlocks.resize(static_cast<std::size_t>(hg::in::UNIVERSAL_INPUT_ENUM_COUNT));
}

///////////////////////////////////////////////////////////////////////////
// GENERAL                                                               //
///////////////////////////////////////////////////////////////////////////

WindowFrameInputView WindowInputTracker::getInputView() const {
    return WindowFrameInputView{this};
}

void WindowInputTracker::prepForEvents() {
    for (auto& controlBlock : _controlBlocks) {
        controlBlock.advance();
    }

    _mouseDidMove = false;
    _verticalScrollDelta = 0.f;
    _horizontalScrollDelta = 0.f;

    _cursorWithinWindow.advance();
}

void WindowInputTracker::eventOccurred(const hg::win::Event& aEvent) {
    aEvent.visit(
        [this](const hg::win::Event::GainedFocus&) {},
        [this](const hg::win::Event::LostFocus&) {},
        [this](const hg::win::Event::KeyPressed& aEventData) {
            const auto vk = aEventData.virtualKey;
            _controlBlocks.at(static_cast<std::size_t>(vk)).recordPress();
            const auto pk = aEventData.physicalKey;
            _controlBlocks.at(static_cast<std::size_t>(pk)).recordPress();
        },
        [this](const hg::win::Event::KeyReleased& aEventData) {
            const auto vk = aEventData.virtualKey;
            _controlBlocks.at(static_cast<std::size_t>(vk)).recordRelease();
            const auto pk = aEventData.physicalKey;
            _controlBlocks.at(static_cast<std::size_t>(pk)).recordRelease();
        },
        [this](const hg::win::Event::MouseButtonPressed& aEventData) {
            const auto mb = aEventData.button;
            _controlBlocks.at(static_cast<std::size_t>(mb)).recordPress();
        },
        [this](const hg::win::Event::MouseButtonReleased& aEventData) {
            const auto mb = aEventData.button;
            _controlBlocks.at(static_cast<std::size_t>(mb)).recordRelease();
        },
        [this](const hg::win::Event::MouseEntered&) {
            _cursorWithinWindow.recordPress();
        },
        [this](const hg::win::Event::MouseLeft&) {
            _cursorWithinWindow.recordRelease();
        },
        [this](const hg::win::Event::MouseMoved& aEventData) {
            _mouseDidMove = true;
        },
        [this](const hg::win::Event::MouseWheelScrolled& aEventData) {
            switch (aEventData.wheel.val()) {
            case hg::in::MW_VERTICAL:
                _verticalScrollDelta = aEventData.delta;
                break;

            case hg::in::MW_HORIZONTAL:
                _horizontalScrollDelta = aEventData.delta;
            }
        },
        [this](const hg::win::Event::TextEntered& aEventData) {
            // TODO(save)
        }
    );
}

///////////////////////////////////////////////////////////////////////////
// READING: UNIVERSAL                                                    //
///////////////////////////////////////////////////////////////////////////

bool WindowInputTracker::checkPressed(hg::in::UniversalInputEnum aKeyOrButton, Mode aMode) const {
    switch (aMode) {
    case Mode::Default:
        return _controlBlocks.at(static_cast<std::size_t>(aKeyOrButton)).isPressed();

    case Mode::Edge:
        return _controlBlocks.at(static_cast<std::size_t>(aKeyOrButton)).isPressedEdge();

    case Mode::Repeated:
        return _controlBlocks.at(static_cast<std::size_t>(aKeyOrButton)).isPressedRepeated();

    case Mode::Direct:
        if (hg::in::IsVirtualKeyboardKey(aKeyOrButton)) {
            return hg::in::CheckPressedVK(aKeyOrButton);
        }
        if (hg::in::IsPhysicalKeyboardKey(aKeyOrButton)) {
            return hg::in::CheckPressedPK(aKeyOrButton);
        }
        if (hg::in::IsMouseButton(aKeyOrButton)) {
            return hg::in::CheckPressedMB(aKeyOrButton);
        }
        // If none of the above...
        return false;

    default:
        HG_UNREACHABLE("Invalid mode ({}) passed.", (int)aMode);
    }
}

bool WindowInputTracker::checkReleased(hg::in::UniversalInputEnum aKeyOrButton, Mode aMode) const {
    switch (aMode) {
    case Mode::Default:
        return !(_controlBlocks.at(static_cast<std::size_t>(aKeyOrButton)).isPressed());

    case Mode::Edge:
        return _controlBlocks.at(static_cast<std::size_t>(aKeyOrButton)).isReleasedEdge();

    case Mode::Repeated:
        return _controlBlocks.at(static_cast<std::size_t>(aKeyOrButton)).isReleasedEdge();

    case Mode::Direct:
        if (hg::in::IsVirtualKeyboardKey(aKeyOrButton)) {
            return !hg::in::CheckPressedVK(aKeyOrButton);
        }
        if (hg::in::IsPhysicalKeyboardKey(aKeyOrButton)) {
            return !hg::in::CheckPressedPK(aKeyOrButton);
        }
        if (hg::in::IsMouseButton(aKeyOrButton)) {
            return !hg::in::CheckPressedMB(aKeyOrButton);
        }
        // If none of the above...
        return false;

    default:
        HG_UNREACHABLE("Invalid mode ({}) passed.", (int)aMode);
    }
}

///////////////////////////////////////////////////////////////////////////
// READING: MOUSE-SPECIFIC                                               //
///////////////////////////////////////////////////////////////////////////

bool WindowInputTracker::checkMouseMoved() const {
    return _mouseDidMove;
}

hg::math::Vector2f WindowInputTracker::getViewRelativeMousePos(hobgoblin::PZInteger aViewIndex) const {
    return _getViewRelativeMousePos(aViewIndex);
}

hg::math::Vector2i WindowInputTracker::getWindowRelativeMousePos() const {
    return _getWindowRelativeMousePos();
}

float WindowInputTracker::getVerticalMouseWheelScroll() const {
    return _verticalScrollDelta;
}

float WindowInputTracker::getHorizontalMouseWheelScroll() const {
    return _horizontalScrollDelta;
}

bool WindowInputTracker::checkMouseEnteredWindow() const {
    return _cursorWithinWindow.isPressedEdge();
}

bool WindowInputTracker::checkMouseLeftWindow() const {
    return _cursorWithinWindow.isReleasedEdge();
}

bool WindowInputTracker::checkMouseInWindow() const {
    return _cursorWithinWindow.isPressed();
}

///////////////////////////////////////////////////////////////////////////
// CONTROL BLOCK                                                         //
///////////////////////////////////////////////////////////////////////////

/*
Control block _status member is structured like this:

Bit 0: ABS bit for current frame (ABS = 'Absolute', that is, absolute YES or NO for 'is it pressed?')
Bit 1: EDGE bit for current frame (is 1 if the key was pressed/released in this frame)
Bit 2: ABS bit from one frame ago
Bit 3: EDGE bit from one frame ago
...

The whole thing is shifted 2 bits to the left every frame, preserving the ABS bit but clearing the EDGE bit.
*/

#define BIT_0 0x01
#define BIT_1 0x02
#define BIT_2 0x04

void WindowInputTracker::InputControlBlock::advance() {
    const decltype(_status) lowestBit = _status & BIT_0;
    _status = ((_status << 2) & ~(BIT_0 | BIT_1)) | lowestBit;
}

void WindowInputTracker::InputControlBlock::recordPress() {
    _status |= BIT_0; // Set bit 0 (ABS bit)
    _status |= BIT_1; // Set bit 1 (EDGE bit)
}

void WindowInputTracker::InputControlBlock::recordRelease() {
    _status &= ~BIT_0; // Clear bit 0 (ABS bit)
    _status |= BIT_1;  // Set bit 1 (EDGE bit)
}

bool WindowInputTracker::InputControlBlock::isPressed() const {
    return ((_status & BIT_0) != 0);
}

bool WindowInputTracker::InputControlBlock::isPressedEdge() const {
    // Is it pressed now but wasn't in the previous step?
    return (isPressed() && ((_status & BIT_2) == 0));
}

bool WindowInputTracker::InputControlBlock::isPressedRepeated() const {
    return (isPressed() && ((_status & BIT_1) != 0));
}

bool WindowInputTracker::InputControlBlock::isReleasedEdge() const {
    // Is it released now but wasn't in the previous step?
    return (!isPressed() && ((_status & BIT_2) != 0));
}

#undef BIT_2
#undef BIT_1
#undef BIT_0

} // namespace detail
} // namespace spempe
} // namespace jbatnozic

// clang-format on
