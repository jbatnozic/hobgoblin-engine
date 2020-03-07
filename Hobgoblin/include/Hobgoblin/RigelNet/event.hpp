#ifndef UHOBGOBLIN_RN_EVENT_HPP
#define UHOBGOBLIN_RN_EVENT_HPP

#include <Hobgoblin/Common.hpp>
#include <Hobgoblin/Utility/Visitor.hpp>

#include <optional>
#include <string>
#include <utility>
#include <variant>

#include <Hobgoblin/Private/Pmacro_define.hpp>

HOBGOBLIN_NAMESPACE_START
namespace rn {

struct RN_Event {

    struct BadPassphrase {
        std::optional<PZInteger> clientIndex; // Not used on client side
        std::string incorrectPassphrase;
    };

    struct AttemptTimedOut {
    };

    struct Connected {
        std::optional<PZInteger> clientIndex; // Not used on client side
    };

    struct Disconnected {
        std::optional<PZInteger> clientIndex; // Not used on client side
        std::string reason;
    };

    struct ConnectionTimedOut {
        std::optional<PZInteger> clientIndex; // Not used on client side
    };

    using EventVariant = std::variant<
        BadPassphrase,
        AttemptTimedOut,
        Connected,
        Disconnected,
        ConnectionTimedOut>;

    template <class T>
    RN_Event(T&& arg)
        : eventVariant{std::forward<T>(arg)}
    {
    }

    EventVariant eventVariant;

    template <class ...Callables>
    void visit(Callables&&... callables) {
        std::visit(util::MakeVisitor(std::forward<Callables>(callables)...), eventVariant);
    }
};

class RN_Node;

namespace detail {
    
class EventFactory {
public:
    explicit EventFactory(RN_Node& node);
    explicit EventFactory(RN_Node& node, PZInteger clientIndex);

    void createBadPassphrase(std::string incorrectPassphrase) const;
    void createAttemptTimedOut() const;
    void createConnected() const;
    void createDisconnected(std::string reason) const;
    void createConnectionTimedOut() const;

private:
    RN_Node& _node;
    std::optional<PZInteger> _clientIndex;
};

} // namespace detail

} // namespace rn
HOBGOBLIN_NAMESPACE_END

#include <Hobgoblin/Private/Pmacro_undef.hpp>

#endif // !UHOBGOBLIN_RN_EVENT_HPP