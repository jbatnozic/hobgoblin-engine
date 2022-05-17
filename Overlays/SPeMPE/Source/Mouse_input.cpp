
#include <SPeMPE/Other/Mouse_input.hpp>

#include <utility>

namespace jbatnozic {
namespace spempe {

///////////////////////////////////////////////////////////////////////////
// MOUSE INPUT TRACKER                                                   //
///////////////////////////////////////////////////////////////////////////

MouseInputTracker::MouseInputTracker(GetMousePosFunc aGetMousePos)
    : _getMousePos{std::move(aGetMousePos)}
{
}

sf::Vector2f MouseInputTracker::getMousePos(hobgoblin::PZInteger aViewIndex) const {
    return _getMousePos(aViewIndex);
}

} // namespace spempe
} // namespace jbatnozic
