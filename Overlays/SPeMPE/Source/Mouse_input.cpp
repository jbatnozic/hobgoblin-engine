
#include <SPeMPE/Other/Mouse_input.hpp>

#include <utility>

namespace jbatnozic {
namespace spempe {

namespace {

sf::Mouse::Button ToSfButton(MouseButton aButton) {
    switch (aButton) {
    case MouseButton::Left:
        return sf::Mouse::Left;

    case MouseButton::Right:
        return sf::Mouse::Right;

    case MouseButton::Middle:
        return sf::Mouse::Middle;

    case MouseButton::Extra_1:
        return sf::Mouse::XButton1;

    case MouseButton::Extra_2:
        return sf::Mouse::XButton2;

    default: {}
    }

    throw hobgoblin::TracedRuntimeError{"ToSfButton - Unknown button type provided!"};
}

MouseButton ToSPeMPEButton(sf::Mouse::Button aButton) {
    switch (aButton) {
    case sf::Mouse::Left:
        return MouseButton::Left;

    case sf::Mouse::Right:
        return MouseButton::Right;

    case sf::Mouse::Middle:
        return MouseButton::Middle;

    case sf::Mouse::XButton1:
        return MouseButton::Extra_1;

    case sf::Mouse::XButton2:
        return MouseButton::Extra_2;

    default: {}
    }

    return MouseButton::Unknown;
}

} // namespace

///////////////////////////////////////////////////////////////////////////
// MOUSEINPUT                                                            //
///////////////////////////////////////////////////////////////////////////

MouseInput::MouseInput(const MouseInputTracker& aTracker)
    : _tracker{aTracker}
{    
}

bool MouseInput::checkPressed(MouseButton aButton, Mode aMode) const {
   if (aButton < MouseButton::Left || aButton >= MouseButton::ButtonCount) {
       throw hobgoblin::TracedLogicError{"Must pass a valid MouseButton value (except 'Unknown')!"};
   }

   switch (aMode) {
   case Mode::Default:
       return _tracker._controlBlocks[static_cast<std::size_t>(aButton)].isPressed();

   case Mode::Edge:
       return _tracker._controlBlocks[static_cast<std::size_t>(aButton)].isPressedEdge();

   case Mode::Direct:
       return sf::Mouse::isButtonPressed(ToSfButton(aButton));

   default:
       throw hobgoblin::TracedLogicError{"Invalid mode passed!"};
   }
}

bool MouseInput::checkReleased(MouseButton aButton, Mode aMode) const {
    if (aButton < MouseButton::Left || aButton >= MouseButton::ButtonCount) {
        throw hobgoblin::TracedLogicError{"Must pass a valid MouseButton value (except 'Unknown')!"};
    }

    switch (aMode) {
    case Mode::Default:
        return !(_tracker._controlBlocks[static_cast<std::size_t>(aButton)].isPressed());

    case Mode::Edge:
        return _tracker._controlBlocks[static_cast<std::size_t>(aButton)].isReleasedEdge();

    case Mode::Direct:
        return !sf::Mouse::isButtonPressed(ToSfButton(aButton));

    default:
        throw hobgoblin::TracedLogicError{"Invalid mode passed!"};
    }
}

bool MouseInput::checkMoved() const {
    return _tracker._didMove;
}

sf::Vector2f MouseInput::getViewRelativePos(hobgoblin::PZInteger aViewIndex) const {
    return _tracker._getViewRelativeMousePos(aViewIndex);
}

sf::Vector2i MouseInput::getWindowRelativePos() const {
    return _tracker._getWindowRelativeMousePos();
}

float MouseInput::getVerticalWheelScroll() const {
    return _tracker._verticalScrollDelta;
}

float MouseInput::getHorizontalWheelScroll() const {
    return _tracker._horizontalScrollDelta;
}

bool MouseInput::checkEnteredWindow() const {
    return  _tracker._cursorWithinWindow.isPressedEdge();
}

bool MouseInput::checkLeftWindow() const {
    return  _tracker._cursorWithinWindow.isReleasedEdge();
}

bool MouseInput::checkInWindow() const {
    return  _tracker._cursorWithinWindow.isPressed();
}

///////////////////////////////////////////////////////////////////////////
// CONTROL BLOCK                                                         //
///////////////////////////////////////////////////////////////////////////

#define BIT_0 0x01
#define BIT_1 0x02

void MouseInputTracker::ButtonControlBlock::advance() {
    const decltype(_status) lowestBit = _status & BIT_0;
    _status = ((_status << 1) & ~BIT_0) | lowestBit;
}

void MouseInputTracker::ButtonControlBlock::recordPress() {
    _status |= BIT_0;
}

void MouseInputTracker::ButtonControlBlock::recordRelease() {
    _status &= ~BIT_0;
}

bool MouseInputTracker::ButtonControlBlock::isPressed() const {
    return ((_status & BIT_0) != 0);
}

bool MouseInputTracker::ButtonControlBlock::isPressedEdge() const {
    // Is it pressed now but wasn't in the previous step?
    return (isPressed() && ((_status & BIT_1) == 0));
}

bool MouseInputTracker::ButtonControlBlock::isReleasedEdge() const {
    // Is it released now but wasn't in the previous step?
    return (!isPressed() && ((_status & BIT_1) != 0));
}

#undef BIT_1
#undef BIT_0

///////////////////////////////////////////////////////////////////////////
// INPUT TRACKER                                                         //
///////////////////////////////////////////////////////////////////////////

MouseInputTracker::MouseInputTracker(GetViewRelativeMousePosFunc   aGetViewRelativeMousePos,
                                     GetWindowRelativeMousePosFunc aGetWindowRelativeMousePos)
    : _getViewRelativeMousePos{std::move(aGetViewRelativeMousePos)}
    , _getWindowRelativeMousePos(std::move(aGetWindowRelativeMousePos))
{
    _controlBlocks.resize(static_cast<std::size_t>(MouseButton::ButtonCount));
}

MouseInput MouseInputTracker::getInput() const {
    return MouseInput{*this};
}

MouseInputMutator MouseInputTracker::getMutator() {
    return MouseInputMutator{};
}

void MouseInputTracker::prepForEvents() {
    _didMove = false;
    _verticalScrollDelta = 0.f;
    _horizontalScrollDelta = 0.f;

    for (auto& controlBlock : _controlBlocks) {
        controlBlock.advance();
    }

    _cursorWithinWindow.advance();
}

void MouseInputTracker::buttonEventOccurred(const sf::Event& aEvent) {
    auto button = MouseButton::Unknown;

    switch (aEvent.type) {
    case sf::Event::MouseWheelScrolled:
        if (aEvent.mouseWheelScroll.wheel == sf::Mouse::VerticalWheel) {
            _verticalScrollDelta = aEvent.mouseWheelScroll.delta;
            break;
        }
        if (aEvent.mouseWheelScroll.wheel == sf::Mouse::HorizontalWheel) {
            _horizontalScrollDelta = aEvent.mouseWheelScroll.delta;
            break;
        }
        break;

    case sf::Event::MouseButtonPressed:
        button = ToSPeMPEButton(aEvent.mouseButton.button);
        if (button != MouseButton::Unknown) {
            _controlBlocks[static_cast<std::size_t>(button)].recordPress();
        }
        break;

    case sf::Event::MouseButtonReleased:
        button = ToSPeMPEButton(aEvent.mouseButton.button);
        if (button != MouseButton::Unknown) {
            _controlBlocks[static_cast<std::size_t>(button)].recordRelease();
        }
        break;

    case sf::Event::MouseMoved:
        _didMove = true;
        break;

    case sf::Event::MouseEntered:
        _cursorWithinWindow.recordPress();
        break;

    case sf::Event::MouseLeft:
        _cursorWithinWindow.recordRelease();
        break;

    default:
        throw hobgoblin::TracedLogicError{"Invalid event passed!"};
    }
}

} // namespace spempe
} // namespace jbatnozic
