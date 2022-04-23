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
    //! TODO
    util::Stopwatch timeoutStopwatch;

    //! TODO
    //! IMPORTANT: This is the round trip latency! If you need an 
    //! (estimated) one-direction delay, divide this by 2.
    std::chrono::microseconds meanLatency;

    //! TODO
    //! IMPORTANT: This is the round trip latency! If you need an 
    //! (estimated) one-direction delay, divide this by 2.
    std::chrono::microseconds optimisticLatency;

    //! TODO
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
