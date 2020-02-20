#ifndef UHOBGOBLIN_RN_REMOTE_INFO_HPP
#define UHOBGOBLIN_RN_REMOTE_INFO_HPP

#include <Hobgoblin/Utility/stopwatch.hpp>

#include <SFML/Network.hpp>

#include <chrono>
#include <cstdint>

#include <Hobgoblin/Private/pmacro_define.hpp>

HOBGOBLIN_NAMESPACE_START
namespace rn {

enum class RN_RemoteStatus {
    Disconnected,
    Connected
};

struct RN_RemoteInfo {
    RN_RemoteStatus status = RN_RemoteStatus::Disconnected;
    util::Stopwatch timeoutStopwatch;
    std::chrono::microseconds latency;
    sf::IpAddress ipAddress;
    std::uint16_t port;
};

} // namespace rn
HOBGOBLIN_NAMESPACE_END

#include <Hobgoblin/Private/pmacro_undef.hpp>

#endif // !UHOBGOBLIN_RN_REMOTE_INFO_HPP