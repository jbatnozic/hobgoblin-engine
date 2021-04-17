#ifndef UHOBGOBLIN_RN_CONFIGURATION_HPP
#define UHOBGOBLIN_RN_CONFIGURATION_HPP

#include <Hobgoblin/Common.hpp>

#include <chrono>
#include <functional>

#include <Hobgoblin/Private/Pmacro_define.hpp>

HOBGOBLIN_NAMESPACE_START
namespace rn {

enum class RN_Protocol {
    None, //! No protocol implemented (only on dummy nodes).
    TCP,  //! Transmission control protocol (NOT YET IMPLEMENTED).
    UDP   //! User datagram protocol.
};

enum class RN_NetworkingStack {
    Default,  //! Use socket implementation and networking stack of the host OS.
    ZeroTier  //! TODO Add description...
};

struct RN_ComposeForAllType {};
constexpr RN_ComposeForAllType RN_COMPOSE_FOR_ALL{};

using RN_RetransmitPredicate = std::function<bool(PZInteger, // Cycles since last transmit
                                                  std::chrono::microseconds, // Time since last send
                                                  std::chrono::microseconds  // Current latency
                                                  )>;

} // namespace rn
HOBGOBLIN_NAMESPACE_END

#include <Hobgoblin/Private/Pmacro_undef.hpp>
#include <Hobgoblin/Private/Short_namespace.hpp>

#endif // !UHOBGOBLIN_RN_CONFIGURATION_HPP