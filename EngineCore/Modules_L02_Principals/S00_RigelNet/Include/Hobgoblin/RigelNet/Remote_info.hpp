#ifndef UHOBGOBLIN_RN_REMOTE_INFO_HPP
#define UHOBGOBLIN_RN_REMOTE_INFO_HPP

#include <Hobgoblin/Utility/Time_utils.hpp>

#include <SFML/Network.hpp>

#include <chrono>
#include <cstdint>

#include <Hobgoblin/Private/Pmacro_define.hpp>

HOBGOBLIN_NAMESPACE_BEGIN
namespace rn {

struct RN_RemoteInfo {
    //! The stopwatch showing the elapsed time since the last contact from
    //! the related remote. Each time a message is received from that remote,
    //! the stopwatch restarts.
    util::Stopwatch timeoutStopwatch;

    //! Latency to the remote.
    //! In case of a lag spike after which many packets arrive at once,
    //! this value should give you roughly something between the
    //! 'optimistic' and 'pessimistic' latencies (see below).
    //! During normal operation with a stable connection, all 3 latencies
    //! should have similar values.
    //! IMPORTANT: This is the round trip latency! If you need an 
    //! (estimated) one-direction delay, divide this by 2.
    std::chrono::microseconds meanLatency;

    //! Latency to the remote.
    //! In case of a lag spike after which many packets arrive at once,
    //! the 'optimistic' latency will take into account only the most
    //! recent packet, so it shouldn't spike much (if at all).
    //! During normal operation with a stable connection, all 3 latencies
    //! should have similar values.
    //! IMPORTANT: This is the round trip latency! If you need an 
    //! (estimated) one-direction delay, divide this by 2.
    std::chrono::microseconds optimisticLatency;

    //! Latency to the remote.
    //! In case of a lag spike after which many packets arrive at once,
    //! the 'pessimistic' latency will also briefly spike.
    //! During normal operation with a stable connection, all 3 latencies
    //! should have similar values.
    //! IMPORTANT: This is the round trip latency! If you need an 
    //! (estimated) one-direction delay, divide this by 2.
    std::chrono::microseconds pessimisticLatency;

    //! IPv4 network address
    sf::IpAddress ipAddress;

    //! Port number this remote is using
    std::uint16_t port;

    RN_RemoteInfo(sf::IpAddress ipAddress, std::uint16_t port)
        : timeoutStopwatch{}
        , meanLatency{std::chrono::microseconds{-1}}
        , optimisticLatency{std::chrono::microseconds{-1}}
        , pessimisticLatency{std::chrono::microseconds{-1}}
        , ipAddress{ipAddress}
        , port{port}
    {
    }

    RN_RemoteInfo()
        : RN_RemoteInfo{sf::IpAddress::None, 0}
    {
    }
};

} // namespace rn
HOBGOBLIN_NAMESPACE_END

#include <Hobgoblin/Private/Pmacro_undef.hpp>
#include <Hobgoblin/Private/Short_namespace.hpp>

#endif // !UHOBGOBLIN_RN_REMOTE_INFO_HPP
