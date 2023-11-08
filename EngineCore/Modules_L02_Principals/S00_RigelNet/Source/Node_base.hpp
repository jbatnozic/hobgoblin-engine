#ifndef UHOBGOBLIN_RN_NODE_BASE_HPP
#define UHOBGOBLIN_RN_NODE_BASE_HPP

#include <Hobgoblin/RigelNet/Events.hpp>
#include <Hobgoblin/Utility/Any_ptr.hpp>

#include <deque>

#include <Hobgoblin/Private/Pmacro_define.hpp>

HOBGOBLIN_NAMESPACE_BEGIN
namespace rn {

class RN_NodeBase {
protected:
    std::deque<RN_Event> _eventQueue;
    util::AnyPtr _userData;
};

} // namespace rn
HOBGOBLIN_NAMESPACE_END

#include <Hobgoblin/Private/Pmacro_undef.hpp>

#endif // !UHOBGOBLIN_RN_NODE_BASE_HPP