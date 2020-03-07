
#include <Hobgoblin/RigelNet/Event.hpp>
#include <Hobgoblin/RigelNet/Node.hpp>

#include <Hobgoblin/Private/Pmacro_define.hpp>

HOBGOBLIN_NAMESPACE_START
namespace rn {
namespace detail {

EventFactory::EventFactory(RN_Node& node)
    : _node{node}
    , _clientIndex{}
{
}

EventFactory::EventFactory(RN_Node& node, PZInteger clientIndex)
    : _node{node}
    , _clientIndex{clientIndex}
{
}

#define ENQUEUE_EVENT(...) _node._eventQueue.emplace_back(__VA_ARGS__)

void EventFactory::createBadPassphrase(std::string incorrectPassphrase) const {
    ENQUEUE_EVENT(RN_Event::BadPassphrase{_clientIndex, std::move(incorrectPassphrase)});
}

void EventFactory::createAttemptTimedOut() const {
    ENQUEUE_EVENT(RN_Event::AttemptTimedOut{});
}

void EventFactory::createConnected() const {
    ENQUEUE_EVENT(RN_Event::Connected{_clientIndex});
}

void EventFactory::createDisconnected(std::string reason) const {
    ENQUEUE_EVENT(RN_Event::Disconnected{_clientIndex, std::move(reason)});
}

void EventFactory::createConnectionTimedOut() const {
    ENQUEUE_EVENT(RN_Event::ConnectionTimedOut{_clientIndex});
}

} // namespace detail
} // namespace rn
HOBGOBLIN_NAMESPACE_END

#include <Hobgoblin/Private/Pmacro_undef.hpp>