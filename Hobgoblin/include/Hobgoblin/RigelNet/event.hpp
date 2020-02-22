#ifndef UHOBGOBLIN_RN_EVENT_HPP
#define UHOBGOBLIN_RN_EVENT_HPP

#include <string>
#include <variant>

#include <Hobgoblin/Private/Pmacro_define.hpp>

HOBGOBLIN_NAMESPACE_START
namespace rn {

/*
    BadPassphrase
    AttemptTimedOut
    Connected
    Disconnected
    ConnectionTimedOut
    Kicked
*/

struct RN_EvBadPassphrase {
    int clientIndex; // Not used on client side
};

struct RN_EvAttemptTimedOut {
};

struct RN_EvConnected {
    int clientIndex; // Not used on client side
};

struct RN_EvDisconnected {
    int clientIndex; // Not used on client side
};

struct RN_EvConnectionTimedOut {
    int clientIndex; // Not used on client side
};

struct RN_EvKicked {
    std::string reason;
};

namespace detail {

using RN_EventBase = std::variant<
    RN_EvBadPassphrase,
    RN_EvAttemptTimedOut,
    RN_EvConnected,
    RN_EvDisconnected,
    RN_EvConnectionTimedOut,
    RN_EvKicked
>;

} // namespace detail

/*struct RN_Event : public detail::RN_EventBase {
    using detail::RN_EventBase::RN_EventBase;

    template <class ...Args>
    void visit(Args&&... args) {
        std::visit(std::forward<Args>(args)..., Self);
    }
};*/

using RN_Event = detail::RN_EventBase; // TODO

} // namespace rn
HOBGOBLIN_NAMESPACE_END

#include <Hobgoblin/Private/Pmacro_undef.hpp>

#endif // !UHOBGOBLIN_RN_EVENT_HPP