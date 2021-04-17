#ifndef UHOBGOBLIN_RN_SOCKET_ADAPTER_HPP
#define UHOBGOBLIN_RN_SOCKET_ADAPTER_HPP

#include <Hobgoblin/Common.hpp>
#include <Hobgoblin/RigelNet/Configuration.hpp>
#include <Hobgoblin/RigelNet/Packet_wrapper.hpp>
#include <SFML/Network.hpp>

#define HOBGOBLIN_RN_ZEROTIER_SUPPORT // TODO Temp.
#ifdef  HOBGOBLIN_RN_ZEROTIER_SUPPORT
    #include <Ztcpp.hpp>
    namespace zt = jbatnozic::ztcpp;
#endif

#include <cstdint>
#include <variant>
#include <vector>

#include <Hobgoblin/Private/Pmacro_define.hpp>

HOBGOBLIN_NAMESPACE_START
namespace rn {

//! For now, always constructs an UDP socket, regardless of specified protocol
class RN_SocketAdapter {
public:
    RN_SocketAdapter(RN_Protocol aProtocol, RN_NetworkingStack aNetworkingStack);

    void init(PZInteger aRecvBufferSize);

    void bind(sf::IpAddress aIpAddress, std::uint16_t aLocalPort);

    //! Returns true on success.
    //! Throws exception on error.
    bool send(util::Packet& aPacket,
              const sf::IpAddress& aTargetAddress,
              std::uint16_t aTargetPort);

    //! Returns true if any data was received, false otherwise.
    //! Throws exception on error.
    bool recv(util::Packet& aPacket, 
              sf::IpAddress& aRemoteAddress, 
              std::uint16_t& aRemotePort);

    void close();

    std::uint16_t getLocalPort() const;

    RN_Protocol getProtocol() const noexcept;

    RN_NetworkingStack getNetworkingStack() const noexcept;

private:
    RN_Protocol _protocol;
    RN_NetworkingStack _networkingStack;

    std::variant<
        int, // Dummy
        sf::UdpSocket,
    #ifdef HOBGOBLIN_RN_ZEROTIER_SUPPORT
        zt::Socket
    #endif
    > _socket;

#ifdef HOBGOBLIN_RN_ZEROTIER_SUPPORT
    std::vector<std::uint8_t> _recvBuffer;
#endif
};

} // namespace rn
HOBGOBLIN_NAMESPACE_END

#include <Hobgoblin/Private/Pmacro_undef.hpp>

#endif // !UHOBGOBLIN_RN_SOCKET_ADAPTER_HPP