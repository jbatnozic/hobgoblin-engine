// Copyright 2024 Jovan Batnozic. Released under MS-PL licence in Serbia.
// See https://github.com/jbatnozic/Hobgoblin?tab=readme-ov-file#licence

// clang-format off


#include <Hobgoblin/RigelNet/Events.hpp>

#include <Hobgoblin/Private/Pmacro_define.hpp>

HOBGOBLIN_NAMESPACE_BEGIN
namespace rn {
namespace rn_detail {

EventFactory::EventFactory(const std::vector<RN_EventListener*>& aEventListeners)
    : _eventListeners{aEventListeners}
    , _clientIndex{}
{
}

EventFactory::EventFactory(const std::vector<RN_EventListener*>& aEventListeners, PZInteger clientIndex)
    : _eventListeners{aEventListeners}
    , _clientIndex{clientIndex}
{
}

#define DISPATCH_EVENT(...) \
    do { \
        const RN_Event event = (__VA_ARGS__); \
        for (const auto& listener : _eventListeners) { \
            listener->onNetworkingEvent(event); \
        } \
    } while (0)

void EventFactory::createBadPassphrase(std::string incorrectPassphrase) const {
    DISPATCH_EVENT(RN_Event::BadPassphrase{_clientIndex, std::move(incorrectPassphrase)});
}

void EventFactory::createConnectAttemptFailed(RN_Event::ConnectAttemptFailed::Reason reason) const {
    DISPATCH_EVENT(RN_Event::ConnectAttemptFailed{reason});
}

void EventFactory::createConnected() const {
    DISPATCH_EVENT(RN_Event::Connected{_clientIndex});
}

void EventFactory::createDisconnected(RN_Event::Disconnected::Reason reason, std::string message) const {
    DISPATCH_EVENT(RN_Event::Disconnected{_clientIndex, reason, std::move(message)});
}

} // namespace rn_detail
} // namespace rn
HOBGOBLIN_NAMESPACE_END

#include <Hobgoblin/Private/Pmacro_undef.hpp>

// clang-format on
