#ifndef UHOBGOBLIN_RN_EVENT_HPP
#define UHOBGOBLIN_RN_EVENT_HPP

#include <Hobgoblin/Utility/Visitor.hpp>

#include <string>
#include <variant>

#include <Hobgoblin/Private/Pmacro_define.hpp>

HOBGOBLIN_NAMESPACE_START
namespace rn {

struct RN_Event {

    struct BadPassphrase {
        int clientIndex; // Not used on client side
    };

    struct AttemptTimedOut {
    };

    struct Connected {
        int clientIndex; // Not used on client side
    };

    struct Disconnected {
        int clientIndex; // Not used on client side
    };

    struct ConnectionTimedOut {
        int clientIndex; // Not used on client side
    };

    struct Kicked {
        std::string reason;
    };

    using EventVariant = std::variant<
        BadPassphrase,
        AttemptTimedOut,
        Connected,
        Disconnected,
        ConnectionTimedOut,
        Kicked>;

    EventVariant eventVariant;

    template <class ...Callables>
    void visit(Callables&&... callables) {
        std::visit(util::MakeVisitor(std::forward<Callables>(callables)...), eventVariant);
    }
};

} // namespace rn
HOBGOBLIN_NAMESPACE_END

#include <Hobgoblin/Private/Pmacro_undef.hpp>

#endif // !UHOBGOBLIN_RN_EVENT_HPP