
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

void EventFactory::createConnectAttemptFailed(RN_Event::ConnectAttemptFailed::Reason reason) const {
    ENQUEUE_EVENT(RN_Event::ConnectAttemptFailed{reason});
}

void EventFactory::createConnected() const {
    ENQUEUE_EVENT(RN_Event::Connected{_clientIndex});
}

void EventFactory::createDisconnected(RN_Event::Disconnected::Reason reason, std::string message) const {
    ENQUEUE_EVENT(RN_Event::Disconnected{_clientIndex, reason, std::move(message)});
}

} // namespace detail
} // namespace rn
HOBGOBLIN_NAMESPACE_END

#include <Hobgoblin/Private/Pmacro_undef.hpp>