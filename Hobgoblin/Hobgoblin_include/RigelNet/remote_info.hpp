#ifndef UHOBGOBLIN_RN_REMOTE_INFO_HPP
#define UHOBGOBLIN_RN_REMOTE_INFO_HPP

#include <SFML/Network.hpp>
#include <SFML/System/Clock.hpp>

#include <chrono>
#include <cstdint>

#include <Hobgoblin_include/Private/pmacro_define.hpp>

HOBGOBLIN_NAMESPACE_START
namespace rn {

enum class RN_RemoteStatus {
    Disconnected,
    Connected
};

struct RN_RemoteInfo {
    RN_RemoteStatus status = RN_RemoteStatus::Disconnected;
    sf::Clock timeoutClock; // TODO Replace with hg::Clock
    std::chrono::microseconds latency;
    sf::IpAddress ipAddress;
    std::uint16_t port;
};

} // namespace rn
HOBGOBLIN_NAMESPACE_END

#include <Hobgoblin_include/Private/pmacro_undef.hpp>

#endif // !UHOBGOBLIN_RN_REMOTE_INFO_HPP